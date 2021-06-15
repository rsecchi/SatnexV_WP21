#include <stdlib.h>
#include <pcap.h>
#include <stdio.h>
#include <limits.h>

/* CFLAGS=-lpcap make udprate */

#define MAX_FEATURES   512

int started;
int num_intv, col;
long long feature[MAX_FEATURES];	
struct timeval start_time;
long long curr_data;

long long tick_us;
long long begin_us, end_us, end_time;
long long now;
long long bin_end;

char* label;
double bitrate;
double scale_factor;

long long total_data;
long long duration_us;

long long delta(struct timeval t1, struct timeval t2)
{
	long long int x;

	x = 1000000UL * (t1.tv_sec - t2.tv_sec);
	x += t1.tv_usec - t2.tv_usec;

	return x;
}

void pre_loop(u_char* user, const struct pcap_pkthdr *h, const u_char *bytes)
{
	if (!started)
	{
		start_time = h->ts;
		started = 1;
		return;
	}

	now = delta(h->ts, start_time);
	
	if (now >= begin_us && now <= end_us) {
		total_data += h->caplen;
		end_time = now;
	}
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
					printf("%10.5lf,", feature[k]*scale_factor);
				printf("%10.5lf, %s\n", 
						feature[num_intv-1]*scale_factor,
					 	label);
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
	int i;
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

	pcap_loop(handle, -1, pre_loop, NULL);
	pcap_close(handle);
	duration_us = end_time - begin_us;
	bitrate = (((double)(total_data)) / duration_us)*1e6;
	scale_factor = 1e6/(bitrate*tick_us);

	/*
	printf("total data: %lld\n", total_data);
	printf("total duration (us): %lld\n", duration_us);
	printf("bitrate (bps): %lf\n", bitrate);
	printf("scale_factor: %.15lf\n", scale_factor);

	exit(0);
	*/

	// print header
	for(i=0; i<num_intv; i++)
		printf("F%d,", i);
	printf("label\n");

	handle = pcap_open_offline(argv[1], err_string);
	started = 0;
	pcap_loop(handle, -1, main_loop, NULL);
	fprintf(stderr, "bitrate (bps): %lf\n", bitrate);
}

