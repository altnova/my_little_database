//! \file cln.c \brief tcp client

#define PORT 5000

#include "../___.h"
#include "cln.h"

#define PROMPT "client$ "

ZI c=0,connected=0;
ZC pt[50];
Z STDIN_CALLBACK stdin_fn;
Z ON_CONNECT_CALLBACK connect_fn;

ZV prompt() {
	write(0, pt, scnt(pt));}

ZI bind_stdin(I d) {
	R tcp_stdin(d, stdin_fn);
}

ZI msg_recv_or_reconnect(I d) {
	I r = msg_recv(d);
	if(r==-1)connected=0,close(c),c=-1;
	R r;}

ZI on_connect() {
	R connect_fn(c);
}

V cln_set_stdin_callback(STDIN_CALLBACK fn) {
	stdin_fn = fn;
	sd1(0,bind_stdin);}

V cln_set_on_connect_callback(ON_CONNECT_CALLBACK fn) {
	connect_fn = fn;
}

V cln_set_prompt(S p) {
	scpy(pt, p, 49);
}

ZI check_conn(I addr, I port) {
	LOG("check_conn");
	C ip[30];ipv4(addr,ip);
	P(connected,0);
	if(c<=0){
		if(errno>0)
			T(INFO, "%s:%d (%d: %s)", ip, port, errno, strerror(errno));
		c = conn(addr,port); 
		R1;
	}else {
		sd1(c, msg_recv_or_reconnect);
		connected = 1;
		on_connect();
		R0;
	}
}

I cln_init() {
	msg_init();
	R tcp_init();
}

I cln_shutdown() {
	msg_shutdown();
	R tcp_shutdown();
}

I cln_connect(I addr, I port) {
	LOG("cln_connect");
	I d; struct timeval tv;
	check_conn(addr, port);
	W(tcp_active_conns()){
		check_conn(addr, port);
		if(poke(),d=tcp_select(tv)>0) {
			tcp_trigger_reads();
			prompt();
		}
	}
	R0;
}

#ifdef RUN_TESTS_CLN

ZV on_connect_test(I c) {
	LOG("on_connect_test");
	T(INFO, "connected to server on port %d", PORT);
	msg_send(c, rpc_HEY_req(4, "cli"));
}

ZI stdin_test_callback(I d, UI l, S str) {
	LOG("stdin_callback");
	if(l&&!mcmp(str,"s",1)) {
		on_connect();
		//T(TEST, "sent HEY %d bytes", msg_size(m));
	}

	if(l&&!mcmp(str,"g",1)) {
		msg_send(c, rpc_GET_req(1));
	}

	if(l&&!mcmp(str,"d",1)) {
		msg_send(c, rpc_DEL_req(1));
	}

	R0;}

ZI cln_on_msg_test(I d, MSG_HDR *h, pMSG *m) {
	LOG("cln_on_msg");
	SW(h->type){
		CS(SAY_res,;
			pSAY_res *m_say = (pSAY_res*)m;
			T(TEST, "rcvd SAY(%d): %.*s", m_say->data_len, m_say->data_len, m_say->msg);
		)
		CS(HEY_res,;
			pHEY_res *m_hey = (pHEY_res*)m;
			//T(TEST, "HEY res: %lu %lu %lu", m_hey->info[0],m_hey->info[1],m_hey->info[2]);
			T(TEST, "HEY res");
		)
		CS(GET_res,;
			pGET_res *m_get = (pGET_res*)m;
			T(TEST, "GET res: %d", m_get->data_len);
		)
		CS(DEL_res,;
			pDEL_res *m_del = (pDEL_res*)m;
			T(TEST, "DEL res: %lu", m_del->rec_id);
		)

		CD: msg_send_err(d, ERR_UNKNOWN_MSG_TYPE, "unknown message type");
	}
	R0;	
}

I main() {
	LOG("cln");
	cln_init();

	msg_set_callback(cln_on_msg_test);
	cln_set_stdin_callback(stdin_test_callback);
	cln_set_on_connect_callback(on_connect_test);
	cln_set_prompt(PROMPT);

	cln_connect(0,PORT);

	R cln_shutdown();
}

#endif

//:~

