#ifndef _SYSCALLS_H
#define _SYSCALLS_H

/* defined in module.c */
extern struct proto *sys;

extern struct sock *ulp_accept(struct sock *, int, int *, bool);

extern int ulp_listen(struct socket *sock, int backlog);

extern void ulp_close(struct sock *, long int);

#endif
