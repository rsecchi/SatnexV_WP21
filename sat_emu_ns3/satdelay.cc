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


using namespace ns3;


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


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();  


  Simulator::Run ();
}
