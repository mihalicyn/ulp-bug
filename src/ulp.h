#ifndef _ULP_H
#define _ULP_H

#include <linux/net.h>
#include <linux/socket.h>
#include <net/tcp.h>

#include "conf.h"

/* We must define SOL_LUA in /include/linux/socket.h */
#define SOL_LUA                (999)

/* #include <uapi/linux/ulp_lua.h> */
#define ULP_LOADSCRIPT      (1)
#define ULP_ENTRYPOINT      (2)

typedef enum {
   LISTENER = 1,
   CONNECTION
} ulp_data_type;

// this structure can contain some useful data, but for minimal example contains only socket type
struct ulp_data {
   ulp_data_type type;
};

static inline int sk_set_ulp_data(struct sock *sk, ulp_data_type type)
{
   struct ulp_data *ulp_data;

   ulp_data = kmalloc(sizeof(struct ulp_data), GFP_KERNEL);
   if (unlikely(ulp_data == NULL))
      return -ENOMEM;

   ulp_data->type = type;

   inet_csk(sk)->icsk_ulp_data = ulp_data;

   return 0;
}

static inline void sk_cleanup_ulp_data(struct sock *sk)
{
   struct ulp_data *data = inet_csk(sk)->icsk_ulp_data;
   WARN_ON(data == NULL);
   if (unlikely(data == NULL))
      return;

   inet_csk(sk)->icsk_ulp_data = NULL;
   kfree(data);
}

#endif
