#include <stdlib.h>
#include <pcap.h>
#include <stdio.h>
#include <limits.h>

/* CFLAGS=-lpcap make udprate */

#define MAX_FEATURES   32

int started;
int num_intv, col;
long long feature[MAX_FEATURES];	
struct timeval start_time;
long long curr_data;

long long tick_us;
long long begin_us, end_us;
long long now;
long long bin_end;

char* label;

long long delta(struct timeval t1, struct timeval t2)
{
	long long int x;

	x = 1000000UL * (t1.tv_sec - t2.tv_sec);
	x += t1.tv_usec - t2.tv_usec;

	return x;
}

void main_loop(u_char* user, const struct pcap_pkthdr *h, const u_char *bytes)
{

	int k;

	if (!started)
	{
		start_time = h->ts;
		started = 1;
		bin_end = tick_us;
		
		curr_data = h->caplen;
		return;
	}

	now = delta(h->ts, start_time);

	while  (now > bin_end) {
		if (now > begin_us && now<end_us) {
			feature[col] = curr_data;
			col++;
			if (col==num_intv) {
				for(k=0; k<num_intv-1; k++)
					printf("%7lld,", feature[k]);
				printf("%7lld, %s\n", feature[num_intv-1], label);
				col = 0;
			}
			
		}
		bin_end += tick_us;
		curr_data = 0;
	}

	curr_data += h->caplen;

}

int main(int argc, char* argv[])
{
	char err_string[PCAP_ERRBUF_SIZE];
	pcap_t *handle = NULL;

	if (argc!=7) {
		fprintf(stderr, "usage: %s <file> <interv_ms> "
						"<num_intv> <start_time> "
						"<end_time> <label>\n", 
			argv[0]);
		exit(1);
	}

	handle = pcap_open_offline(argv[1], err_string);
	if (!handle) {
		fprintf(stderr, "%s\n",err_string);
		exit(1);
	}

	tick_us = atoll(argv[2]) * 1000;
	if (tick_us == 0) {
		fprintf(stderr, "invalid <interv_ms>\n");
		exit(1);
	}

	num_intv = atoi(argv[3]);
	begin_us = atof(argv[4]) * 1e6;
	end_us   = atof(argv[5]) * 1e6;
	if (end_us < 0)
		end_us = LLONG_MAX;	

	label = argv[6];

	pcap_loop(handle, -1, main_loop, NULL);
}

