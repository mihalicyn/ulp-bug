#include <linux/kernel.h>

#include <linux/net.h>
#include <linux/socket.h>
#include <net/tcp.h>

#include "ulp.h"
#include "syscalls.h"

void ulp_close(struct sock *sk, long int timeout)
{
   struct ulp_data *data;

   // behavior changes with that lock
   lock_sock(sk);

   data = inet_csk(sk)->icsk_ulp_data;
   BUG_ON(data == NULL);

   if (data->type == CONNECTION) {
      // We need to NULL icsk_ulp_ops to prevent module_put call inside tcp_ulp.c in kernel
      inet_csk(sk)->icsk_ulp_ops = NULL;

      //module_put(THIS_MODULE);
   } else {
      printk("Non connection socket %p %p\n", (void *)sk, (void *)inet_csk(sk)->icsk_ulp_data);
   }

   sk_cleanup_ulp_data(sk);

   release_sock(sk);

   sys->close(sk, timeout);
}
