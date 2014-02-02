#include "packet.h"
#include<time.h>

char *get_tcp_info(const u_char *packet, char *buf);

const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
const struct sniff_ip *ip;              /* The IP header */
const struct sniff_tcp *tcp;
const char *payload;

int size_ip;
int size_tcp;
int size_payload;
char tmp[15];

void got_packet(u_char *user, const struct pcap_pkthdr *h, const u_char *packet) {
  char data_buf[2048] = "";

  ethernet = (struct sniff_ethernet*)(packet);
  ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
  size_ip = IP_HL(ip)*4;
  if(size_ip < 20) {
    fprintf(stderr, "Invalid IP header length: %u bytes\n", size_ip);
    return;
  }

  /*
  const struct tm *tm = localtime(&(h->ts.tv_sec));
  if(tm == NULL) {
    fprintf(stderr, "ERROR");
    return;
  }

  if(strftime(tmp, sizeof(tmp), "%Y%m%d-%H:%M:%S", tm) == 0) {
    fprintf(stderr, "ERROR");
    return;
  }
  */
  sprintf(data_buf, "%d", h->ts.tv_sec);
  strcat(data_buf, "-");
  sprintf(tmp, "%d", h->ts.tv_usec);
  strsccat(data_buf,tmp);
  sprintf(tmp, "%d", h->caplen); 
  strsccat(data_buf, tmp);
  strsccat(data_buf, inet_ntoa(ip->ip_src));
  strsccat(data_buf, inet_ntoa(ip->ip_dst));

  /* determine protocol */  
  switch(ip->ip_p) {
    case IPPROTO_TCP:
      strsccat(data_buf, "TCP");
      get_tcp_info(packet, data_buf);
      break;
    case IPPROTO_UDP:
      strsccat(data_buf, "UDP");
      break;
    case IPPROTO_ICMP:
      strsccat(data_buf, "ICMP");
      break;
    case IPPROTO_IP:
      strsccat(data_buf, "IP");
      break;
    default:
      strsccat(data_buf, "Others");
      break;
  }

  printf("%s\n", data_buf);
  return;
}

char *get_tcp_info(const u_char *packet, char *buf) {
  tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);

  size_tcp = TH_OFF(tcp)*4;
  if(size_tcp < 20) {
    fprintf(stderr, "Invalid TCP hader length: %u bytes\n", size_tcp);
    return;
  }

  sprintf(tmp, "%d", ntohs(tcp->th_sport));
  strsccat(buf, tmp);
  sprintf(tmp, "%d", ntohs(tcp->th_dport));
  strsccat(buf, tmp);
  if((tcp ->th_flags & TH_FIN) && (tcp->th_flags & TH_ACK)) {
    strsccat(buf, "FA");
  } else {
    strsccat(buf, " ");
  }

  payload = (packet + SIZE_ETHERNET + size_ip + size_tcp);
  size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

  if(size_payload > 0 ) {
    print_http(payload, buf);
  }

  return buf;
}
