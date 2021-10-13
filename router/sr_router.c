/**********************************************************************
 * file:  sr_router.c
 * date:  Mon Feb 18 12:50:42 PST 2002
 * Contact: casado@stanford.edu
 *
 * Description:
 *
 * This file contains all the functions that interact directly
 * with the routing table, as well as the main entry method
 * for routing.
 *
 **********************************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "sr_if.h"
#include "sr_rt.h"
#include "sr_router.h"
#include "sr_protocol.h"
#include "sr_arpcache.h"
#include "sr_utils.h"

struct sr_if* get_interface_by_ip(struct sr_instance* sr, uint32_t tip);
char* get_interface_by_LPM(struct sr_instance* sr, uint32_t ip_dst);
int sanity_check(uint8_t *buf, unsigned int length);
int handle_chksum(sr_ip_hdr_t *ip_hdr);

/*---------------------------------------------------------------------
 * Method: sr_init(void)
 * Scope:  Global
 *
 * Initialize the routing subsystem
 *
 *---------------------------------------------------------------------*/

void sr_init(struct sr_instance* sr)
{
    /* REQUIRES */
    assert(sr);

    /* Initialize cache and cache cleanup thread */
    sr_arpcache_init(&(sr->cache));

    pthread_attr_init(&(sr->attr));
    pthread_attr_setdetachstate(&(sr->attr), PTHREAD_CREATE_JOINABLE);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_t thread;

    pthread_create(&thread, &(sr->attr), sr_arpcache_timeout, sr);
    
    /* Add initialization code here! */

} /* -- sr_init -- */

/*---------------------------------------------------------------------
 * Method: sr_handlepacket(uint8_t* p,char* interface)
 * Scope:  Global
 *
 * This method is called each time the router receives a packet on the
 * interface.  The packet buffer, the packet length and the receiving
 * interface are passed in as parameters. The packet is complete with
 * ethernet headers.
 *
 * Note: Both the packet buffer and the character's memory are handled
 * by sr_vns_comm.c that means do NOT delete either.  Make a copy of the
 * packet instead if you intend to keep it around beyond the scope of
 * the method call.
 *
 *---------------------------------------------------------------------*/

void sr_handlepacket(struct sr_instance* sr,
        uint8_t * packet/* lent */,
        unsigned int len,
        char* interface/* lent */)
{
  /* REQUIRES */
  assert(sr);
  assert(packet);
  assert(interface);

  printf("*** -> Received packet of length %d \n",len);

  /* fill in code here */

  /* sanity check */
  int success = sanity_check(packet, len);
  if (success != 0) {
    fprintf(stderr, "Failed to handle packet\n");
    return;
  }

  /* get the ethernet header */
  sr_ethernet_hdr_t *ehdr = (sr_ethernet_hdr_t *)packet;
  uint16_t ethtype = ethertype(packet);
  struct sr_if *source_if = sr_get_interface(sr, interface);
  
  /* case1: is an arp request */
  if (ethtype == ethertype_arp) {
    sr_arp_hdr_t *arp_hdr = (sr_arp_hdr_t *)(packet+sizeof(sr_ethernet_hdr_t));
    print_hdr_arp(packet+sizeof(sr_ethernet_hdr_t));
    /* In the case of an ARP request, you should only send an ARP reply if the target IP address is one of
     * your router's IP addresses */
    struct sr_if *target_if = get_interface_by_ip(sr, arp_hdr->ar_tip);

    /* case1.1: the request destinates to an router interface */
    if (ntohs(arp_hdr->ar_op) == arp_op_request && target_if) {
      fprintf(stdout, "---------case1.1----------\n");
      /* construct ARP reply */
      unsigned long reply_len = sizeof(sr_ethernet_hdr_t) + sizeof(sr_arp_hdr_t);
      uint8_t *reply_packet = (uint8_t *)malloc(reply_len);
      
      /* construct ethernet header */
      sr_ethernet_hdr_t *reply_ehdr = (sr_ethernet_hdr_t *)reply_packet;
      memcpy(reply_ehdr->ether_dhost, ehdr->ether_shost, ETHER_ADDR_LEN);
      memcpy(reply_ehdr->ether_shost, source_if->addr, ETHER_ADDR_LEN);
      reply_ehdr->ether_type = htons(ethertype_arp);

      /* construct arp header */
      sr_arp_hdr_t *reply_arp_hdr = (sr_arp_hdr_t *)(reply_packet + sizeof(sr_ethernet_hdr_t));
      memcpy(reply_arp_hdr, arp_hdr, sizeof(sr_arp_hdr_t));
      reply_arp_hdr->ar_op = htons(arp_op_reply);
      memcpy(reply_arp_hdr->ar_sha, source_if->addr, ETHER_ADDR_LEN);
      reply_arp_hdr->ar_sip = source_if->ip;
      memcpy(reply_arp_hdr->ar_tha, arp_hdr->ar_sha, ETHER_ADDR_LEN);
      reply_arp_hdr->ar_tip = arp_hdr->ar_sip;

      fprintf(stdout, "sending ARP reply packet\n");
      sr_send_packet(sr, reply_packet, reply_len, source_if->name);
      free(reply_packet);
    }

    /* In the case of an ARP reply, you should only cache the entry if the target IP
       address is one of your router's IP addresses. */
    
    /* case1.2: the request does not destinate to an router interface */
    else if (ntohs(arp_hdr->ar_op) == arp_op_reply && target_if) {
      fprintf(stdout, "---------case1.2----------\n");
      fprintf(stdout, "arpcache--before:\n");
      sr_arpcache_dump(&(sr->cache));
      struct sr_arpreq *arpreq = sr_arpcache_insert(&(sr->cache), arp_hdr->ar_sha, ntohl(arp_hdr->ar_sip));
      if (arpreq) {
        struct sr_packet *packet;
        for (packet=arpreq->packets; packet != NULL; packet=packet->next) {
          sr_ethernet_hdr_t *ehdr = (sr_ethernet_hdr_t *)(packet->buf);
          memcpy(ehdr->ether_dhost, arp_hdr->ar_sha, ETHER_ADDR_LEN);
          memcpy(ehdr->ether_shost, source_if->addr, ETHER_ADDR_LEN);
          sr_send_packet(sr, packet->buf, packet->len, packet->iface);
        }
        sr_arpreq_destroy(&(sr->cache), arpreq);
      }
      fprintf(stdout, "arpcache--after:\n");
      sr_arpcache_dump(&(sr->cache));
    }

    else {
      fprintf(stdout, "---------case1.3----------\n");
    }
  }

  /* case2: is an ip request */
  else if (ethtype == ethertype_ip) {

    sr_ip_hdr_t *ip_hdr = (sr_ip_hdr_t *)(packet+sizeof(sr_ethernet_hdr_t));
    struct sr_if *if_walker = get_interface_by_ip(sr, ip_hdr->ip_dst);
    print_hdr_ip(packet+sizeof(sr_ethernet_hdr_t));
    /* sr_arpcache_dump(&(sr->cache)); */

    /* If it is sent to one of your router's IP addresses, */
    /* case2.1: the request destinates to an router interface */
    if (if_walker) {
      fprintf(stderr, "---------case2.1----------\n");
    }
    /* case2.2: the request does not destinate to an router interface */
    else {
      fprintf(stderr, "---------case2.2----------\n");
      int success = handle_chksum(ip_hdr);
      if (success == -1) return;

      /* Find out which entry in the routing table has the longest prefix match 
         with the destination IP address. */
      char *oif_name = get_interface_by_LPM(sr, ip_hdr->ip_dst);
      struct sr_if *oif = sr_get_interface(sr, oif_name);

      /* send packet to next_hop_ip */
      struct sr_arpentry *entry = sr_arpcache_lookup(&(sr->cache), ip_hdr->ip_dst);
      if (entry) {
        /* use next_hop_ip->mac mapping in entry to send the packet
          free entry */
        memcpy(ehdr->ether_dhost, entry->mac, ETHER_ADDR_LEN);
        memcpy(ehdr->ether_shost, oif->addr, ETHER_ADDR_LEN);
        sr_send_packet(sr, packet, len, oif_name);
        free(entry);
      }
      else {
        struct sr_arpreq *req = sr_arpcache_queuereq(&(sr->cache), ntohl(ip_hdr->ip_dst), packet, len, oif_name);
        handle_arpreq(sr, req);
      }
    }
  }

  /* case3: forwading */
  else {
     fprintf(stdout, "---------case3----------\n");
  }

}/* end sr_ForwardPacket */

/* Get interface name by longest prefix match */
char* get_interface_by_LPM(struct sr_instance* sr, uint32_t ip_dst) {
  struct sr_rt *entry = sr->routing_table;
  struct sr_rt *match;
  uint32_t diff = 0xFFFFFFFF;
  while (entry) {
    uint32_t netid = ntohl(entry->dest.s_addr) & ntohl(entry->mask.s_addr);
    if (diff > netid - ntohl(ip_dst)) {
      diff = netid - ntohl(ip_dst);
      match = entry;
    }
    entry = entry->next;
  }
  return match->interface;
}

/* Get interface object by longest prefix match */
struct sr_if* get_interface_by_ip(struct sr_instance* sr, uint32_t tip) {
  struct sr_if *if_walker = sr->if_list;
  while (if_walker) {
    if (if_walker->ip == tip) {
      return if_walker;
    }
    if_walker = if_walker->next;
  }
  return 0;
}

int handle_chksum(sr_ip_hdr_t *ip_hdr) {
    /* inspect checksum */
    uint16_t sum = ip_hdr->ip_sum;
    ip_hdr->ip_sum = 0;
    ip_hdr->ip_sum = cksum(ip_hdr, sizeof(sr_ip_hdr_t));
    if (sum != ip_hdr->ip_sum) {
      fprintf(stderr, "Incorrect checksum\n");
      return -1;
    }

    /* decrement TTL by 1 */
    ip_hdr->ip_ttl--;
    ip_hdr->ip_sum = 0;
    ip_hdr->ip_sum = cksum(ip_hdr, sizeof(sr_ip_hdr_t));
    
    return 0;
}

int sanity_check(uint8_t *buf, unsigned int length) {
  int minlength = sizeof(sr_ethernet_hdr_t);
  if (length < minlength) {
    return -1;
  }
  uint16_t ethtype = ethertype(buf);
  if (ethtype == ethertype_ip) { /* IP */
    minlength += sizeof(sr_ip_hdr_t);
    if (length < minlength) {
      return -1;
    }

    uint8_t ip_proto = ip_protocol(buf + sizeof(sr_ethernet_hdr_t));
    if (ip_proto == ip_protocol_icmp) { /* ICMP */
      minlength += sizeof(sr_icmp_hdr_t);
      if (length < minlength)
        return -1;
    }
  }
  else if (ethtype == ethertype_arp) { /* ARP */
    minlength += sizeof(sr_arp_hdr_t);
    if (length < minlength)
      return -1;
  }
  else {
    return -1;
  }
  return 0;
}