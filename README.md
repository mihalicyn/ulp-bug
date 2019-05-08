ULP bug demonstration
========

An minimal example of ULP driver that cause error with module refcount.

How to caught a problem
===============

1. Compile and load driver:
```
./load.sh
```

2. Then start server ulp.out from tests directory
```
cd tests && make && ./ulp.out
```

3. Make some heavy load. For example:
```
ab -S -d -n 10000 -c 256 http://127.0.0.1:1337/;
```

4. Force stop server under load.

5. Check `dmesg` and lsmod...

You will got something like:
```
[ 1959.459050] WARNING: CPU: 3 PID: 3582 at kernel/module.c:1140 module_put+0x103/0x170
[ 1959.459051] Modules linked in: ulp_lua(O) [last unloaded: ulp_lua]
[ 1959.459053] CPU: 3 PID: 3582 Comm: ulp.out Tainted: G        W  O      5.0.5 #2
[ 1959.459054] Hardware name: QEMU Standard PC (i440FX + PIIX, 1996), BIOS 1.11.1-1ubuntu1 04/01/2014
[ 1959.459056] RIP: 0010:module_put+0x103/0x170
[ 1959.459058] Code: 00 00 48 8b 44 24 60 65 48 33 04 25 28 00 00 00 75 77 48 83 c4 68 5b 5d 41 5c 41 5d 41 5e c3 89 44 24 28 83 e8 01 89 c3 79 8c <0f> 0b eb b4 65 8b 05 12 95 2a 4e 89 c0 48 0f a3 05 f8 36 fc 01 73
[ 1959.459058] RSP: 0018:ffff8881e89478f8 EFLAGS: 00010297
[ 1959.459060] RAX: 0000000000000000 RBX: 00000000ffffffff RCX: ffffffffb1d6eb8e
[ 1959.459061] RDX: 0000000000000003 RSI: dffffc0000000000 RDI: ffffffffc0132590
[ 1959.459062] RBP: 1ffff1103d128f20 R08: fffffbfff80264b3 R09: fffffbfff80264b2
[ 1959.459063] R10: fffffbfff80264b2 R11: ffffffffc0132593 R12: ffffffffc0132280
[ 1959.459064] R13: ffffffffc0132590 R14: 0000000000000007 R15: 0000000000000008
[ 1959.459065] FS:  00007fb1af986500(0000) GS:ffff8881f7780000(0000) knlGS:0000000000000000
[ 1959.459067] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
[ 1959.459068] CR2: 00007fb1af8bdcb7 CR3: 00000001b0812004 CR4: 0000000000360ee0
[ 1959.459069] Call Trace:
[ 1959.459071]  ? find_exported_symbol_in_section+0x100/0x100
[ 1959.459073]  ? find_exported_symbol_in_section+0x100/0x100
[ 1959.459076]  ? kmsg_dump_rewind_nolock+0x54/0x54
[ 1959.459077]  tcp_cleanup_ulp+0x5d/0x80
[ 1959.459079]  tcp_v4_destroy_sock+0x76/0x2c0
[ 1959.459081]  inet_csk_destroy_sock+0x9d/0x1a0
[ 1959.459083]  tcp_close+0x46d/0x6c0
[ 1959.459085]  inet_release+0x6e/0xb0
[ 1959.459086]  __sock_release+0x69/0x120
[ 1959.459088]  sock_close+0xc/0x10
[ 1959.459090]  __fput+0x131/0x2f0
[ 1959.459092]  task_work_run+0xb9/0xe0
[ 1959.459094]  do_exit+0x48e/0x13c0
[ 1959.459096]  ? kmem_cache_free+0x182/0x1a0
[ 1959.459098]  ? release_task.part.20+0x8c0/0x8c0
[ 1959.459099]  ? inet_recvmsg+0xcb/0x1f0
[ 1959.459101]  ? kasan_unpoison_shadow+0x30/0x40
[ 1959.459102]  ? inet_sendpage+0x1a0/0x1a0
[ 1959.459104]  ? security_socket_recvmsg+0x48/0x60
[ 1959.459106]  do_group_exit+0x77/0x130
[ 1959.459108]  get_signal+0x1e7/0xa10
[ 1959.459110]  do_signal+0x8e/0xa40
[ 1959.459112]  ? setup_sigcontext+0x260/0x260
[ 1959.459130]  ? __audit_syscall_entry+0x19b/0x200
[ 1959.459132]  ? ktime_get_coarse_real_ts64+0x45/0x70
[ 1959.459134]  ? syscall_trace_enter+0x201/0x500
[ 1959.459136]  ? unroll_tree_refs+0x161/0x1d0
[ 1959.459138]  ? trace_event_raw_event_sys_exit+0x140/0x140
[ 1959.459139]  ? _raw_write_unlock_irqrestore+0x20/0x20
[ 1959.459141]  ? __audit_syscall_exit+0x3b2/0x400
[ 1959.459143]  exit_to_usermode_loop+0xc2/0xe0
[ 1959.459145]  do_syscall_64+0x114/0x140
[ 1959.459147]  entry_SYSCALL_64_after_hwframe+0x44/0xa9
[ 1959.459148] RIP: 0033:0x7fb1af8bdce1
[ 1959.459149] Code: Bad RIP value.
[ 1959.459150] RSP: 002b:00007fff29e8c9c8 EFLAGS: 00000246 ORIG_RAX: 000000000000002d
[ 1959.459151] RAX: fffffffffffffe00 RBX: 0000000000000003 RCX: 00007fb1af8bdce1
[ 1959.459152] RDX: 0000000000002000 RSI: 00005560c8a2e260 RDI: 0000000000000004
[ 1959.459153] RBP: 00007fb1af986480 R08: 0000000000000000 R09: 0000000000000000
[ 1959.459154] R10: 0000000000000000 R11: 0000000000000246 R12: 0000000000000004
[ 1959.459155] R13: 00005560c8a2e260 R14: 00007fff29e8ca00 R15: 00007fff29e8c9ec
[ 1959.459156] ---[ end trace a6510f54c32cd875 ]---
```

and

```
$ lsmod
Module                  Size  Used by
ulp_lua                16384  -1
```

Linux kernel version:
```
$ uname -a
Linux kern-dev 5.0.5 #2 SMP Wed Mar 27 11:39:37 MSK 2019 x86_64 GNU/Linux
```

References
=======

https://github.com/tammela/ulp-lua

Big thanks to Pedro Tammela.