#include <linux/module.h>

#include <linux/kernel.h>

#include <linux/net.h>

#include <linux/types.h>

#include <linux/skbuff.h>

#include <linux/slab.h>

#include <linux/netfilter.h>

#include <linux/netfilter_ipv6.h>

#include <linux/ipv6.h>

#include <linux/in6.h>

#include <linux/tcp.h>

#include <linux/errno.h>

#include <linux/rcupdate.h>

#include <net/tcp.h>

#include <net/ipv6.h>

#include "ip6_option.h"



/* Various flags used by the module */

/* This flag makes sure that only one instance of the ip6 option device

 * can be in use at any one time. */

static int ip6_option_in_use = 0;



/* Control device major number */

static int major = 0; 



/* This struct will describe our hook procedure. */

struct nf_hook_ops nf_ip6_option;



/* Some function prototypes to be used by radio_aware_fops and tcp_option_ops below. */

static int ip6_option_open(struct inode *inode, struct file *file);

static int ip6_option_release(struct inode *inode, struct file *file);

static int ip6_option_ioctl(struct inode *inode, struct file *file,

    unsigned int cmd, unsigned long arg);



/*

 * This is the interface device's file_operations structure

 */

struct file_operations ip6_option_fops = {

    .owner = THIS_MODULE,

    .open = ip6_option_open,

    .release = ip6_option_release,

    .ioctl = ip6_option_ioctl

};



/*********************************************/

/*

* File operations functions for control device

*/



static int ip6_option_open(struct inode *inode, struct file *file)

{

    PR_DEBUG(DRV_NAME ": enter ip6_option_open()\n");

    

    if (ip6_option_in_use) {

        pr_info(DRV_NAME ":ip6_option_in_use\n");

        return -EBUSY;

    } else {

        try_module_get(ip6_option_fops.owner);

        ip6_option_in_use++;

        return 0;

    }

}



static int ip6_option_release(struct inode *inode, struct file *file)

{

    PR_DEBUG(DRV_NAME ": enter ip6_option_release()\n");

    

    ip6_option_in_use ^= ip6_option_in_use;

    module_put(ip6_option_fops.owner);



    return 0;

}



static int ip6_option_ioctl(struct inode *inode, struct file *file,

    unsigned int cmd, unsigned long arg)

{

    PR_DEBUG(DRV_NAME ": enter ip6_option_ioctl(), cmd:%d\n", cmd);

    

    return 0;

}



/*

 * This is the function that will be called by the hook

 */

unsigned int try_get_ip6_option_hookfn(unsigned int hooknum,

                   struct sk_buff *skb,

                   const struct net_device *in,

                   const struct net_device *out,

                   int (*okfn)(struct sk_buff *))

{

    struct ipv6hdr *hdr;

    struct ipv6_rt_hdr *rt_hdr;

    unsigned int nhoff = 0;

    int nexthdr;

    unsigned int rthdr_len;

    u32 pkt_len;

    int i;

 

    if (skb->pkt_type == PACKET_OTHERHOST) {

        kfree_skb(skb);

        return NET_RX_DROP;

    }



    rcu_read_lock();



    hdr = ipv6_hdr(skb);



    if (hdr->version != 6)

        goto drop;



    nhoff = offsetof(struct ipv6hdr, nexthdr);

    

    /* 

     *  Assueme packets from IB only have HOP-BY-HOP, ROUTING and TCP header,

     *  If any other header exist, never to retrieve the terminal IP.

     *  Note: This doesn't obey the RFC2460.

     */

    if (hdr->nexthdr == NEXTHDR_HOP)

        nhoff = sizeof(struct ipv6hdr);



    nexthdr = skb_network_header(skb)[nhoff];

    if (nexthdr == NEXTHDR_ROUTING) {

        pkt_len = ntohs(hdr->payload_len);



        

        rt_hdr = (struct ipv6_rt_hdr *)(skb_network_header(skb) + sizeof(*hdr));

        if (rt_hdr->nexthdr == NEXTHDR_TCP) {

            rthdr_len = (rt_hdr->hdrlen + 1) << 3;



            //change

            rt_hdr->segments_left = 0;

            rt_hdr->type = 1;



            nhoff = (&rt_hdr->nexthdr) - skb_network_header(skb);

        }

        

    }

    

    rcu_read_unlock();    

    return NF_ACCEPT;

    

drop:

    rcu_read_unlock();

    kfree_skb(skb);

    return NET_RX_DROP;

}



/*********************************************/

/*

 * Module initialisation and cleanup follow...

 */

static int __init ip6_option_init(void)

{

    /* Attempt to register the IPv6 Option control device */

    major = register_chrdev(IP6_OPTION_MAJOR, IP6_OPTION_NAME, &ip6_option_fops);

    if (major < 0) {

        pr_err(DRV_NAME ": Failed registering control device!\n");

		pr_err(DRV_NAME ": Module installation aborted.\n");



        return major;

    }



    /* Make sure the usage marker for the control device is cleared */

    ip6_option_in_use ^= ip6_option_in_use;

    

    pr_info(DRV_NAME ": Control device successfully registered.\n");



    /* Now register the network hooks */

    nf_ip6_option.hook = try_get_ip6_option_hookfn;

    nf_ip6_option.hooknum = NF_INET_PRE_ROUTING;

    nf_ip6_option.pf = PF_INET6;

    nf_ip6_option.priority = NF_IP6_PRI_FIRST;



    /* And register... */

	nf_register_hook(&nf_ip6_option);



    pr_info(DRV_NAME ": Network hooks successfully installed.\n");

	pr_info(DRV_NAME ": Module installation successful.\n");



    return 0;

}



static void __exit ip6_option_exit(void)

{

	/* Remove IPv6 hook */

    nf_unregister_hook(&nf_ip6_option);



	/* Now unregister control device */

    unregister_chrdev(IP6_OPTION_MAJOR, IP6_OPTION_NAME);

    

    pr_info(DRV_NAME ": Removal of module successful.\n");

}



MODULE_LICENSE("GPL");

MODULE_AUTHOR("TEST");

MODULE_DESCRIPTION("Retrieve IPv6 Option for Linux 2.6");



module_init(ip6_option_init);

module_exit(ip6_option_exit);



