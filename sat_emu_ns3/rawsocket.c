#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <linux/if.h>
#include <linux/if_tun.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>


#define MAX_FRAME_LEN   1500
#define ETH_LEN           14
#define FLAGS              0 
char tun_name[] = "tun0";
char tap_name[] = "tap-left";
uint8_t tap_src_hwaddr[] = {0x56, 0x18, 0x4c, 0x7e, 0xd7, 0xf7};
uint8_t tap_dst_hwaddr[] = {0x80, 0x00, 0x2e, 0x00, 0x00, 0x01};

int counter;

int get_card_no(int pf, char* name)
{
	struct ifreq card;

	strcpy(card.ifr_name, name);
	if (ioctl(pf, SIOCGIFINDEX, &card) == -1) {
		perror("SIOCGIFINDEX");
		exit(1);
	}

	return card.ifr_ifindex;
}

void printpacket(struct sockaddr_ll* dev) 
{
	printf("%di -------- \n", ++counter);
	printf("sll_family   %x\n", dev->sll_family);
	printf("sll_protocol %x\n", dev->sll_protocol);
	printf("sll_ifindex  %x\n", dev->sll_ifindex);
	printf("sll_hatype   %x\n", dev->sll_hatype);
	printf("sll_pkttype  %x\n", dev->sll_pkttype);
	printf("sll_halen    %x\n", dev->sll_halen);
	printf("ssl_addr:    %02x:%02x:%02x:%02x:%02x:%02x\n",
					dev->sll_addr[0],
					dev->sll_addr[1],
					dev->sll_addr[2],
					dev->sll_addr[3],
					dev->sll_addr[4],
					dev->sll_addr[5]);


}

void copyaddr(uint8_t* dst, uint8_t* src)
{
	for(int i=0; i<6; i++)
		dst[i] = src[i];
}

int main(int argc, char **argv)
{
	int cnt, sock_tap, sock_tun, sock2;
	int tun, tap, dev;

	unsigned char buf_frame[MAX_FRAME_LEN + ETH_LEN];
	struct sockaddr_ll device;
	struct ether_header* ethp = (struct ether_header*) buf_frame;

	struct sockaddr_ll tun_addr, tap_addr;
	struct sockaddr_in tun_addr_dst;
	socklen_t size_in;
	const socklen_t size_out = sizeof(device);

	struct sockaddr myaddr;
	socklen_t mysize;


	/* create a L2 RAW socket and bind it to tap */
	if ((sock_tap = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))== -1) {
		perror("socket() failed");
		exit(1);
	}
	memset(&tap_addr, 0, size_out);
	tap = get_card_no(sock_tap, tap_name);
	tap_addr.sll_family = AF_PACKET;
	tap_addr.sll_protocol = htons(ETH_P_IP);
	tap_addr.sll_ifindex = tap;
	tap_addr.sll_halen = 6;
	copyaddr(tap_addr.sll_addr, tap_src_hwaddr);
	bind(sock_tap, (struct sockaddr*)& tap_addr, size_out);	


	/* create a L2 RAW socket and bind it to tun */
	if ((sock_tun = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))== -1) {
		perror("socket() failed");
		exit(1);
	}
	memset(&tun_addr, 0, size_out);
	tun = get_card_no(sock_tun, tun_name);
	tun_addr.sll_family = AF_PACKET;
	tun_addr.sll_protocol = htons(ETH_P_IP);
	tun_addr.sll_ifindex = tun;
	bind(sock_tun, (struct sockaddr*)& tun_addr, size_out);	


	/* Destinations addresses */
	/* init MAC header (to send over tap) */
	copyaddr(ethp->ether_dhost, tap_dst_hwaddr);
	copyaddr(ethp->ether_shost, tap_src_hwaddr);
	ethp->ether_type = htons(ETHERTYPE_IP);


	/* create an L3 RAW socket to send packet to TUN */
	if ((sock2 = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
		perror("socket() failed");
		exit(1);
	}
	int on = 1;
	if (setsockopt(sock2, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
		perror("setsockopt() for IP_HDRINCL error");
		exit(1);
	}

	if (fork()) {

			// received from TUN and sent to TAP
			while(1) {
				cnt = recvfrom(sock_tun, buf_frame + ETH_LEN, MAX_FRAME_LEN, 
							FLAGS, &myaddr, &mysize);

				if (cnt<0) perror("raw error");
				//printpacket((struct sockaddr_ll*)&myaddr);
			
				cnt = sendto(sock_tap, 	buf_frame, cnt + ETH_LEN,
							FLAGS, (struct sockaddr*)&tap_addr, size_out);
				if (cnt<0) perror("raw error");

			}
	}	
	else {

			// received from TAP and sent to TUN
			while(1) {
				cnt = recvfrom(sock_tap, buf_frame, MAX_FRAME_LEN, 
							FLAGS, &myaddr, &mysize);
				// printpacket((struct sockaddr_ll*)&myaddr);
					//

				struct iphdr* ipp = (struct iphdr*)(buf_frame + ETH_LEN);
				uint8_t *ip_src = (uint8_t*)(&(ipp->saddr));
				uint8_t *ip_dst = (uint8_t*)(&(ipp->daddr));

				/*
				printf("%d.%d.%d.%d -> %d.%d.%d.%d\n",
					ip_src[0], ip_src[1], ip_src[2], ip_src[3],
					ip_dst[0], ip_dst[1], ip_dst[2], ip_dst[3]
				);
				*/				

				tun_addr_dst.sin_family =  AF_INET;
				tun_addr_dst.sin_addr.s_addr = ipp->daddr; 

				cnt = sendto(sock2, buf_frame + ETH_LEN, cnt - ETH_LEN, 
							FLAGS, (struct sockaddr*)&tun_addr_dst, 
							sizeof(tun_addr_dst));

				if (cnt<0) perror("");
			}
	}

}
 

