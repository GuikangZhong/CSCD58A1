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
```
```console
mininet> client ping -c 3 172.64.3.1
PING 172.64.3.1 (172.64.3.1) 56(84) bytes of data.
64 bytes from 172.64.3.1: icmp_seq=1 ttl=64 time=39.9 ms
64 bytes from 172.64.3.1: icmp_seq=2 ttl=64 time=19.5 ms
64 bytes from 172.64.3.1: icmp_seq=3 ttl=64 time=40.6 ms

--- 172.64.3.1 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2004ms
rtt min/avg/max/mdev = 19.528/33.326/40.574/9.761 ms
```
```console
mininet> client ping -c 3 10.0.1.1
PING 10.0.1.1 (10.0.1.1) 56(84) bytes of data.
64 bytes from 10.0.1.1: icmp_seq=1 ttl=64 time=1.72 ms
64 bytes from 10.0.1.1: icmp_seq=2 ttl=64 time=34.0 ms
64 bytes from 10.0.1.1: icmp_seq=3 ttl=64 time=4.46 ms

--- 10.0.1.1 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2003ms
rtt min/avg/max/mdev = 1.718/13.379/33.965/14.599 ms
```
2. Tracerouting from the client to any of the router's interfaces
```console
mininet> client traceroute -n 192.168.2.1
traceroute to 192.168.2.1 (192.168.2.1), 30 hops max, 60 byte packets
 1  10.0.1.1  11.347 ms  22.432 ms  23.532 ms
```
```console
mininet> client traceroute -n 172.64.3.1
traceroute to 172.64.3.1 (172.64.3.1), 30 hops max, 60 byte packets
 1  10.0.1.1  49.727 ms  64.603 ms  65.291 ms
```
```console
traceroute to 10.0.1.1 (10.0.1.1), 30 hops max, 60 byte packets
 1  10.0.1.1  37.239 ms  49.942 ms  50.877 ms
```
3. Pinging from the client to any of the app servers (192.168.2.2, 172.64.3.10)
```console
mininet> client ping -c 3 server1
PING 192.168.2.2 (192.168.2.2) 56(84) bytes of data.
64 bytes from 192.168.2.2: icmp_seq=1 ttl=63 time=102 ms
64 bytes from 192.168.2.2: icmp_seq=2 ttl=63 time=69.5 ms
64 bytes from 192.168.2.2: icmp_seq=3 ttl=63 time=53.6 ms

--- 192.168.2.2 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2004ms
rtt min/avg/max/mdev = 53.640/75.032/101.915/20.087 ms
```
```console
mininet> client ping -c 3 server2
PING 172.64.3.10 (172.64.3.10) 56(84) bytes of data.
64 bytes from 172.64.3.10: icmp_seq=1 ttl=63 time=102 ms
64 bytes from 172.64.3.10: icmp_seq=2 ttl=63 time=26.1 ms
64 bytes from 172.64.3.10: icmp_seq=3 ttl=63 time=2.72 ms

--- 172.64.3.10 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2003ms
rtt min/avg/max/mdev = 2.724/43.667/102.200/42.472 ms
```
4. Tracerouting from the client to any of the app servers
```console
mininet> client traceroute -n server1
traceroute to 192.168.2.2 (192.168.2.2), 30 hops max, 60 byte packets
 1  10.0.1.1  25.248 ms  33.581 ms  79.141 ms
 2  * * *
 3  * * *
 4  * * *
 5  192.168.2.2  183.733 ms  184.227 ms  184.681 ms
```
```console
mininet> client traceroute -n server2
traceroute to 172.64.3.10 (172.64.3.10), 30 hops max, 60 byte packets
 1  10.0.1.1  35.903 ms  42.843 ms  88.390 ms
 2  * * *
 3  * * *
 4  * * *
 5  172.64.3.10  163.734 ms  164.343 ms  164.921 ms
```
5. Downloading a file using HTTP from one of the app servers
```console
mininet> client wget http://192.168.2.2
--2021-10-28 15:35:37--  http://192.168.2.2/
Connecting to 192.168.2.2:80... connected.
HTTP request sent, awaiting response... 200 OK
Length: 161 [text/html]
Saving to: 'index.html.1'

index.html.1        100%[===================>]     161  --.-KB/s    in 0s

2021-10-28 15:35:37 (75.9 MB/s) - 'index.html.1' saved [161/161]
```
```console
mininet> client wget http://172.64.3.10
--2021-10-28 15:36:09--  http://172.64.3.10/
Connecting to 172.64.3.10:80... connected.
HTTP request sent, awaiting response... 200 OK
Length: 161 [text/html]
Saving to: 'index.html.2'

index.html.2        100%[===================>]     161  --.-KB/s    in 0s

2021-10-28 15:36:09 (74.8 MB/s) - 'index.html.2' saved [161/161]
```

6. Destination net unreachable (type 3, code 0)
```console
mininet> client ping -c 3 12.12.12.12
PING 12.12.12.12 (12.12.12.12) 56(84) bytes of data.
From 10.0.1.1 icmp_seq=1 Destination Net Unreachable
From 10.0.1.1 icmp_seq=2 Destination Net Unreachable
From 10.0.1.1 icmp_seq=3 Destination Net Unreachable

--- 12.12.12.12 ping statistics ---
3 packets transmitted, 0 received, +3 errors, 100% packet loss, time 2004ms
```
![alt text](https://github.com/GuikangZhong/CSCD58A1/blob/main/image/Type3Code0Wireshark.PNG "Wireshark Capture 2")

7. Destination host unreachable (type 3, code 1)
```console
mininet> client ping -c 3 192.168.2.3
PING 192.168.2.3 (192.168.2.3) 56(84) bytes of data.
From 192.168.2.1 icmp_seq=3 Destination Host Unreachable
From 192.168.2.1 icmp_seq=2 Destination Host Unreachable
From 192.168.2.1 icmp_seq=1 Destination Host Unreachable

--- 192.168.2.3 ping statistics ---
3 packets transmitted, 0 received, +3 errors, 100% packet loss, time 2033ms
pipe 3
```
We inserted a routing entry with IP address 192.168.2.3 in the routing table, which does not exist in the network.
![alt text](https://github.com/GuikangZhong/CSCD58A1/blob/main/image/rtable.PNG "Wireshark Capture 3")<br>
After the router sent five ARP requests, the Wireshark captured the Type3 Code1 messages from the router.
![alt text](https://github.com/GuikangZhong/CSCD58A1/blob/main/image/Type3Code1Wireshark.PNG "Wireshark Capture 4")

8. Port unreachable (type 3, code 3)
```console
mininet> client traceroute -n 192.168.2.1
traceroute to 192.168.2.1 (192.168.2.1), 30 hops max, 60 byte packets
 1  10.0.1.1  15.341 ms  29.956 ms  31.024 ms
```
The wireshark captured the (type 3, code 3) messages
![alt text](https://github.com/GuikangZhong/CSCD58A1/blob/main/image/Type3Code3Wireshark.PNG "Wireshark Capture 5")
