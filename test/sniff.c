#include<pcap/pcap.h>
#include<stdlib.h>
#include<net/ethernet.h>
#include<linux/ip.h>
#include<linux/tcp.h>

void got_packet(u_char *user, const struct pcap_pkthdr *h, const u_char *packet);

void got_packet(u_char *user, const struct pcap_pkthdr *h, const u_char *packet) {
  const struct ether_header *ethernet;
  const struct iphdr *ip;
  const struct tcphdr *tcp;

  ethernet = (struct ether_header*)(packet);
  ip = (struct iphdr*)(packet + sizeof(struct ether_header));

  return;
}

int main(int argc, char *argv[]) {
  char errbuf[PCAP_ERRBUF_SIZE];
  char *dev = argv[1];
  pcap_t *handle;
  bpf_u_int32 net;
  bpf_u_int32 mask;
  struct bpf_program bpf;
  char *filter = "ip";

  printf("Device you entered: %s\n", dev);
  if( pcap_lookupnet(dev, &net, &mask, errbuf) == -1 ) {
    printf("Couldn't get network address for %s: %s\n", dev, errbuf);
    exit(EXIT_FAILURE);
  }
  handle = pcap_open_live(dev, 64, 1, 2000, errbuf);
  if( handle == NULL ) {
    printf("Couldn't open device %s: %s\n", dev, errbuf);
    exit(EXIT_FAILURE);
  }

  if( pcap_compile(handle, &bpf, filter, 0, net) == -1 ) {
    printf("Couldn't parse filter %s: %s\n", "", pcap_geterr(handle));
    exit(EXIT_FAILURE);
  }
  if( pcap_setfilter(handle, &bpf) == -1 ) {
    printf("Couldn't install filter %s: %s\n", "", pcap_geterr(handle));
    exit(EXIT_FAILURE);
  }

  pcap_loop(handle, -1, got_packet, NULL);

  pcap_freecode(&bpf);
  pcap_close(handle);

  return(0);
}
