//! \file msg.c \brief simple data retrieval protocol

#include <pwd.h>

#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "rpc/rpc.h"
#include "msg.h"
#include "tcp.h"
#include "str.h"

UI msg_size(MSG m) {
	R SZ_MSG_HDR + m->hdr.len;
}

V msg_hdr_dump(MSG_HDR *h) {
	LOG("msg_hdr_dump");
	T(TEST, "ver -> %d", h->ver);
	T(TEST, "type -> %d", h->type);
	T(TEST, "len -> %d", h->len);
}

I msg_is_err(MSG_HDR *h) {
	R h->type==50||h->type==150;
}

I msg_is_req(MSG_HDR *h) {
	R h->type<100; //TODO
}

I msg_err(I p, I err_id, S msg) {
	LOG("msg_err");
	T(WARN, "send err (%d) %s", err_id, msg);
	MSG m = rpc_ERR_res((UI)err_id, scnt(msg), msg);
	snd(p, m, msg_size(m));
	R0;
}

I msg_recv(I d) {
	LOG("recv_msg");
	MSG_HDR h; I n=rcv(d,&h,SZ_MSG_HDR);
	P(n<=0,(sd0(d),-1))
	T(TRACE, "rcvd header %d bytes", n);
	//msg_hdr_dump(&h);
	X(h.ver!=rpc_ver(), {msg_err(d, 1, "invalid rpc version");sd0(d);},-1);
	X(h.len > MSG_MAX_LEN, {msg_err(d, 2, "message is too big");sd0(d);},-1);

	pMSG *m = malloc(h.len); chk(m,-1);
	n=rcv(d,m,h.len);
	T(TRACE, "rcvd payload %d bytes", n);

	if(msg_is_err(&h)) {
		pERR_res *e = (pERR_res*)m;
		T(WARN, "recv err (%d) %.*s", e->err_id, 20, e->msg);
	} else {
		T(TRACE, "msg ready to process (type=%d)", h.type);
	}

	if(h.type==SAY_res) {
		pSAY_res *t = (pSAY_res*)m;
		T(TEST, "rcvd SAY(%d): %.*s", t->data_len, t->data_len, t->msg);
	}

	if(h.type==HEY_req) {
		MSG m = rpc_SAY_res(20,"Pleased to meet you.");
		snd(d, m, msg_size(m));
		T(TEST, "sent SAY %d bytes", msg_size(m));
		//msg_hdr_dump(&m->hdr);
	}

	free(m);

	R0;}


I msg_shutdown() {
	R0;
}

I msg_init() {
	R rpc_init();
}

#ifdef RUN_TESTS_MSG

Rec make_rec(UH year, S author, S subject) {
	LOG("make_rec");
	Rec r = (Rec)calloc(1, SZ_REC);chk(r,NULL);
	r->year = year;
	r->pages = 0;
	scpy(r->author, author, scnt(author));
	scpy(r->publisher, "publisher", 9);
	scpy(r->title, "title", 5);
	scpy(r->subject, subject, scnt(subject));
	R r;	
}

V hdr_test(S label, MSG_HDR h, I type, UI len) {
	LOG("hdr_test");
	ASSERT(h.ver==1, label);
	ASSERT(h.type==type, label);
	ASSERT(h.len==len, label);	
}

I main() {
	LOG("msg_test");
	msg_init();

	MSG m;

	//C b[256];
	//gethostname((S)b,256);
	//S hostname = lcase(b,scnt(b));
	struct passwd*u=getpwuid(getuid());
	S username = (S)u->pw_name;

	UJ info[] = {111,222,333};
	ID rec_id = 777;
	UI cnt = 3, max_hits=33, page_num=44,
	per_page=100, out_of=999, field_id=55, dir=321;
	S query = "kelas search!";
	Rec r1 = make_rec(2019, "kelas1", "test1");
	Rec r2 = make_rec(2020, "kelas2", "test2");
	Rec r3 = make_rec(2021, "kelas3", "test3");
	SIZETYPE records_len = cnt*SZ_REC;
	V* records = malloc(records_len);chk(records,1);
	mcpy(records,r1,SZ_REC);
	mcpy(records+SZ_REC,r2,SZ_REC);
	mcpy(records+2*SZ_REC,r3,SZ_REC);
	
	m = rpc_HEY_req(scnt(username), username);	
	pHEY_req *t1 = &m->as.HEY_req;
	hdr_test("t1 header", m->hdr, HEY_req, SZ(SIZETYPE)+scnt(username));
	ASSERT(t1->data_len==scnt(username), "t1 tail_len");
	ASSERT(!scmp(t1->username, username), "t1 tail");
	free(m);

	m = rpc_HEY_res(3 * SZ(UJ), &info[0]);
	hdr_test("t2 header", m->hdr, HEY_res, SZ(SIZETYPE)+3*SZ(UJ));
	pHEY_res *t2 = &m->as.HEY_res;
	ASSERT(t2->data_len==3*SZ(UJ), "t2 tail_len");
	ASSERT(!mcmp(t2->info, info, 3*SZ(UJ)), "t2 tail");
	free(m);

	m = rpc_GET_req(rec_id);
	pGET_req *t3 = &m->as.GET_req;
	hdr_test("t3 header", m->hdr, GET_req, SZ(ID));	
	free(m);

	m = rpc_GET_res(SZ_REC, r1);
	pGET_res *t4 = &m->as.GET_res;
	hdr_test("t4 header", m->hdr, GET_res, SZ(SIZETYPE)+SZ_REC);	
	ASSERT(t4->data_len==SZ_REC, "t4 tail_len");
	ASSERT(!mcmp(t4->record, r1, SZ_REC), "t4 tail");
	free(m);

	m = rpc_DEL_req(rec_id);
	pDEL_req *t5 = &m->as.DEL_req;
	hdr_test("t5 header", m->hdr, DEL_req, SZ(ID));	
	free(m);

	m = rpc_DEL_res(rec_id);
	pDEL_res *t6 = &m->as.DEL_res;
	hdr_test("t6 header", m->hdr, DEL_res, SZ(ID));	
	free(m);

	m = rpc_UPD_req(cnt, records_len, records);
	pUPD_req *t7 = &m->as.UPD_req;
	hdr_test("t7 header", m->hdr, UPD_req, SZ(UI)+SZ(SIZETYPE)+3*SZ_REC);
	//rpc_dump_header(m);
	ASSERT(t7->cnt==cnt, "t7 arg1");
	ASSERT(t7->data_len==cnt*SZ_REC, "t7 tail_len");
	ASSERT(!mcmp(t7->records, records, cnt*SZ_REC), "t7 tail");
	free(m);

	m = rpc_UPD_res(cnt);
	pUPD_res *t8 = &m->as.UPD_res;
	ASSERT(t8->cnt==cnt, "t8 arg1");
	free(m);

	m = rpc_ADD_req(cnt, records_len, records);
	pADD_req *t9 = &m->as.ADD_req;
	ASSERT(t9->cnt==cnt, "t9 arg1");
	ASSERT(t9->data_len==cnt*SZ_REC, "t9 tail_len");
	ASSERT(!mcmp(t9->records, records, cnt*SZ_REC), "t9 tail");
	free(m);

	m = rpc_ADD_res(cnt);
	pADD_res *t10 = &m->as.ADD_res;
	free(m);

	m = rpc_FND_req(max_hits, scnt(query), query);
	pFND_req *t11 = &m->as.FND_req;
	free(m);

	m = rpc_FND_res(cnt, records_len, records);
	pFND_res *t12 = &m->as.FND_res;
	hdr_test("t12 header", m->hdr, FND_res, SZ(UI)+SZ(SIZETYPE)+3*SZ_REC);	
	ASSERT(t12->cnt==cnt, "t12 arg1");
	ASSERT(t12->data_len==cnt*SZ_REC, "t12 tail_len");
	ASSERT(!mcmp(t12->records, records, cnt*SZ_REC), "t12 tail");
	free(m);

	m = rpc_LST_req(page_num, per_page);
	pLST_req *t13 = &m->as.LST_req;
	free(m);

	m = rpc_LST_res(page_num, out_of, records_len, records);
	pLST_res *t14 = &m->as.LST_res;
	free(m);

	m = rpc_SRT_req(field_id, dir);
	pSRT_req *t15 = &m->as.SRT_req;
	free(m);

	m = rpc_SRT_res(page_num, out_of, 2 * SZ_REC, records);
	pSRT_res *t16 = &m->as.SRT_res;
	free(m);

	m = rpc_BYE_req();
	pBYE_req *t17 = &m->as.BYE_req;
	free(m);

	m = rpc_BYE_res();
	pBYE_res *t18 = &m->as.BYE_res;
	hdr_test("t18 header", m->hdr, BYE_res, 0);	
	free(m);

	free(r1);
	free(r2);
	free(r3);
	free(records);

	ASSERT(1, "RPC looks fine")

	R msg_shutdown();
}

#endif


//:~


