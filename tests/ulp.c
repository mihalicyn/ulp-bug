#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/tcp.h>

#define ULP_LOADSCRIPT       (1)
#define ULP_ENTRYPOINT       (2)
#define SOL_TCP (6)
#define SOL_LUA (999)

#define raise_err()  \
   ({fprintf(stderr, "%s:%s (%d)", strerror(errno), __func__, __LINE__); exit(-1);})

int main(int argc, char **argv)
{
   const char *hello = "Hello from server!";
   int listener = socket(AF_INET, SOCK_STREAM, 0);
   int err;

   if (listener == -1)
      raise_err();

   struct sockaddr_in addr =
      {.sin_family = AF_INET,
      .sin_addr.s_addr = INADDR_ANY,
      .sin_port = htons(1337)};

   int on = 1;
   err = setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &on,sizeof(int));
   if (err == -1)
      raise_err();

   err = bind(listener, (struct sockaddr *) &addr, sizeof(addr));
   if (err == -1)
      raise_err();

   err = listen(listener, 32);
   if (err == -1)
      raise_err();

   /* setup ulp */
   err = setsockopt(listener, SOL_TCP, TCP_ULP, "lua", sizeof("lua"));
   if (err == -1)
      raise_err();

   char *msg = malloc(8192);
   if (msg == NULL)
      raise_err();

   struct sockaddr_in cl;
   socklen_t len;
   while(1) {
      int sock = accept(listener, (struct sockaddr *) &cl, &len);
      if (sock == -1)
         raise_err();

      size_t msgsz = recv(sock, msg, 8192, 0);
      if (msgsz == 0) {
         close(sock);
         printf("Empty message\n");
         continue;
      }

      if (msgsz == -1) {
         if (errno == ECONNREFUSED) {
            close(sock);
            printf("ECONNREFUSED\n");
            continue;
         }
         raise_err();
      }

      sprintf(msg,"HTTP/1.1 200 OK\r\nServer: ulp-lua/1.0\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: text/plain\r\n\r\n%s", strlen(hello), hello);
      write(sock, msg, strlen(msg));

      shutdown(sock, SHUT_WR);
      close(sock);
   }

   close(listener);
   free(msg);

   return 0;
}
