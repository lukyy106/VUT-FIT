/*
Author - Lukas Neupauer
Date - 11.2022
Program - netflow, project to ISA, VUT FIT
*/

#include "netflow.h"



FILE *file; // pcap file pointer
bool file_flag = false; // read from file
char *collector_addr = "127.0.0.1"; // collector address
char *port = "2055"; // collector port
unsigned int active_timer = 60; // max time flow is on program
unsigned int inactive_timer = 10; // time of inactivity after flow is exported
int count_size = 1024; // max flows at once in program
int flow_counter = 0; // curren number of flows in program
char errbuf[PCAP_ERRBUF_SIZE]; // error for handle
u_char protocol; // type of protocol
struct list_s *list; // list of flows
struct pcap_pkthdr header; // packet header
struct ether_header *eth_h; // ethernet header
struct ip *ip_h; // ip header
struct tcphdr *tcp_h; // tcp header
struct udphdr *udp_h; // udp header
struct icmphdr *icmp_h; // icmp header
const u_char *packet; // packet
int sock; // socket for connection with collector
struct sockaddr_in server; // server
struct hostent *servent; // servent


// parse arguments from command line
void parse_arguments(int args, char*argv[]){
  for(int i = 1; i < args; i+=2){
    if(i+1 == args){
      warn("missing argument\n");
      exit(1);
    }

    if(!strcmp(argv[i], "-f")){
      file = fopen(argv[i+1], "r");
      if(file == NULL){
        warn("Can not open %s\n", argv[i+1]);
        exit(1);
      }
      file_flag = true;

    }else if(!strcmp(argv[i], "-c")){
      if(strchr(argv[i+1], ':') != NULL){ // looks for : in string and divide address from port is there is :
        char* input = ":";
        collector_addr = strtok(argv[i+1], input);
        struct in_addr ip_s;
        struct hostent *hp;
        if (!inet_aton(collector_addr, &ip_s)){
          if ((hp = gethostbyname(collector_addr)) == NULL)
              errx(1, "no name associated with %s", collector_addr);
        }
        port = strtok(NULL, ":");
      }else{
        collector_addr = argv[i+1];
      }

    }else if(!strcmp(argv[i], "-a")){
      active_timer = atoi(argv[i+1]);

    }else if(!strcmp(argv[i], "-i")){
      inactive_timer = atoi(argv[i+1]);

    }else if(!strcmp(argv[i], "-m")){
      count_size = atoi(argv[i+1]);
    }
  }
}

// set headers and protocol for every packet
void initialize_values(){
  eth_h = (struct ether_header *) packet;
  ip_h = (struct ip *)(packet + sizeof(struct ether_header));
  protocol = *(packet + ETHERNET_HEADER_LENGH + 9); // protocol is stored in packet as 10th byte
}

// assigne packet to flow of make new flow
void new_packet(time_t time_s, suseconds_t time_us, u_short source_port, u_short dst_port, u_int8_t tos, unsigned long source_addr, unsigned long dst_addr, struct list_s *list_2, unsigned int packet_lengh, u_int8_t flags){
  struct list_s *prev = list_2;
  while(list_2 != NULL){
    if(!list_2->filled){
      break;
    }
    if(protocol == list_2->c_flow.flow.protocol){
      if(source_port == list_2->c_flow.flow.src_port){
        if(dst_port == list_2->c_flow.flow.dest_port){
          if(tos == list_2->c_flow.flow.tos){
            if(source_addr == list_2->c_flow.flow.src_ip){
              if(dst_addr == list_2->c_flow.flow.dest_ip){ // packet part of existing flow
                list_2->c_flow.flow.flow_packets += htonl(1);
                list_2->c_flow.flow.flow_finish = htonl(htonl(list_2->c_flow.flow.flow_start) + (htonl(time_s) - htonl(list_2->c_flow.header.time_sec))*1000 + (htonl(time_us)/1000 - htonl(list_2->c_flow.header.time_nanosec)/1000000));
                list_2->c_flow.header.uptime_ms = htonl(htonl(time_us) / 1000);
                list_2->c_flow.flow.flow_octets += htonl(packet_lengh);
                list_2->last_change = time_s;
                return;
              }
            }
          }
        }
      }
    }
    list_2 = list_2->next;
  }
  // new flow is made
  flow_counter++;

  while(prev->next != NULL){
    prev = prev->next;
  }

  if(list_2 == NULL){
    list_2 = malloc(sizeof(struct list_s));
    list_2->prev = prev;
    list_2->prev->next = list_2;
  }
  list_2->last_change = time_s;
  list_2->c_flow.header.version = htons(5);
  list_2->c_flow.header.count = htons(1);
  list_2->c_flow.header.uptime_ms = htonl(htonl(time_us) / 1000);
  list_2->c_flow.header.time_sec = time_s;
  list_2->c_flow.header.time_nanosec = htonl(htonl(time_us) * 1000);
  list_2->c_flow.header.engine_type = 0;
  list_2->c_flow.header.engine_id = 0;
  list_2->c_flow.header.sampling_interval = 0;
  list_2->c_flow.flow.src_ip = source_addr;
  list_2->c_flow.flow.dest_ip = dst_addr;
  list_2->c_flow.flow.nexthop_ip = 0;
  list_2->c_flow.flow.if_index_in = 0;
  list_2->c_flow.flow.if_index_out = 0;
  list_2->c_flow.flow.flow_packets = htonl(1);
  list_2->c_flow.flow.flow_octets = htonl(packet_lengh);
  list_2->c_flow.flow.flow_start = htonl(htonl(time_us) / 1000);
  list_2->c_flow.flow.flow_finish = htonl(htonl(time_us) / 1000);
  list_2->c_flow.flow.src_port = source_port;
  list_2->c_flow.flow.dest_port = dst_port;
  list_2->c_flow.flow.pad1 = 0;
  list_2->c_flow.flow.tcp_flags = flags;
  list_2->c_flow.flow.protocol = protocol;
  list_2->c_flow.flow.tos = tos;
  list_2->c_flow.flow.src_as = 0;
  list_2->c_flow.flow.dest_as = 0;
  list_2->c_flow.flow.src_mask = 0;
  list_2->c_flow.flow.dst_mask = 0;
  list_2->c_flow.flow.pad2 = 0;
  list_2->filled = 1;
  list_2->next = NULL;
}

// send flow to collector
void export(struct list_s *list_2){
  flow_counter--;
  struct list_s *next = list_2->next;
  struct list_s *prev = list_2->prev;
  int i = send(sock,&list_2->c_flow,sizeof(struct complete_flow),0);     // send data to the server
  if (i == -1)                   // check if data was sent correctly
    warn("send() failed");

  if(next != NULL){
    next->prev = prev;
  }

  if(prev != NULL){
    prev->next = next;
  }

  if(list_2 == list){
    list = list_2->next;
  }

  free(list_2);
}

// check active and inactive timer and send flows is needed
void timer(struct list_s *list_2, time_t time){
  while(list_2 != NULL){
    if(!list_2->filled) // no flow yet
      return;

    if(htonl(time) - htonl(list_2->c_flow.header.time_sec) >= active_timer || time - htonl(list_2->last_change) - htonl(time) >= inactive_timer){ // timer trigger
      export(list_2);
      list_2 = list;
      continue;
    }

    list_2 = list_2->next;
  }
}


int main(int args, char*argv[]){
  parse_arguments(args, argv);
  pcap_t *handle;
  memset(&server,0,sizeof(server));
  server.sin_family = AF_INET;
  if((servent = gethostbyname(collector_addr)) == NULL){
    warn("gethostbyname() failed\n");
    return 1;
  }

  memcpy(&server.sin_addr,servent->h_addr,servent->h_length);
  server.sin_port = htons(atoi(port));
  if((sock = socket(AF_INET , SOCK_DGRAM , 0)) == -1){   //create a client socket
    warn("socket() failed\n");
    return 1;
  }

  if(connect(sock, (struct sockaddr *)&server, sizeof(server))  == -1){
    warn("connect() failed");
    return 1;
  }


  if(file_flag){
    handle = pcap_fopen_offline(file, errbuf);
  }else{
    handle = pcap_open_offline("-", errbuf);
  }

  list = malloc(sizeof(struct list_s));
  list->next = NULL;
  list->prev = NULL;
  list->filled = 0;


  while(1){
    packet = pcap_next(handle, &header);

    if(packet == NULL)
      break;

    timer(list, htonl(header.ts.tv_sec));

    if(list == NULL){
      list = malloc(sizeof(struct list_s));
      list->next = NULL;
      list->prev = NULL;
      list->filled = 0;
    }

    initialize_values();

    if(htons(eth_h->ether_type) != ETHERTYPE_IP) // not en IP packet
      continue;

    if(protocol == IPPROTO_TCP){
      tcp_h = (struct tcphdr*)(packet + ETHERNET_HEADER_LENGH + ip_h->ip_hl * 4);
      new_packet(htonl(header.ts.tv_sec), htonl(header.ts.tv_usec), tcp_h->th_sport, tcp_h->th_dport, htons(ip_h->ip_tos), ip_h->ip_src.s_addr, ip_h->ip_dst.s_addr, list, htons(ip_h->ip_len), tcp_h->th_flags);
    }

    if(protocol == IPPROTO_UDP){
      udp_h = (struct udphdr*)(packet + ETHERNET_HEADER_LENGH + ip_h->ip_hl * 4);
      new_packet(htonl(header.ts.tv_sec), htonl(header.ts.tv_usec), udp_h->uh_sport, udp_h->uh_dport, htons(ip_h->ip_tos), ip_h->ip_src.s_addr, ip_h->ip_dst.s_addr, list, htons(ip_h->ip_len), 0);
    }

    if(protocol == IPPROTO_ICMP){
      icmp_h = (struct icmphdr*)(packet + ETHERNET_HEADER_LENGH + ip_h->ip_hl * 4);
      new_packet(htonl(header.ts.tv_sec), htonl(header.ts.tv_usec), 0, 0, htons(ip_h->ip_tos), ip_h->ip_src.s_addr, ip_h->ip_dst.s_addr, list, htons(ip_h->ip_len), 0);

    }

    if(flow_counter == count_size){
      export(list);
    }
  }



  pcap_close(handle);


  while(list != NULL){
    void *next = list->next;
    int i = send(sock,&list->c_flow,sizeof(struct complete_flow),0);     // send data to the server
    if (i == -1)                   // check if data was sent correctly
      warn("send() failed");

    free(list);
    list = next;
  }

  close(sock);

  return 0;
}
