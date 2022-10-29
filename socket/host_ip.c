#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <string.h>

void show_interfaces()
{
	int i = 0;
	int sockfd;
	struct ifconf ifconf;
	struct ifreq *ifreq;
	char buf[1024];
 
	ifconf.ifc_len = 1024;
	ifconf.ifc_buf = buf;
 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
		exit(-1);
	}
 
	// get all interfaces info
	if (ioctl(sockfd, SIOCGIFCONF, &ifconf) == -1) {
        perror("ioctl error");
        exit(-1);
    }
 
	ifreq = (struct ifreq*)buf;
    printf("len:%d, sizeof(struct ifreq)=%d\n", ifconf.ifc_len, sizeof(struct ifreq));
	for (i = (ifconf.ifc_len/sizeof(struct ifreq)); i > 0; i--) {
		printf("[%s] %s, ", ifreq->ifr_name,
            inet_ntoa(((struct sockaddr_in *)&(ifreq->ifr_addr))->sin_addr));
        if (ioctl(sockfd, SIOCGIFFLAGS, ifreq) == -1) {
            perror("ioctl error");
        } else {
            printf("is_up=%d is_lookback=%d\n",
                (ifreq->ifr_flags & IFF_UP) != 0? 1:0, (ifreq->ifr_flags & IFF_LOOPBACK) != 0? 1:0);
        }
		ifreq++;
	}
}

int get_host_ip(char *ret, int len)
{
	int i = 0;
	int sockfd;
	struct ifconf ifconf;
	struct ifreq *ifreq;
	char buf[1024];
 
	ifconf.ifc_len = 1024;
	ifconf.ifc_buf = buf;
 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
	    return -1;
	}
 
	if (ioctl(sockfd, SIOCGIFCONF, &ifconf) == -1) {
        perror("ioctl error");
        return -1;
    }
 
	ifreq = (struct ifreq*)buf;
	for (i = (ifconf.ifc_len/sizeof(struct ifreq)); i > 0; i--) {
        if (ioctl(sockfd, SIOCGIFFLAGS, ifreq) != -1 &&
                (ifreq->ifr_flags & IFF_UP) == 1 &&
                (ifreq->ifr_flags & IFF_LOOPBACK) == 0) {
            memcpy(ret, inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr), len);
            return 0;
        }
        ifreq++;
	}
    return -1;
}

int main(int argc, char **argv)
{
    char host[24] = {'\0'};
    get_host_ip(host, sizeof(host));
    printf("host ip %s\n", host);

    show_interfaces();
    return 0;
}
