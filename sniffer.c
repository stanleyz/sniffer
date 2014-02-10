#define APP_NAME "sniffer"
#include "packet.h"
#include<signal.h>

pcap_t *handle;
void print_usage() {
  printf("Usage: %s [ -i interface ] [ expression ]\n", APP_NAME);
  printf("         interface:  network interface name. \n");
  printf("         expression: just like the way you use tcpdump. \n");
  exit(EXIT_SUCCESS);
}

void sig_handler(int sig) {
  if(handle != NULL) {
    pcap_breakloop(handle);
  }
  fflush(stdout);
  fflush(stderr);
  exit(EXIT_FAILURE);
}
int main(int argc, char *argv[]) {
  char errbuf[PCAP_ERRBUF_SIZE];
  char *dev = NULL;
  char filter[2048];
  bpf_u_int32 net; 
  bpf_u_int32 mask;
  struct bpf_program bpf;

  int i = 0;
  memset(filter, 0, sizeof(filter));
  char *tmp = filter;

  signal(SIGTERM, sig_handler);
  signal(SIGINT, sig_handler);
  for(i = 1;i < argc;i++) {
    if(strcasecmp(argv[i], "-h") == 0) {
      print_usage();
    }
    if(strcasecmp(argv[i], "-i") == 0) {
      if(i == 1 || i == argc - 2) {
        dev = argv[++i];
        continue;
      } else {
        printf("%s, Syntax error\n", APP_NAME);
        print_usage();
      }
    }
    tmp = stpcpy(tmp, argv[i]);
    *tmp++ = ' ';
  }

  if(dev == NULL) {
    dev = pcap_lookupdev(errbuf);
    if(dev == NULL) {
      fprintf(stderr, "Couldn't find default device for you: %s\n", errbuf);
      exit(EXIT_FAILURE);
    }
  }

  if( pcap_lookupnet(dev, &net, &mask, errbuf) == -1 ) {
    fprintf(stderr, "Couldn't get network address for %s: %s\n", dev, errbuf);
    exit(EXIT_FAILURE);
  }
  handle = pcap_open_live(dev, 518, 1, 2000, errbuf);
  if( handle == NULL ) {
    fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
    exit(EXIT_FAILURE);
  }

  if( pcap_compile(handle, &bpf, filter, 0, net) == -1 ) {
    fprintf(stderr, "Couldn't parse filter '%s': %s\n", filter, pcap_geterr(handle));
    exit(EXIT_FAILURE);
  }
  if( pcap_setfilter(handle, &bpf) == -1 ) {
    fprintf(stderr, "Couldn't install filter '%s': %s\n", filter, pcap_geterr(handle));
    exit(EXIT_FAILURE);
  }

  pcap_loop(handle, -1, got_packet, NULL);

  pcap_freecode(&bpf);
  pcap_close(handle);

  return(0);
}
