/******************************************************Copyright (c)**************************************************
 **                                             Huawei Technology CO., LTD.
 **
 **                                             E-Mail: ChinaFengliang@163.com
 **
 **---------File Information-------------------------------------------------------------------------------------------
 ** File name:            main.c
 ** Last version:         V1.00
 ** Descriptions:         wake up personal computer via net
 ** Hardware platform:    
 ** SoftWare platform:    Linux
 **
 **--------------------------------------------------------------------------------------------------------------------
 ** Created by:           Feng Liang
 ** Created date:         2016/02/15  15:15:8 
 ** Version:              V1.00
 ** Descriptions:         The original version
 **
 **--------------------------------------------------------------------------------------------------------------------
 ** Modified by: 
 ** Modified date:
 ** Version:
 ** Descriptions:
 **
 *********************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/ether.h>

#define BUFMAX 1024
#define MACLEN 6

static const char help[] =
"wake your PC up via net\n" \
"\n" \
"Usage: \n" \
"  wakepc\n" \
"  wakepc <alias name>\n" \
"  wakepc <MAC address>\n" \
"\n" \
"Options:\n" \
"  --help, -h\n" \
"  Print usage" \
"\n" \
"  --init, -i\n" \
"  Create a new \"~/.pclist\" file, which is a list" \
"  of personal computers\n" \
"\n" \
"  --version, -v\n" \
"  Print version information" \
"\n" \
"Examples:\n" \
"  · wake up PC which is define by ~/.pclist\n" \
" 		$ wakepc\n" \
"\n" \
"  · wake up PC which is define by alias name\n" \
" 		$ wakepc workstation\n" \
"\n" \
"  · wake up PC which is define by Mac address\n" \
" 		& wakepc 44:39:c4:8f:4f:39\n";

static char pclist_head[] =
"# list of personal computers\n" \
"#\n" \
"# Syntax:\n" \
"# 	white-space separated list of entries;\n" \
"# 	each entry has the fields documented below.\n" \
"#\n" \
"# 	Lines starting with '#' are comments.\n" \
"# 	Blank lines are ignored.\n" \
"#\n" \
"# Fields:\n" \
"# <mac address> <alias name>\n" \
"\n" \
"26:a2:e1:26:c3:0c        workmint\n";

static char version[] = "wakepc version 1.0.0\n";

static char* skip_whitespace(char *s)
{
	while (*s == ' ' || *s == '\t')
		++s;
	return s;
}

int main(int argc, char *argv[])
{
	int status = 0;
	FILE *list;
	int sockfd;
	char magic[BUFMAX];
	struct sockaddr_in sin;
	struct ether_addr* mac;
	char defpath[128];
	char line[128], *entry;
	int i;

	if (argc == 1) {
		sprintf(defpath, "%s/%s", getenv("HOME"), ".pclist");
		list = fopen(defpath, "r");
		if (NULL == list) {
			fprintf(stderr, "failed to read ~/.pclist, error(%d)\n", errno);
			return -1;
		}

		while (NULL != fgets(line, sizeof(line), list)) {
			/* skip over leading whitespace */
			entry = skip_whitespace(line);

			/* ignore blank/comment line */
			if (*entry == '\n' || *entry == '#')
				continue;

			/* pick up the mac address */
			mac = ether_aton(entry);
			if (NULL == mac) {
				fprintf(stderr, "failed to convert mac address\n");
				return 0;
			}
			fclose(list);
			goto do_wake;
		}
		fclose(list);
		fprintf(stderr, "failed to read entry in .pclist\n");
		return -1;
	} else if (argc == 2) {
		mac = ether_aton(argv[1]);
		if (NULL == mac) {
			fprintf(stderr, "failed to convert mac address\n");
			return 0;
		}
	} else {
		fprintf(stderr, "%s", help);
		return 0;
	}

do_wake:
	printf("Waking up personal computer, MAC address "
			"%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", 
			mac->ether_addr_octet[0],
			mac->ether_addr_octet[1],
			mac->ether_addr_octet[2],
			mac->ether_addr_octet[3],
			mac->ether_addr_octet[4],
			mac->ether_addr_octet[5]);

	/*
	 * create magic packet
	 */
	int size = 6;
	memset(magic, 0xFF, 6);
	for (i = 0; i < 16; i++) {
		memcpy(magic + size, mac->ether_addr_octet, 6);
		size += 6;
	}

	/*
	 * create socket
	 */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(9);
	sin.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		fprintf(stderr, "failed to get socket, error(%d)\n", errno);
		status = -1;
		goto out;
	}

	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) {
		fprintf(stderr, "failed to set sockopt, error(%d)\n", errno);
		status = -2;
		goto out;
	}

	/*
	 * wake pc up
	 */
	if (sendto(sockfd, magic, size, 0, (struct sockaddr*)&sin, sizeof(sin)) == -1) {
		fprintf(stderr, "failed to send Magic packet, error(%d)\n", errno);
		status = -3;
	}

out:
	close(sockfd);
	return status;
}
