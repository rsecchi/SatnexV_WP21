#include <iostream>
#include <fstream>
#include <sstream>
#include "ns3/csma-module.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/traffic-control-helper.h"
#include "ns3/tap-bridge-helper.h"
#include "ns3/red-queue-disc.h"
#include "ns3/fifo-queue-disc.h"
#include "ns3/fq-codel-queue-disc.h"

#define NOW (Simulator::Now ().GetSeconds ())

using namespace ns3;

uint64_t initDeq;
double initTime;

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

static void
ChangeRate (Ptr<PointToPointNetDevice> dp)
{
  static const std::string rateON = "5Mbps";
  static const std::string rateOFF = "500kbps";
  static enum {ON, OFF} channel_state = ON;
  static const double channel_update = 0.1;

  if (channel_state == ON) {
    channel_state = OFF;
    dp->SetAttribute ("DataRate", StringValue(rateOFF));
    //std::cout << "CR " << NOW << " " << rateON << std::endl;
    //std::cout << "CR " << NOW << " " << rateOFF << std::endl;
  }
  else
  if (channel_state == OFF) {
    channel_state = ON;
    dp->SetAttribute ("DataRate", StringValue(rateON));
    //std::cout << "CR " << NOW << " " << rateOFF << std::endl;
    //std::cout << "CR " << NOW << " " << rateON << std::endl;
  }

  Simulator::Schedule(Seconds(channel_update), &ChangeRate, dp);
}



static void
PeriodicPrint (Ptr<QueueDisc> qp, Ptr<Queue<Packet> > netqueue)
{
  static const double sample_update = 0.02;
  static uint32_t prev_sent = 0;

  uint32_t current = qp->GetStats().nTotalSentBytes * 8;
  uint32_t delta   = current - prev_sent;

  prev_sent = current;

  std::cout << "Q " << NOW << " " << qp->GetCurrentSize () << " "
       << netqueue->GetCurrentSize () << " "
       << delta/sample_update << std::endl;

  Simulator::Schedule (Seconds(sample_update), &PeriodicPrint, qp, netqueue);

}

///////////////////////////////////////////////////////////
///////// QueueDisc Period Utilisation measurement ////////

static void
PrintResults(Ptr<QueueDisc> qp)
{
  const QueueDisc::Stats stats = qp->GetStats();
  uint64_t totDeq = stats.nTotalDequeuedBytes - initDeq;
  double period = NOW - initTime;

  double tput = 8*totDeq/period;
  double util = tput/5e6; 

  std::cout << period << " " << totDeq << " " 
            << tput   << " " << util   << " " 
            << qp->GetCurrentSize () << std::endl;
}

void
TurnUpRate (Ptr<QueueDisc> qp, Ptr<PointToPointNetDevice> dp)
{
  static const double stats_update = 0.2;
  dp->SetAttribute ("DataRate", StringValue("5Mbps"));
  const QueueDisc::Stats stats = qp->GetStats();
  initDeq = stats.nTotalDequeuedBytes;
  initTime = NOW;

  for(uint32_t i=1; i<20; i++)
    Simulator::Schedule(Seconds(i*stats_update), &PrintResults, qp);
}

void
TurnDownRate (Ptr<QueueDisc> qp, Ptr<PointToPointNetDevice> dp)
{
  // static const double stats_update = 0.2;
  dp->SetAttribute ("DataRate", StringValue("500kbps"));
  // const QueueDisc::Stats stats = qp->GetStats();
  // initDeq = stats.nTotalDequeuedBytes;
  // Simulator::Schedule(Seconds(stats_update), &PrintResults, qp);
  // Simulator::Schedule(Seconds(2*stats_update), &PrintResults, qp);
  // Simulator::Schedule(Seconds(3*stats_update), &PrintResults, qp);
  // Simulator::Schedule(Seconds(4*stats_update), &PrintResults, qp);
}


/////////////////////////////////////
// Internal Console to update commands
class  InternalConsole
{
public:

  InternalConsole(Ptr<Node> node, 
      Ptr<PointToPointNetDevice> p2p, Ptr<QueueDisc> qdiscp);

  void
  ReceiveCommand(Ptr<Socket> sock)
  {
    uint8_t local_buffer[1500];
    std::string token;

    Address from;
    Ptr<Packet> packet = sock->RecvFrom(from);

    uint32_t copied = packet->CopyData(local_buffer, 1500);
    local_buffer[copied] = 0;

    std::string cmd = (char*) local_buffer; 
    std::istringstream str(cmd);

    if (str >> token) {

      if (token == "turn_up_rate") 
        TurnUpRate(m_queuedisc, m_netdev);
      

      if (token == "turn_down_rate")
        TurnDownRate(m_queuedisc, m_netdev);
    }

    sock->Send(local_buffer, copied, 0);

  }

  void
  IncomingCommand(Ptr<Socket> s, const Address& from);

private:

  Ptr<Socket> m_deamon_sock;

  Ptr<PointToPointNetDevice> m_netdev;
  Ptr<QueueDisc> m_queuedisc;

};
  
InternalConsole::InternalConsole(Ptr<Node> node, 
    Ptr<PointToPointNetDevice> p2p, Ptr<QueueDisc> qdiscp)
{

  m_netdev = p2p;
  m_queuedisc = qdiscp;

  // -------- Setup Socket for communication with host --------///
  Address localaddr(InetSocketAddress (Ipv4Address::GetAny(), 4444));  
  Ptr<Socket> sock = Socket::CreateSocket (node, TcpSocketFactory::GetTypeId());
  sock->Bind(localaddr);
  sock->Listen();
  sock->SetAcceptCallback (MakeNullCallback<bool, Ptr<Socket>, const Address&> (),
    MakeCallback(&InternalConsole::IncomingCommand, this));
  // -------- Use Socket to communication with outsize --------///

}


void
InternalConsole::IncomingCommand(Ptr<Socket> s, const Address& from)
{
  s->SetRecvCallback (MakeCallback (&InternalConsole::ReceiveCommand, this));
}
////\\\\\\\\\\\\\\\\\\\//////////////////

int 
main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));


  /* Access network (LAN) */
  NodeContainer nodes; 
  nodes.Create (2);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  NetDeviceContainer devices = csma.Install (nodes);

  TapBridgeHelper tapBridge;
  tapBridge.SetAttribute ("Mode", StringValue ("UseLocal"));
  tapBridge.SetAttribute ("DeviceName", StringValue ("tap-left"));
  tapBridge.Install (nodes.Get (0), devices.Get (0));


  /* Creating bottleneck link */

  NodeContainer linknodes;
  linknodes.Create (1);
  linknodes.Add (nodes.Get (1));
 
  InternetStackHelper internet;
  internet.Install(linknodes);

 
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue ("500Kbps"));
  p2p.SetChannelAttribute("Delay", StringValue ("50ms"));

  NetDeviceContainer devs = p2p.Install (linknodes);

  /* Configuring AQM bottleneck */

  ////// RED QUEUE DISC ////// 
  
  TrafficControlHelper tch;
  tch.SetRootQueueDisc("ns3::RedQueueDisc");
  
  QueueDiscContainer qdc = tch.Install(devs.Get (1));
  Ptr<RedQueueDisc> qdisc = qdc.Get (0)->GetObject<RedQueueDisc> ();
  qdisc->SetAttribute("MaxSize", QueueSizeValue(QueueSize("100p")));
  qdisc->SetAttribute("MinTh", DoubleValue(2.0));
  qdisc->SetAttribute("MaxTh", DoubleValue(20.0));
  //qdisc->SetAttribute("LInterm", DoubleValue(400.0));
  qdisc->SetAttribute("UseEcn", BooleanValue(true));  
  

  ////// RED QUEUE DISC ////// 

  /*
  TrafficControlHelper tch;
  tch.SetRootQueueDisc("ns3::FqCoDelQueueDisc");
  QueueDiscContainer qdc = tch.Install(devs.Get (1));
  Ptr<FqCoDelQueueDisc> qdisc = qdc.Get (0)->GetObject<FqCoDelQueueDisc> ();
  qdisc->SetAttribute("MaxSize", QueueSizeValue(QueueSize("10p")));
  */

  ////// Fifo QUEUE DISC ////// 
  /*
  TrafficControlHelper tch;
  tch.SetRootQueueDisc("ns3::FifoQueueDisc");
  
  QueueDiscContainer qdc = tch.Install(devs.Get (1));
  Ptr<FifoQueueDisc> qdisc = qdc.Get (0)->GetObject<FifoQueueDisc> ();
  qdisc->SetAttribute("MaxSize", QueueSizeValue(QueueSize("100p")));
  */
  
  /* Netdev queue set to a small value */
  Ptr<PointToPointNetDevice> pp = devs.Get (1)->GetObject<PointToPointNetDevice> ();
  Ptr<Queue<Packet>> netqueue = pp->GetQueue ();
  netqueue->SetAttribute ("MaxSize", StringValue("2p"));


  /* ******************** */

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer ipv4cont = ipv4.Assign (devs);


  ipv4.SetBase ("10.1.1.0", "255.255.255.0", "0.0.0.2");
  ipv4.Assign (NetDeviceContainer (devices.Get (1)));

  NodeContainer nodes2;
  nodes2.Create (1);
  nodes2.Add (linknodes.Get (0));
  


  /* Access network (LAN) */
  NetDeviceContainer devices2 = csma.Install(nodes2);
  tapBridge.SetAttribute ("DeviceName", StringValue ("tap-right"));
  tapBridge.Install (nodes2.Get (0), devices2.Get (0));

  ipv4.SetBase ("10.1.2.0", "255.255.255.0", "0.0.0.2");
  ipv4.Assign (NetDeviceContainer (devices2.Get (1)));

  // csma.EnablePcapAll("debug", false);
  // p2p.EnablePcapAll("p2p_pcap");


  /*
  uint16_t port = 50000;
  Address localaddr(InetSocketAddress (Ipv4Address::GetAny(), port));
  PacketSinkHelper sink_helper ("ns3::TcpSocketFactory", localaddr);
  ApplicationContainer sinkApp = sink_helper.Install(nodes.Get (1));
  sinkApp.Start(Seconds(0.0));
  */ 

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();  

/*
  Ptr<Node> mynode = nodes.Get (1);
  Ipv4StaticRoutingHelper helper;

  Ptr<Ipv4> ipv4obj = mynode->GetObject<Ipv4>();
  Ptr<Ipv4StaticRouting> ipv4stat = helper.GetStaticRouting(ipv4obj);

  ipv4stat->LookupStatic("10.1.2.1");

  // Ipv4stat->SetDefaultRoute(....)


  Ptr<GlobalRouter> gr = mynode->GetObject<GlobalRouter>();
  gr->InjectRoute("0.0.0.0", "0.0.0.0");
  Ipv4GlobalRoutingHelper::RecomputeRoutingTables();

  Ipv4StaticRoutingHelper helper;
  Ptr<Ipv4> stack = mynode -> GetObject<Ipv4>();
  Ptr<Ipv4StaticRouting> staticrouting = helper.GetStaticRouting(stack);

  uint32_t numroutes = staticrouting->GetNRoutes();

  Ipv4RoutingTableEntry entry;
  std::cout << "Routing table for device: " <<
      Names::FindName(mynode) << "\n";
  std::cout << "Destination\tMask\t\tGateway\t\tIface\n";

  for (uint32_t i =0 ; i<numroutes;i++) {
     entry = staticrouting->GetRoute(i);
     std::cout << entry.GetDestNetwork() << "\t"
               << entry.GetDestNetworkMask() << "\t"
               << entry.GetGateway() << "\t\t"
               << entry.GetInterface() << "\n";
  }
*/

  // InternalConsole cons(nodes.Get(1), pp, qdisc); 

  // Simulator::Schedule (Seconds (0.0), &PeriodicPrint, qdisc, netqueue); 
  // Simulator::Schedule (Seconds (0.0), &ChangeRate, pp);
  // Simulator::Schedule (Seconds (5.0), &TurnUpRate, qdisc, pp);
  Simulator::Run ();
}
