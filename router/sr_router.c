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

struct sr_if* find_interface(struct sr_instance* sr, uint32_t tip);

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
  /* get the ethernet header */
  sr_ethernet_hdr_t *ehdr = (sr_ethernet_hdr_t *)packet;

  uint16_t ethtype = ethertype(packet);

  if (ethtype == ethertype_arp) {
    sr_arp_hdr_t *arp_hdr = (sr_arp_hdr_t *)(packet+sizeof(sr_ethernet_hdr_t));
    /* In the case of an ARP request, you should only send an ARP reply if the target IP address is one of
     * your router's IP addresses */
    struct sr_if *if_walker = find_interface(sr, arp_hdr->ar_tip);
    if (ntohs(arp_hdr->ar_op) == arp_op_request && if_walker) {
      /* construct ARP reply */
      uint8_t *reply_packet = (uint8_t *)malloc(sizeof(sr_ethernet_hdr_t) + sizeof(sr_arp_hdr_t));
      struct sr_if* source_if = sr_get_interface(sr, interface);
      
      /* construct ethernet header */
      sr_ethernet_hdr_t *reply_ehdr = (sr_ethernet_hdr_t *)reply_packet;
      memcpy(reply_ehdr->ether_dhost, arp_hdr->ar_sip, sizeof(uint32_t));
      memcpy(reply_ehdr->ether_shost, source_if->ip, sizeof(uint32_t));
      reply_ehdr->ether_type = ethertype_arp;

      /* construct arp header */
    }
  }

}/* end sr_ForwardPacket */

struct sr_if* find_interface(struct sr_instance* sr, uint32_t tip) {
  struct sr_if *if_walker = sr->if_list;
  while (if_walker) {
    if (if_walker->ip == tip) {
      return if_walker;
    }
    if_walker = if_walker->next;
  }
  return 0;
}