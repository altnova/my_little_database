//! \file srv.c \brief tcp server

#include "../___.h"
#include "srv.h"

#define PORT 5000
#define PROMPT "server$ "

ZI p = -1; // port we are listening on
Z STDIN_CALLBACK stdin_fn;
Z RECV_CALLBACK recv_fn;

ZI srv_each(I d){
	LOG("srv_each");
	I p[2];acc(p,d);
	I nd = sd2(*p,recv_fn,p[1]);
	T(TEST, "client connected (%d)", nd);
	R0;}

ZI srv_bind_stdin(I d) {
	P(!stdin_fn, 1);
	R tcp_stdin(d, stdin_fn);}

V srv_set_stdin_callback(STDIN_CALLBACK fn) {
	stdin_fn = fn;
	sd1(0,srv_bind_stdin);}

I srv_listen(I addr, I port, RECV_CALLBACK fn) {
	LOG("srv_listen");
	p = lstn(addr,port);
	W(p<0) {
		p = lstn(addr,port);T(WARN, "%d:%d (%d: %s)", addr, port, errno, strerror(errno));
		sleep(1);
	}
	recv_fn = fn;
	sd1(p,srv_each);
	T(TEST, "listening on %d:%d", addr, port);

	R tcp_serve();
}

V srv_shutdown() {
	LOG("srv_shutdown");
	msg_shutdown();
	tcp_shutdown();
	exit(0);}

I srv_init() {
	signal(SIGPIPE, SIG_IGN);
	msg_init();
	R tcp_init();}

#ifdef RUN_TESTS_SRV

ZI srv_on_msg(I d, MSG_HDR *h, pMSG *m) {
	LOG("srv_on_msg");
	if(h->type==SAY_res) {
		pSAY_res *t = (pSAY_res*)m;
		T(TEST, "rcvd SAY(%d): %.*s", t->cnt, t->cnt, t->msg);
	}
	else if(h->type==HEY_req) {
		MSG m = rpc_SAY_res(20,"Pleased to meet you.");
		snd(d, m, msg_size(m));
		T(TEST, "sent SAY %d bytes", msg_size(m));
		//msg_hdr_dump(&m->hdr);
	}
	R0;}

I srv_test_stdin(I d, UI l, S str) {
	LOG("srv_stdin_callback");
	if(l&&!mcmp(str,"s",1)) {
		MSG m = rpc_HEY_req(5,"server");
		snd(d, m, msg_size(m));
		T(TEST, "sent HEY %d bytes", msg_size(m));
		msg_hdr_dump(&m->hdr);
		free(m);}
	write(d, PROMPT, scnt(PROMPT));
	R0;}

I main() {
	LOG("srv");
	signal(SIGINT, srv_shutdown); //< catch SIGINT and cleanup nicely

	msg_set_callback(srv_on_msg);

	srv_init();
	srv_set_stdin_callback(srv_test_stdin);

	srv_listen(0, PORT, msg_recv);

	srv_shutdown();
}

#endif
