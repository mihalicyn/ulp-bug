#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt "\n"
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/net.h>
#include <linux/socket.h>
#include <net/tcp.h>

#include "ulp.h"
#include "syscalls.h"

struct proto *sys;

static struct proto newprot;

static void ulp_destroy(struct sock *sk)
{
   struct ulp_data *data;

   sock_owned_by_me(sk);

   data = inet_csk(sk)->icsk_ulp_data;

   if (inet_csk(sk)->icsk_ulp_ops) {
      printk("ulp_destroy %d %p %p\n", sk->sk_state, (void *)sk, (void *)inet_csk(sk)->icsk_ulp_data);
   }

   sk->sk_prot = sys;

   sys->destroy(sk);
}

static void register_funcs(struct proto **skp)
{
   newprot.accept = ulp_accept;
   newprot.close = ulp_close;
   newprot.destroy = ulp_destroy;

   *skp = &newprot;
}

static int sk_init(struct sock *sk)
{
   if (sk->sk_family != AF_INET)
      return -ENOTSUPP;

   /* save the original state */
   sys = sk->sk_prot;
   newprot = *(sk->sk_prot);

   register_funcs(&sk->sk_prot);

   return sk_set_ulp_data(sk, LISTENER);
}

static int ulp_lua_init(struct sock *sk)
{
   /* the ulp protocol is cloned in the creation
    * of the request sockets. we assume that the
    * ulp initializion was done on the listener socket.
    */
   if (sk->sk_state == TCP_ESTABLISHED)
      return -EINVAL;

   return sk_init(sk);
}

static struct tcp_ulp_ops ulp_lua_ops __read_mostly = {
   .name          = "lua",
   .owner         = THIS_MODULE,
   .init          = ulp_lua_init
};

static int __init modinit(void)
{
   tcp_register_ulp(&ulp_lua_ops);
   return 0;
}

static void __exit modexit(void)
{
   tcp_unregister_ulp(&ulp_lua_ops);
}

module_init(modinit);
module_exit(modexit);
MODULE_AUTHOR("Lua in Kernel Team (https://github.com/luainkernel)");
MODULE_ALIAS_TCP_ULP("lua");
MODULE_LICENSE("GPL");
