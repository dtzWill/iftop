/*
 * options.c:
 *
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>

#include "iftop.h"
#include "options.h"

options_t options;

char optstr[] = "+i:f:n:dhpb";

/* Global options. */

static char *get_first_interface(void) {
    int s, size = 1;
    struct ifreq *ifr;
    struct ifconf ifc = {0};
    char *i = NULL;
    /* Use if_nameindex(3) instead? */
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        return NULL;
    ifc.ifc_len = sizeof *ifr;
    do {
        ++size;
        ifc.ifc_req = xrealloc(ifc.ifc_req, size * sizeof *ifc.ifc_req);
        ifc.ifc_len = size * sizeof *ifc.ifc_req;
        if (ioctl(s, SIOCGIFCONF, &ifc) == -1) {
            perror("SIOCGIFCONF");
            return NULL;
        }
    } while (size * sizeof *ifc.ifc_req <= ifc.ifc_len);
    /* Ugly. */
    for (ifr = ifc.ifc_req; (char*)ifr < (char*)ifc.ifc_req + ifc.ifc_len; ++ifr) {
        if (strcmp(ifr->ifr_name, "lo") != 0 && strncmp(ifr->ifr_name, "dummy", 5) != 0
            && ioctl(s, SIOCGIFFLAGS, ifr) == 0 && ifr->ifr_flags & IFF_UP) {
            i = xstrdup(ifr->ifr_name);
            break;
        }
    }
    xfree(ifc.ifc_req);
    return i;
}

static void set_defaults() {
    /* Should go through the list of interfaces, and find the first one which
     * is up and is not lo or dummy*. */
    options.interface = get_first_interface();
    if (!options.interface)
        options.interface = "eth0";

    options.filtercode = NULL;
    options.netfilter = 0;
    inet_aton("10.0.1.0", &options.netfilternet);
    inet_aton("255.255.255.0", &options.netfiltermask);
    options.dnsresolution = 1;
    options.promiscuous = 0;
    options.showbars = 1;
    options.aggregate = OPTION_AGGREGATE_OFF;
}

static void die(char *msg) {
    fprintf(stderr, msg);
    exit(1);
}

static void set_net_filter(char* arg) {
    char* mask;

    mask = strchr(arg, '/');
    if (mask == NULL) {
        die("Could not parse net/mask\n");
    }
    *mask = '\0';
    mask++;
    if (inet_aton(arg, &options.netfilternet) == 0)
        die("Invalid network address\n");
    /* Accept a netmask like /24 or /255.255.255.0. */
    if (!mask[strspn(mask, "0123456789")]) {
        int n;
        n = atoi(mask);
        if (n > 32)
            die("Invalid netmask");
        else {
            uint32_t mm = 0xffffffffl;
            mm >>= n;
            options.netfiltermask.s_addr = htonl(~mm);
        }
    } else if (inet_aton(mask, &options.netfiltermask) == 0)
        die("Invalid netmask\n");

    options.netfilter = 1;

}

/* usage:
 * Print usage information. */
static void usage(FILE *fp) {
    fprintf(fp,
"iftop: display bandwidth usage on an interface by host\n"
"\n"
"Synopsis: iftop -h | [-dpb] [-i interface] [-f filter code] [-n net/mask]\n"
"\n"
"   -h                  display this message\n"
"   -d                  don't do hostname lookups\n"
"   -p                  run in promiscuous mode (show traffic between other\n"
"                       hosts on the same network segment)\n"
"   -b                  don't display a bar graph of traffic\n"
"   -i interface        listen on named interface (default: eth0)\n"
"   -f filter code      use filter code to select packets to count\n"
"                       (default: none, but only IP packets are counted)\n"
"   -n net/mask         show traffic flows in/out of network\n"
"\n"
"iftop, version " IFTOP_VERSION " copyright (c) 2002 Paul Warren <pdw@ex-parrot.com>\n"
            );
}

void options_read(int argc, char **argv) {
    int opt;

    set_defaults();

    opterr = 0;
    while ((opt = getopt(argc, argv, optstr)) != -1) {
        switch (opt) {
            case 'h':
                usage(stdout);
                exit(0);

            case 'd':
                options.dnsresolution = 0;
                break;

            case 'i':
                options.interface = optarg;
                break;

            case 'f':
                options.filtercode = optarg;
                break;

            case 'p':
                options.promiscuous = 1;
                break;

            case 'n':
                set_net_filter(optarg);
                break;

            case 'b':
                options.showbars = 0;
                break;


            case '?':
                fprintf(stderr, "iftop: unknown option -%c\n", optopt);
                usage(stderr);
                exit(1);

            case ':':
                fprintf(stderr, "iftop: option -%c requires an argument\n", optopt);
                usage(stderr);
                exit(1);
        }
    }

    if (optind != argc)
        fprintf(stderr, "iftop: warning: ignored arguments following options\n");
}
