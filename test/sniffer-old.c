#include<stdio.h>
#include<stdlib.h>
#include<pcap/pcap.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#define _GNU_SOURCE
#include<string.h>

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN  6
#define SIZE_ETHERNET 14
const char *http_method = "GET;POST;PUT;OPTIONS;HEAD;DELETE;TRACE;CONNECT;";

void got_packet(u_char *user, const struct pcap_pkthdr *h, const u_char *packet);

/* Ethernet header */ 
struct sniff_ethernet { 
  u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */ 
  u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */ 
  u_short ether_type;                     /* IP? ARP? RARP? etc */ 
}; 

/* IP header */ 
struct sniff_ip { 
  u_char  ip_vhl;                 /* version << 4 | header length >> 2 */ 
  u_char  ip_tos;                 /* type of service */ 
  u_short ip_len;                 /* total length */ 
  u_short ip_id;                  /* identification */ 
  u_short ip_off;                 /* fragment offset field */ 
  #define IP_RF 0x8000            /* reserved fragment flag */ 
  #define IP_DF 0x4000            /* dont fragment flag */ 
  #define IP_MF 0x2000            /* more fragments flag */ 
  #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */ 
  u_char  ip_ttl;                 /* time to live */ 
  u_char  ip_p;                   /* protocol */ 
  u_short ip_sum;                 /* checksum */ 
  struct  in_addr ip_src,ip_dst;  /* source and dest address */ 
}; 
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f) 
#define IP_V(ip)                (((ip)->ip_vhl) >> 4) 

/* TCP header */ 
typedef u_int tcp_seq; 

struct sniff_tcp { 
  u_short th_sport;               /* source port */ 
  u_short th_dport;               /* destination port */ 
  tcp_seq th_seq;                 /* sequence number */ 
  tcp_seq th_ack;                 /* acknowledgement number */ 
  u_char  th_offx2;               /* data offset, rsvd */ 
  #define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4) 
  u_char  th_flags; 
  #define TH_FIN  0x01 
  #define TH_SYN  0x02 
  #define TH_RST  0x04 
  #define TH_PUSH 0x08 
  #define TH_ACK  0x10 
  #define TH_URG  0x20 
  #define TH_ECE  0x40 
  #define TH_CWR  0x80 
  #define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR) 
  u_short th_win;                 /* window */ 
  u_short th_sum;                 /* checksum */ 
  u_short th_urp;                 /* urgent pointer */ 
};

void print_http(const char *data, int size) {
  char url_buf[256];
  char host_buf[96];
  char method_buf[9];
  char http_data[15180];
  int i = 0;
  
  memcpy((void *)http_data, (void *)data, (size_t)15180);
  printf("%s\n", http_data);

  while(*data != ' ' && i < 7) {
    method_buf[i++] = toupper(*data++);
  }
  method_buf[i++] = ';';
  method_buf[i] = '\0';
  printf("HTTP method is %s\n", method_buf);

  if(strstr(http_method, method_buf) != NULL) {
    data += 1;
    i = 0;
    while(*data != '\r') {
      if(*data != ' ' && i < 255) {
        url_buf[i++] = *data++;
      } else {
        if(i < 256) {
          url_buf[i++] = '\0';
        }
        *data++;
      }
    }

    printf("GET url is: %s\n", url_buf);

    i = 0;
    data += 6;
    while(*data != '\r' && i < 94) {
      host_buf[i++] = *data++;
    }
    host_buf[i] = '\0';

    printf("Host is %s\n", host_buf);
  }
  return;
}
void got_packet(u_char *user, const struct pcap_pkthdr *h, const u_char *packet) {
  /* declare pointers to packet headers */
  const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
  const struct sniff_ip *ip;              /* The IP header */
  const struct sniff_tcp *tcp;            /* The TCP header */
  const char *payload;                    /* Packet payload */

  int size_ip;
  int size_tcp;
  int size_payload;

  ethernet = (struct sniff_ethernet*)(packet);
  ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
  size_ip = IP_HL(ip)*4;
  if(size_ip < 20) {
    printf("      Invalid IP header length: %u bytes\n", size_ip);
    return;
  }

  printf("\n      From: %s\n", inet_ntoa(ip->ip_src));
  printf("        To: %s\n", inet_ntoa(ip->ip_dst));

  /* determine protocol */  
  switch(ip->ip_p) {
    case IPPROTO_TCP:
      printf("  Protocol: TCP\n");
      break;
    case IPPROTO_UDP:
      printf("  Protocol: UDP\n");
      return;
    case IPPROTO_ICMP:
      printf("  Protocol: ICMP\n");
      return;
    case IPPROTO_IP:
      printf("  Protocol: IP\n");
      return;
    default:
      printf("  Protocol: unknown\n");
      return;
  }

  tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
  size_tcp = TH_OFF(tcp)*4;
  if(size_tcp < 20) {
    printf("      Invalid TCP hader length: %u bytes\n", size_tcp);
    return;
  }
  
  printf("    S-Port: %d\n", ntohs(tcp->th_sport));
  printf("    D-Port: %d\n", ntohs(tcp->th_dport));
  printf("       SYN: %d\n", ntohs(tcp->th_seq));
  printf("       ACK: %d\n", ntohs(tcp->th_ack));
  printf("     Flags: %x\n", tcp->th_flags);
  if(tcp ->th_flags & TH_FIN) {
    printf("            :FIN");
  }
  if(tcp ->th_flags & TH_SYN) {
    printf("            SYN");
  }
  if(tcp ->th_flags & TH_ACK) {
    printf("            ACK");
  }
  if(tcp ->th_flags & TH_RST) {
    printf("            RST");
  }
  if(tcp ->th_flags & TH_PUSH) {
    printf("            PUSH");
  }
  printf("\n");

  payload = (packet + SIZE_ETHERNET + size_ip + size_tcp);
  size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

  if(size_payload > 0 && (ntohs(tcp->th_dport) == 80 || ntohs(tcp->th_dport) == 443) ) {
    print_http(payload, size_payload);
  }

  return;
}

int main(int argc, char *argv[]) {
  pcap_t *handle;
  char errbuf[PCAP_ERRBUF_SIZE];
  char *dev = argv[1];
  char *filter = "port 80 or port 443";
  bpf_u_int32 net; 
  bpf_u_int32 mask;
  struct bpf_program bpf;

  printf("Device you entered: %s\n", dev);
  if( pcap_lookupnet(dev, &net, &mask, errbuf) == -1 ) {
    printf("Couldn't get network address for %s: %s\n", dev, errbuf);
    exit(EXIT_FAILURE);
  }
  handle = pcap_open_live(dev, 15180, 1, 2000, errbuf);
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
