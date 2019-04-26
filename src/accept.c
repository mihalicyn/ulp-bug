#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt "\n"
#include <linux/kernel.h>

#include <linux/net.h>
#include <linux/socket.h>
#include <net/tcp.h>

#include "ulp.h"
#include "syscalls.h"

struct sock *ulp_accept(struct sock *sk, int flags, int *err, bool kern)
{
   int ret;
   struct sock *reqsk;

   reqsk = sys->accept(sk, flags, err, kern);

   if (reqsk == NULL)
      return NULL;

   ret = sk_set_ulp_data(reqsk, CONNECTION);
   // should never happen
   if (ret) {
      goto close;
   }

   //try_module_get(THIS_MODULE);

   return reqsk;
close:
   BUG_ON(true);
   inet_csk(reqsk)->icsk_ulp_ops = NULL;
   sys->close(reqsk, 0);
   return NULL;
}
