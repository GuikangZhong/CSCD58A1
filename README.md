# CSCD58A1

## Team member
Guikang Zhong 1004304807 zhonggui<br>
Jingwei Wang 1003777754 wangj589<br>

## Contributions from each member of the team
Guikang Zhong:<br>
1.Built the ARP cache.<br>
2.Built Ethernet, ARP, IP, ICMP headers.<br>

Jingwei Wang:<br>
1.Hanle ICMP replies<br>
2.Forward TCP/UDP packets<br>

## Documentation for function implementing the required and missed functionalities in the starter code
We finished all the required functionality.<br>
1.The router can route packets between the Internet and the application servers.<br>
2.The router correctly handles ARP requests and replies.<br>
3.The router correctly handles traceroutes through it and to it.<br>
4.The router responds correctly to ICMP echo requests.<br>
5.The router handles TCP/UDP packets sent to one of its interfaces by responding an ICMP port unreachable.<br>
6.The router maintains an ARP cache whose entries are invalidated after a timeout period.<br>
7.The router queues all packets waiting for outstanding ARP replies. If a host does not respond to 5 ARP requests, the queued packet is dropped and an ICMP host unreachable message is sent back to the source of the queued packet.<br>
8.The router drop packets when the packet is smaller than the minimum length requirements, or the checksum is invalid, or when the router needs to send ICMP Type 3 or Type 11 messages back to sending hosts.<br>
9.The router guarantees on timeouts. If an ARP request is not responded to within a 15 seconds, the ICMP host unreachable message is generated and sent out.<br>
## List of tests cases run and results
1. Pinging from the client to any of the router's interfaces (192.168.2.1, 172.64.3.1, 10.0.1.1).
```console
mininet> client ping -c 3 192.168.2.1 
PING 192.168.2.1 (192.168.2.1) 56(84) bytes of data.
64 bytes from 192.168.2.1: icmp_seq=1 ttl=64 time=29.6 ms
64 bytes from 192.168.2.1: icmp_seq=2 ttl=64 time=9.45 ms
64 bytes from 192.168.2.1: icmp_seq=3 ttl=64 time=35.2 ms

--- 192.168.2.1 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2003ms
rtt min/avg/max/mdev = 9.445/24.732/35.198/11.052 ms

mininet> client ping -c 3 172.64.3.1
PING 172.64.3.1 (172.64.3.1) 56(84) bytes of data.
64 bytes from 172.64.3.1: icmp_seq=1 ttl=64 time=39.9 ms
64 bytes from 172.64.3.1: icmp_seq=2 ttl=64 time=19.5 ms
64 bytes from 172.64.3.1: icmp_seq=3 ttl=64 time=40.6 ms

--- 172.64.3.1 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2004ms
rtt min/avg/max/mdev = 19.528/33.326/40.574/9.761 ms

mininet> client ping -c 3 10.0.1.1
PING 10.0.1.1 (10.0.1.1) 56(84) bytes of data.
64 bytes from 10.0.1.1: icmp_seq=1 ttl=64 time=1.72 ms
64 bytes from 10.0.1.1: icmp_seq=2 ttl=64 time=34.0 ms
64 bytes from 10.0.1.1: icmp_seq=3 ttl=64 time=4.46 ms

--- 10.0.1.1 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2003ms
rtt min/avg/max/mdev = 1.718/13.379/33.965/14.599 ms
```
