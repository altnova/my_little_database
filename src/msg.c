//! \file msg.c \brief simple data retrieval protocol

#define SIZETYPE UI

#include <pwd.h>

#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "msg.h"
#include "tcp.h"
#include "rpc.h"
#include "str.h"

#define SZ_MSG_HDR SZ(MSG_HDR)

Z MSG mbuf;

V msg_dump_header(MSG m) {
	LOG("msg_dump_header");
	MSG_HDR h = m->hdr;
	T(TEST, "ver -> %d", h.ver);
	T(TEST, "type -> %d (%s)", h.type, MSG_LABELS[h.type]);
	T(TEST, "len -> %d", h.len);
	UI tail_offset = MSG_TAIL_OFFSET[h.type];
	if(!tail_offset)R;
	T(TEST, "tail_offset -> %d", tail_offset);
	T(TEST, "tail_len -> %d", *(SIZETYPE*)(((V*)&m->msg)+tail_offset));
	T(TEST, "tail -> %s", (S)(((V*)&m->msg)+tail_offset+SZ(SIZETYPE)));
}

/*
V print_hdr(MSG m) {
	LOG("print_hdr");
	T(TEST, "mgc %d",m->magic);
	T(TEST, "dir %.3s",m->dir);
	T(TEST, "cmd %.3s",m->cmd);
	T(TEST, "arg %d %d",m->arg[0],m->arg[1]);
	T(TEST, "cnt %d",m->cnt);
	T(TEST, "sze %d",m->size);}

I s_req_hlo(I d, I majver, I minver) {
	LOG("s_req_hlo");
	MSG m = make_msg("REQ","HLO",majver,minver,0,0,0);
	T(TEST, "sending document...");
	I r = snd(d, m, SZ_MSG_HDR + m->size);
	T(TEST, "sent, result= %d",r);
	free(m);
	R0;
}

*/

I recv_msg(I d) {
	LOG("recv_msg");
	G b[SZ_MSG_HDR]; I n=rcv(d,b,SZ_MSG_HDR);
	O("[~] rcvd_msg %d\n", n);
	P(n<=0,sd0(d))
	//print_hdr((MSG)b);
	//P(42!=b[0]||n<0||n==0,(/*clo(d),*/-1))
	T(TEST, "rcvd header %d bytes:");
	//print_hdr((MSG)b);
	X(42!=b[0], T(WARN, "msg magic value is not 42"), -1);
	T(TEST, "rcvd header %d bytes, actual msg header size: %d", n, SZ_MSG_HDR);
	//mbuf = realloc(mbuf, SZ_MSG_HDR + mbuf->size); chk(mbuf,0);
	//T(TEST, "full msg size is %d+%d = ", SZ_MSG_HDR, mbuf->size, SZ_MSG_HDR + mbuf->size);
	//n=rcv(d, mbuf+SZ_MSG_HDR, mbuf->size);
	//T(TEST, "full message should be loaded now, %d more bytes rcvd", n);
	//print_hdr(mbuf);
	//clo(d);
	R -1;}

I msg_shutdown() {
	free(mbuf);
	R0;
}


I msg_init() {
	mbuf = malloc(50);
	R0;
}

#ifdef RUN_TESTS_MSG

I main() {
	LOG("msg_test");
	rpc_init();
	msg_init();

	C b[256];
	gethostname((S)b,256);
	S hostname = lcase(b,scnt(b));
	struct passwd*u=getpwuid(getuid());
	S username = (S)u->pw_name;

	MSG m = rpc_create(OUT_HEY, scnt(username), username);

	msg_dump_header(m);

	free(m);
	R msg_shutdown();
}

#endif


//:~


