//! \file tcp.h \brief comms api

#pragma once

#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

Z struct hostent*h;
Z struct sockaddr_in sa={AF_INET};
typedef struct sockaddr*Sa;
typedef struct item{V*obj;UI size;struct item* next;}pQ;
typedef pQ* Q;

#define CN              1024 //< max connections

typedef V(*TCP_TIMER_FN)(UJ i);
typedef I(*STDIN_CALLBACK)(I d,UI strlen,S str);
ext I  tcp_stdin(I d, STDIN_CALLBACK fn);

ext I  port(I d);
ext S  host(I a);
ext I  addr(S s);
ext V  ipv4(I a,S b);
ext I  conn(I a,I p);
ext I  lstn(I a,I p);
ext I  acc(I*p,I d);
ext I  rcv(I d,V*b,I n);
ext I  snd(I d,V*b,I n);
ext V  end(I d);
ext I  clo(I d);
ext I  poke();

ext I  sd0(I d);
ext I  sd1(I d,I(*fn)(I));
ext I  sd2(I d,I(*fn)(I),I addr);
ext I  cons(I d);

ext I  tcp_trigger_reads();
ext I  tcp_select(struct timeval tv);
ext I  tcp_init();
ext I  tcp_shutdown();
ext I  tcp_serve();
ext I  tcp_active_conns();
ext V  tcp_set_timer(J t, TCP_TIMER_FN fn);
ext V* tcp_buf(I d,I n);

//:~

