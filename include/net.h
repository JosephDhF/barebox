/*
 *	LiMon Monitor (LiMon) - Network.
 *
 *	Copyright 1994 - 2000 Neil Russell.
 *	(See License)
 *
 *
 * History
 *	9/16/00	  bor  adapted to TQM823L/STK8xxL board, RARP/TFTP boot added
 */

#ifndef __NET_H__
#define __NET_H__

#include <driver.h>
#include <linux/types.h>
#include <param.h>
#include <malloc.h>
#include <stdlib.h>
#include <clock.h>
#include <led.h>
#include <asm/byteorder.h>	/* for nton* / ntoh* stuff */

/* How often do we retry to send packages */
#define PKT_NUM_RETRIES 4

/* The number of receive packet buffers */
#define PKTBUFSRX	4

struct device_d;

struct eth_device {
	int active;

	int  (*init) (struct eth_device*);

	int  (*open) (struct eth_device*);
	int  (*send) (struct eth_device*, void *packet, int length);
	int  (*recv) (struct eth_device*);
	void (*halt) (struct eth_device*);
	int  (*get_ethaddr) (struct eth_device*, unsigned char *adr);
	int  (*set_ethaddr) (struct eth_device*, unsigned char *adr);

	struct eth_device *next;
	void *priv;

	struct device_d dev;
	struct device_d *parent;

	struct list_head list;
};

#define dev_to_edev(d) container_of(d, struct eth_device, dev)

int eth_register(struct eth_device* dev);    /* Register network device		*/
void eth_unregister(struct eth_device* dev); /* Unregister network device	*/

int eth_open(void);			/* open the device		*/
int eth_send(void *packet, int length);	   /* Send a packet		*/
int eth_rx(void);			/* Check for received packets	*/
void eth_halt(void);			/* stop SCC			*/

/* associate a MAC address to a ethernet device. Should be called by
 * board code for boards which store their MAC address at some unusual
 * place.
 */
#if !defined(CONFIG_NET)
static inline void eth_register_ethaddr(int ethid, const char *ethaddr)
{
}
#else
void eth_register_ethaddr(int ethid, const char *ethaddr);
#endif
/*
 *	Ethernet header
 */
struct ethernet {
	uint8_t		et_dest[6];	/* Destination node		*/
	uint8_t		et_src[6];	/* Source node			*/
	uint16_t	et_protlen;	/* Protocol or length		*/
} __attribute__ ((packed));

#define ETHER_HDR_SIZE	14		/* Ethernet header size		*/

#define PROT_IP		0x0800		/* IP protocol			*/
#define PROT_ARP	0x0806		/* IP ARP protocol		*/
#define PROT_RARP	0x8035		/* IP ARP protocol		*/
#define PROT_VLAN	0x8100		/* IEEE 802.1q protocol		*/

#define IPPROTO_ICMP	 1	/* Internet Control Message Protocol	*/
#define IPPROTO_UDP	17	/* User Datagram Protocol		*/

/*
 *	Internet Protocol (IP) header.
 */
struct iphdr {
	uint8_t		hl_v;
	uint8_t		tos;
	uint16_t	tot_len;
	uint16_t	id;
	uint16_t	frag_off;
	uint8_t		ttl;
	uint8_t		protocol;
	uint16_t	check;
	uint32_t	saddr;
	uint32_t	daddr;
	/* The options start here. */
} __attribute__ ((packed));

struct udphdr {
	uint16_t	uh_sport;	/* source port */
	uint16_t	uh_dport;	/* destination port */
	uint16_t	uh_ulen;	/* udp length */
	uint16_t	uh_sum;		/* udp checksum */
} __attribute__ ((packed));

/*
 *	Address Resolution Protocol (ARP) header.
 */
struct arprequest
{
	uint16_t	ar_hrd;		/* Format of hardware address	*/
#define ARP_ETHER	1		/* Ethernet  hardware address	*/
	uint16_t	ar_pro;		/* Format of protocol address	*/
	uint8_t		ar_hln;		/* Length of hardware address	*/
	uint8_t		ar_pln;		/* Length of protocol address	*/
	uint16_t	ar_op;		/* Operation			*/
#define ARPOP_REQUEST	1		/* Request  to resolve  address	*/
#define ARPOP_REPLY	2		/* Response to previous request	*/

#define RARPOP_REQUEST	3		/* Request  to resolve  address	*/
#define RARPOP_REPLY	4		/* Response to previous request */

	/*
	 * The remaining fields are variable in size, according to
	 * the sizes above, and are defined as appropriate for
	 * specific hardware/protocol combinations.
	 */
	uint8_t		ar_data[0];
} __attribute__ ((packed));

#define ARP_HDR_SIZE	(8 + 20)	/* Size assuming ethernet	*/

/*
 * ICMP stuff (just enough to handle (host) redirect messages)
 */
#define ICMP_ECHO_REPLY		0	/* Echo reply 			*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO_REQUEST	8	/* Echo request			*/

/* Codes for REDIRECT. */
#define ICMP_REDIR_NET		0	/* Redirect Net			*/
#define ICMP_REDIR_HOST		1	/* Redirect Host		*/

struct icmphdr {
	uint8_t		type;
	uint8_t		code;
	uint16_t	checksum;
	union {
		struct {
			uint16_t	id;
			uint16_t	sequence;
		} echo;
		uint32_t	gateway;
		struct {
			uint16_t	__unused;
			uint16_t	mtu;
		} frag;
	} un;
} __attribute__ ((packed));


/*
 * Maximum packet size; used to allocate packet storage.
 * TFTP packets can be 524 bytes + IP header + ethernet header.
 * Lets be conservative, and go for 38 * 16.  (Must also be
 * a multiple of 32 bytes).
 */
#define PKTSIZE			1518

/**********************************************************************/
/*
 *	Globals.
 *
 * Note:
 *
 * All variables of type IPaddr_t are stored in NETWORK byte order
 * (big endian).
 */

extern unsigned char *NetRxPackets[PKTBUFSRX];/* Receive packets		*/

void net_set_ip(IPaddr_t ip);
void net_set_serverip(IPaddr_t ip);
void net_set_netmask(IPaddr_t ip);
void net_set_gateway(IPaddr_t ip);
IPaddr_t net_get_ip(void);
IPaddr_t net_get_serverip(void);

/* Do the work */
void net_poll(void);

static inline struct iphdr *net_eth_to_iphdr(char *pkt)
{
	return (struct iphdr *)(pkt + ETHER_HDR_SIZE);
}

static inline struct udphdr *net_eth_to_udphdr(char *pkt)
{
	return (struct udphdr *)(net_eth_to_iphdr(pkt) + 1);
}

static inline struct icmphdr *net_eth_to_icmphdr(char *pkt)
{
	return (struct icmphdr *)(net_eth_to_iphdr(pkt) + 1);
}

static inline char *net_eth_to_icmp_payload(char *pkt)
{
	return (char *)(net_eth_to_icmphdr(pkt) + 1);
}

static inline char *net_eth_to_udp_payload(char *pkt)
{
	return (char *)(net_eth_to_udphdr(pkt) + 1);
}

static inline int net_eth_to_udplen(char *pkt)
{
	struct udphdr *udp = net_eth_to_udphdr(pkt);
	return ntohs(udp->uh_ulen) - 8;
}

int net_checksum_ok(unsigned char *, int);	/* Return true if cksum OK	*/
uint16_t net_checksum(unsigned char *, int);	/* Calculate the checksum	*/

/* Print an IP address on the console */
void print_IPaddr (IPaddr_t);

/*
 * The following functions are a bit ugly, but necessary to deal with
 * alignment restrictions on ARM.
 *
 * We're using inline functions, which had the smallest memory
 * footprint in our tests.
 */
/* return IP *in network byteorder* */
static inline IPaddr_t net_read_ip(void *from)
{
	IPaddr_t ip;
	memcpy((void*)&ip, from, sizeof(ip));
	return ip;
}

/* return uint32 *in network byteorder* */
static inline uint32_t net_read_uint32(uint32_t *from)
{
	ulong l;
	memcpy((void*)&l, (void*)from, sizeof(l));
	return l;
}

/* write IP *in network byteorder* */
static inline void net_write_ip(void *to, IPaddr_t ip)
{
	memcpy(to, (void*)&ip, sizeof(ip));
}

/* copy IP */
static inline void net_copy_ip(void *to, void *from)
{
	memcpy(to, from, sizeof(IPaddr_t));
}

/* copy ulong */
static inline void net_copy_uint32(uint32_t *to, uint32_t *from)
{
	memcpy(to, from, sizeof(uint32_t));
}

/* Convert an IP address to a string */
char *ip_to_string (IPaddr_t x);

/* Convert a string to ip address */
int string_to_ip(const char *s, IPaddr_t *ip);

IPaddr_t getenv_ip(const char *name);
int setenv_ip(const char *name, IPaddr_t ip);

int string_to_ethaddr(const char *str, char *enetaddr);
void ethaddr_to_string(const unsigned char *enetaddr, char *str);

#ifdef CONFIG_NET_RESOLV
IPaddr_t resolv(char *host);
#else
static inline IPaddr_t resolv(char *host)
{
	IPaddr_t ip = 0;
	string_to_ip(host, &ip);
	return ip;
}
#endif

/**
 * is_zero_ether_addr - Determine if give Ethernet address is all zeros.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is all zeroes.
 */
static inline int is_zero_ether_addr(const u8 *addr)
{
	return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}

/**
 * is_multicast_ether_addr - Determine if the Ethernet address is a multicast.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is a multicast address.
 * By definition the broadcast address is also a multicast address.
 */
static inline int is_multicast_ether_addr(const u8 *addr)
{
	return (0x01 & addr[0]);
}

/**
 * is_local_ether_addr - Determine if the Ethernet address is locally-assigned one (IEEE 802).
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is a local address.
 */
static inline int is_local_ether_addr(const u8 *addr)
{
	return (0x02 & addr[0]);
}

/**
 * is_broadcast_ether_addr - Determine if the Ethernet address is broadcast
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is the broadcast address.
 */
static inline int is_broadcast_ether_addr(const u8 *addr)
{
	return (addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xff;
}

/**
 * random_ether_addr - Generate software assigned random Ethernet address
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Generate a random Ethernet address (MAC) that is not multicast
 * and has the local assigned bit set.
 */
static inline void random_ether_addr(u8 *addr)
{
	srand(get_time_ns());
	get_random_bytes(addr, 6);
	addr [0] &= 0xfe;	/* clear multicast bit */
	addr [0] |= 0x02;	/* set local assignment bit (IEEE802) */
}

/**
 * is_valid_ether_addr - Determine if the given Ethernet address is valid
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Check that the Ethernet address (MAC) is not 00:00:00:00:00:00, is not
 * a multicast address, and is not FF:FF:FF:FF:FF:FF.
 *
 * Return true if the address is valid.
 */
static inline int is_valid_ether_addr(const u8 *addr)
{
	/* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
	 * explicitly check for it here. */
	return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}

typedef void rx_handler_f(void *ctx, char *packet, unsigned int len);

void eth_set_current(struct eth_device *eth);
struct eth_device *eth_get_current(void);
struct eth_device *eth_get_byname(char *name);
void net_update_env(void);

/**
 * net_receive - Pass a received packet from an ethernet driver to the protocol stack
 * @pkt: Pointer to the packet
 * @len: length of the packet
 *
 * Return 0 if the packet is successfully handled. Can be ignored
 */
int net_receive(unsigned char *pkt, int len);

struct net_connection {
	struct ethernet *et;
	struct iphdr *ip;
	struct udphdr *udp;
	struct icmphdr *icmp;
	unsigned char *packet;
	struct list_head list;
	rx_handler_f *handler;
	int proto;
	void *priv;
};

static inline char *net_alloc_packet(void)
{
	return xmemalign(32, PKTSIZE);
}

struct net_connection *net_udp_new(IPaddr_t dest, uint16_t dport,
		rx_handler_f *handler, void *ctx);

struct net_connection *net_icmp_new(IPaddr_t dest, rx_handler_f *handler,
		void *ctx);

void net_unregister(struct net_connection *con);

static inline int net_udp_bind(struct net_connection *con, int sport)
{
	con->udp->uh_sport = ntohs(sport);
	return 0;
}

static inline void *net_udp_get_payload(struct net_connection *con)
{
	return con->packet + sizeof(struct ethernet) + sizeof(struct iphdr) +
		sizeof(struct udphdr);
}

int net_udp_send(struct net_connection *con, int len);
int net_icmp_send(struct net_connection *con, int len);

void led_trigger_network(enum led_trigger trigger);

#endif /* __NET_H__ */
