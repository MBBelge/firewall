#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

#define DEST_MAC0	0x00
#define DEST_MAC1	0x00
#define DEST_MAC2	0x00
#define DEST_MAC3	0x00
#define DEST_MAC4	0x00
#define DEST_MAC5	0x00

#define ETHER_TYPE	0x0800

#define DEVICE_ONE	"enp0s3"
#define DEVICE_TWO	"enx000c0d163629"
#define BUF_SIZ		1024

int main()
{
    int saddr_size ,saddr_size2, data_size,data_size2, daddr_size,daddr_size2, bytes_sent, bytes_sent2;
    struct sockaddr_ll saddr, daddr,saddr2,daddr2;
    unsigned char *buffer=malloc(65535);
    unsigned char *buffer2=malloc(65535);

    int sock_fromethernet = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;
    int sock_toethernet = socket( PF_PACKET , SOCK_RAW , IPPROTO_RAW) ;
    int sock_frominternet = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;
    int sock_tointernet = socket( PF_PACKET , SOCK_RAW , IPPROTO_RAW) ;
    
    memset(&saddr2, 0, sizeof(struct sockaddr_ll));
    saddr2.sll_family = AF_PACKET;
    saddr2.sll_protocol = htons(ETH_P_ALL);
    saddr2.sll_ifindex = if_nametoindex("enp0s3");
    if (bind(sock_toethernet, (struct sockaddr*) &saddr2, sizeof(saddr2)) < 0) {
        perror("bind failed\n");
        close(sock_toethernet);
    }
    
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "enp0s3");
    if (setsockopt(sock_toethernet, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {
        perror("bind to eth1");
        }

    memset(&daddr2, 0, sizeof(struct sockaddr_ll));
    daddr2.sll_family = AF_PACKET;
    daddr2.sll_protocol = htons(ETH_P_ALL);
    daddr2.sll_ifindex = if_nametoindex("enp0s8");
    if (bind(sock_frominternet, (struct sockaddr*) &daddr2, sizeof(daddr2)) < 0) {
      perror("bind failed\n");
      close(sock_frominternet);
    }
    struct ifreq ifr2;
    memset(&ifr2, 0, sizeof(ifr2));
    snprintf(ifr2.ifr_name, sizeof(ifr2.ifr_name), "enp0s8");
    if (setsockopt(sock_frominternet, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr2, sizeof(ifr2)) < 0) {
        perror("bind to eth1");
        }

    memset(&saddr, 0, sizeof(struct sockaddr_ll));
    saddr.sll_family = AF_PACKET;
    saddr.sll_protocol = htons(ETH_P_ALL);
    saddr.sll_ifindex = if_nametoindex("enp0s3");
    if (bind(sock_fromethernet, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
        perror("bind failed\n");
        close(sock_fromethernet);
    }
    
    struct ifreq ifr3;
    memset(&ifr3, 0, sizeof(ifr3));
    snprintf(ifr3.ifr_name, sizeof(ifr3.ifr_name), "enp0s3");
    if (setsockopt(sock_fromethernet, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr3, sizeof(ifr3)) < 0) {
        perror("bind to eth1");
        }

    memset(&daddr, 0, sizeof(struct sockaddr_ll));
    daddr.sll_family = AF_PACKET;
    daddr.sll_protocol = htons(ETH_P_ALL);
    daddr.sll_ifindex = if_nametoindex("enp0s8");
    if (bind(sock_tointernet, (struct sockaddr*) &daddr, sizeof(daddr)) < 0) {
      perror("bind failed\n");
      close(sock_tointernet);
    }
    struct ifreq ifr4;
    memset(&ifr4, 0, sizeof(ifr4));
    snprintf(ifr4.ifr_name, sizeof(ifr4.ifr_name), "enp0s8");
    if (setsockopt(sock_tointernet, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr4, sizeof(ifr4)) < 0) {
        perror("bind to eth1");
        }

    while(1)
    {
        saddr_size = sizeof (struct sockaddr);
        daddr_size = sizeof (struct sockaddr);
        saddr_size2 = sizeof (struct sockaddr);
        daddr_size2 = sizeof (struct sockaddr);
        //Receive a packet
        data_size = recvfrom(sock_fromethernet , buffer , 65536 , 0 ,(struct sockaddr *) &saddr , (socklen_t*)&saddr_size);
        data_size2 = recvfrom(sock_fromethernet , buffer2 , 65536 , 0 ,(struct sockaddr *) &saddr2 , (socklen_t*)&saddr_size);
        if(data_size <0 || data_size2 <0)
        {
            printf("Recvfrom error , failed to get packets\n");
            return 1;
        }
        else{
        printf("Received %d bytes from ethernet\n",data_size);
        printf("Received %d bytes from internet\n",data_size2);

        //Send the same packet out
        bytes_sent=send(sock_tointernet,buffer,data_size,0);
        bytes_sent2=send(sock_toethernet,buffer2,data_size2,0);
        printf("Sent %d bytes to ethernet\n",bytes_sent);
        printf("Sent %d bytes to internet\n",bytes_sent2);
         if (bytes_sent < 0 || bytes_sent2 < 0) {
            perror("sendto");
            exit(1);
         }

        }
    }
    close(sock_fromethernet);
    close(sock_frominternet);
    close(sock_toethernet);
    close(sock_tointernet);
    return 0;
}
