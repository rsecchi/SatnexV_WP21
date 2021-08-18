#include <stdlib.h>
#include <pcap.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>

/* compile with:
gcc -Ilibpcap/pcap/ -Ilibpcap udprate.c libpcap/libpcap.a -libverbs -o udprate -ldbus-1 -lm
*/

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

/* normalization variables */
char* norm_types[] = {"none", "bitrate", "stdnorm", "minmax", "cap"};
int norm_type;

double bitrate;
double scale_factor;
double variance, stddev;
long long max;

long long total_data;
long long total_data2;
unsigned int n_data;
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

	int k;

	if (!started)
	{
		start_time = h->ts;
		started = 1;
		bin_end = tick_us;
		
		curr_data = h->len;
		return;
	}

	now = delta(h->ts, start_time);

	while  (now > bin_end) {
		if (now > begin_us && now < end_us) {
			total_data  += curr_data;
			end_time = now;
			total_data2 += curr_data * curr_data;
			n_data++;
			if (curr_data>=max)
				max = curr_data;
		}
		bin_end += tick_us;
		curr_data = 0;
	}

	curr_data += h->len;
}


double norm(long long feat)
{
	switch(norm_type)
	{
		case 0:		// not normalized (default)
			return feat;

		case 1:     // normalized to the average bitrate
			return (feat*1e6/tick_us)/bitrate;

		case 2:     // standard normalization
			return (feat - bitrate*tick_us/1e6)/stddev;

		case 3:     // divided by max
			return  ((double)feat)/max;

		defaut:
			return feat;
	}


	return feat;
}


void main_loop(u_char* user, const struct pcap_pkthdr *h, const u_char *bytes)
{

	int k;

	if (!started)
	{
		start_time = h->ts;
		started = 1;
		bin_end = tick_us;
		
		curr_data = h->len;
		return;
	}

	now = delta(h->ts, start_time);

	while  (now > bin_end) {
		if (now > begin_us && now<end_us) {
			feature[col] = curr_data;
			col++;
			if (col==num_intv) {
				for(k=0; k<num_intv; k++)
					printf("%10.5lf,", norm(feature[k]));
				printf("%s\n", label);
				col = 0;
			}
			
		}
		bin_end += tick_us;
		curr_data = 0;
	}

	curr_data += h->len;
}

int main(int argc, char* argv[])
{
	int i;
	char err_string[PCAP_ERRBUF_SIZE], *norm_env;
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

	/* normalization */
	/* retrieve type of normalization from env */
	norm_env = getenv("NORM_TYPE");
	if (norm_env)
		for(i=0; i<sizeof(norm_types)/sizeof(char*); i++)
			if (!strcmp(norm_env, norm_types[i])) norm_type = i;

	/* retrieve statistics for normalization */
	pcap_loop(handle, -1, pre_loop, NULL);
	pcap_close(handle);

	/* normalization variables */
	duration_us = end_time - begin_us;
	bitrate = (((double)(total_data)) / duration_us)*1e6;
	scale_factor = 1e6/(bitrate*tick_us);
	variance = (double)(total_data2 - total_data*total_data/n_data)/(n_data - 1);
	stddev = sqrt(variance);	

	/*
	printf("total data: %lld\n", total_data);
	printf("total data2: %lld\n", total_data2);
	printf("n_data: %d\n", n_data);
	printf("max: %lld\n", max);
	printf("total duration (us): %lld\n", duration_us);
	printf("bitrate (bps): %lf\n", bitrate);
	printf("scale_factor: %.15lf\n", scale_factor);
	printf("standard deviation: %.15f\n", stddev);

	exit(0);
	*/

	/* feature extration */
	

	// print header
	for(i=0; i<num_intv; i++)
		printf("F%d,", i);
	printf("label\n");

	handle = pcap_open_offline(argv[1], err_string);
	started = 0;
	pcap_loop(handle, -1, main_loop, NULL);
	fprintf(stderr, "bitrate (bps): %lf\n", bitrate*8);
}

