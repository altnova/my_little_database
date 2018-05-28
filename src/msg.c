//! \file msg.c \brief simple data retrieval protocol

#include <pwd.h>

#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "msg.h"
#include "tcp.h"
#include "rpc.h"
#include "str.h"

Z MSG mbuf;

/*
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
	//free(mbuf);
	R0;
}


I msg_init() {
	//mbuf = malloc(50);
	R0;
}

#ifdef RUN_TESTS_MSG

I main() {
	LOG("msg_test");
	rpc_init();
	msg_init();

	C b[256];
	//gethostname((S)b,256);
	//S hostname = lcase(b,scnt(b));
	struct passwd*u=getpwuid(getuid());
	S username = (S)u->pw_name;

	MSG m = rpc_create_HEY_req(scnt(username), username);
	//MSG m = rpc_create_HEY_res(scnt(username), username);

	rpc_dump_header(m);

	free(m);
	//free(u);
	R msg_shutdown();
}

#endif


//:~


