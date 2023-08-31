#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#define TARGET_IP "192.168.1.1"
#define SOURCE_IP "192.168.1.100"
#define SOURCE_MAC "\x00\x11\x22\x33\x44\x55"

void send_arp_request(const char *interface) {
    int sockfd;
    struct sockaddr_ll sockaddr;
    unsigned char buffer[ETH_FRAME_LEN];

    // Create a raw socket
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Fill sockaddr structure
    memset(&sockaddr, 0, sizeof(struct sockaddr_ll));
    sockaddr.sll_family = AF_PACKET;
    sockaddr.sll_protocol = htons(ETH_P_ALL);
    sockaddr.sll_ifindex = if_nametoindex(interface);

    // Construct ARP request
    struct ether_header *eth_header = (struct ether_header *)buffer;
    struct ether_arp *arp_header = (struct ether_arp *)(buffer + ETH_HLEN);

    // Ethernet header
    memcpy(eth_header->ether_shost, SOURCE_MAC, ETH_ALEN);
    memset(eth_header->ether_dhost, 0xff, ETH_ALEN);
    eth_header->ether_type = htons(ETHERTYPE_ARP);

    // ARP header
    arp_header->arp_hrd = htons(ARPHRD_ETHER);
    arp_header->arp_pro = htons(ETHERTYPE_IP);
    arp_header->arp_hln = ETH_ALEN;
    arp_header->arp_pln = 4;
    arp_header->arp_op = htons(ARPOP_REQUEST);

    memcpy(arp_header->arp_sha, SOURCE_MAC, ETH_ALEN);
    inet_pton(AF_INET, SOURCE_IP, arp_header->arp_spa);
    memset(arp_header->arp_tha, 0, ETH_ALEN);
    inet_pton(AF_INET, TARGET_IP, arp_header->arp_tpa);

    // Send ARP request
    ssize_t send_len = sendto(sockfd, buffer, ETH_HLEN + sizeof(struct ether_arp), 0,
                              (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_ll));
    if (send_len == -1) {
        perror("sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    close(sockfd);
}

int main() {
    const char *interface = "eth0"; // Replace with your network interface
    send_arp_request(interface);
    return 0;
}

