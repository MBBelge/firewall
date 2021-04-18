#include <WinInet.h>
#include <winsock.h>
#include <udpmib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winioctl.h>
#include <WinInet.h>

#define DEST_MAC0	0xE8
#define DEST_MAC1	0x2A
#define DEST_MAC2	0x44
#define DEST_MAC3	0xD1
#define DEST_MAC4	0xE1
#define DEST_MAC5	0xD7

#define ETHER_TYPE	0x0800

#define DEVICE_ONE	"wlp3s0"
#define DEVICE_TWO	"enp2s0f1"

int main()
{
	int saddr_size, data_size, daddr_size, bytes_sent;
	struct sockaddr_ll saddr, daddr;
	unsigned char *buffer = malloc(65535);

	int sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); //For receiving
	int sock = socket(PF_PACKET, SOCK_RAW, IPPROTO_RAW);            //For sending

	memset(&saddr, 0, sizeof(struct sockaddr_ll));
	saddr.sll_family = AF_PACKET;
	saddr.sll_protocol = htons(ETH_P_ALL);
	saddr.sll_ifindex = if_nametoindex("eth0");
	if (bind(sock_raw, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
		perror("bind failed\n");
		close(sock_raw);
	}

	memset(&daddr, 0, sizeof(struct sockaddr_ll));
	daddr.sll_family = AF_PACKET;
	daddr.sll_protocol = htons(ETH_P_ALL);
	daddr.sll_ifindex = if_nametoindex("eth1");
	if (bind(sock, (struct sockaddr*) &daddr, sizeof(daddr)) < 0) {
		perror("bind failed\n");
		close(sock);
	}
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "eth1");
	if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {
		perror("bind to eth1");
	}

	while (1)
	{
		saddr_size = sizeof(struct sockaddr);
		daddr_size = sizeof(struct sockaddr);
		//Receive a packet
		data_size = recvfrom(sock_raw, buffer, 65536, 0, (struct sockaddr *) &saddr, (socklen_t*)&saddr_size);

		if (data_size <0)
		{
			printf("Recvfrom error , failed to get packets\n");
			return 1;
		}
		else {
			printf("Received %d bytes\n", data_size);

			//Huge code to process the packet (optional)

			//Send the same packet out
			bytes_sent = write(sock, buffer, data_size);
			printf("Sent %d bytes\n", bytes_sent);
			if (bytes_sent < 0) {
				perror("sendto");
				exit(1);
			}

		}
	}
	close(sock_raw);
	return 0;
}
