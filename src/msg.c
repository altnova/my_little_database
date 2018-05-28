//! \file msg.c \brief simple data retrieval protocol

#include <pwd.h>

#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "msg.h"
#include "tcp.h"
#include "rpc.h"
#include "str.h"

/*
I s_req_hlo(I d, I majver, I minver) {
	LOG("s_req_hlo");
	MSG m = make_msg("REQ","HLO",majver,minver,0,0,0);
	T(TEST, "sending document...");
	I r = snd(d, m, SZ_MSG_HDR + m->size);
	T(TEST, "sent, result= %d",r);
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
	rpc_init();
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
	
	m = rpc_create_HEY_req(scnt(username), username);	
	pHEY_req *t1 = &m->as.HEY_req;
	hdr_test("t1 header", m->hdr, HEY_req, SZ(SIZETYPE)+scnt(username));
	ASSERT(t1->data_len==scnt(username), "t1 tail_len");
	ASSERT(!scmp(t1->username, username), "t1 tail");
	free(m);

	m = rpc_create_HEY_res(3 * SZ(UJ), &info[0]);
	hdr_test("t2 header", m->hdr, HEY_res, SZ(SIZETYPE)+3*SZ(UJ));
	pHEY_res *t2 = &m->as.HEY_res;
	ASSERT(t2->data_len==3*SZ(UJ), "t2 tail_len");
	ASSERT(!mcmp(t2->info, info, 3*SZ(UJ)), "t2 tail");
	free(m);

	m = rpc_create_GET_req(rec_id);
	pGET_req *t3 = &m->as.GET_req;
	hdr_test("t3 header", m->hdr, GET_req, SZ(ID));	
	free(m);

	m = rpc_create_GET_res(SZ_REC, r1);
	pGET_res *t4 = &m->as.GET_res;
	hdr_test("t4 header", m->hdr, GET_res, SZ(SIZETYPE)+SZ_REC);	
	ASSERT(t4->data_len==SZ_REC, "t4 tail_len");
	ASSERT(!mcmp(t4->record, r1, SZ_REC), "t4 tail");
	free(m);

	m = rpc_create_DEL_req(rec_id);
	pDEL_req *t5 = &m->as.DEL_req;
	hdr_test("t5 header", m->hdr, DEL_req, SZ(ID));	
	free(m);

	m = rpc_create_DEL_res(rec_id);
	pDEL_res *t6 = &m->as.DEL_res;
	hdr_test("t6 header", m->hdr, DEL_res, SZ(ID));	
	free(m);

	m = rpc_create_UPD_req(cnt, records_len, records);
	pUPD_req *t7 = &m->as.UPD_req;
	hdr_test("t7 header", m->hdr, UPD_req, SZ(UI)+SZ(SIZETYPE)+3*SZ_REC);
	//rpc_dump_header(m);
	ASSERT(t7->cnt==cnt, "t7 arg1");
	ASSERT(t7->data_len==cnt*SZ_REC, "t7 tail_len");
	ASSERT(!mcmp(t7->records, records, cnt*SZ_REC), "t7 tail");
	free(m);


	m = rpc_create_UPD_res(cnt);
	pUPD_res *t8 = &m->as.UPD_res;
	ASSERT(t8->cnt==cnt, "t8 arg1");
	free(m);

	m = rpc_create_ADD_req(cnt, records_len, records);
	pADD_req *t9 = &m->as.ADD_req;
	ASSERT(t9->cnt==cnt, "t9 arg1");
	ASSERT(t9->data_len==cnt*SZ_REC, "t9 tail_len");
	ASSERT(!mcmp(t9->records, records, cnt*SZ_REC), "t9 tail");

	free(m);

	m = rpc_create_ADD_res(cnt);
	pADD_res *t10 = &m->as.ADD_res;
	free(m);

	m = rpc_create_FND_req(max_hits, scnt(query), query);
	pFND_req *t11 = &m->as.FND_req;
	free(m);

	m = rpc_create_FND_res(cnt, records_len, records);
	pFND_res *t12 = &m->as.FND_res;
	hdr_test("t12 header", m->hdr, FND_res, SZ(UI)+SZ(SIZETYPE)+3*SZ_REC);	
	ASSERT(t12->cnt==cnt, "t12 arg1");
	ASSERT(t12->data_len==cnt*SZ_REC, "t12 tail_len");
	ASSERT(!mcmp(t12->records, records, cnt*SZ_REC), "t12 tail");
	free(m);

	m = rpc_create_LST_req(page_num, per_page);
	pLST_req *t13 = &m->as.LST_req;
	free(m);

	m = rpc_create_LST_res(page_num, out_of, records_len, records);
	pLST_res *t14 = &m->as.LST_res;
	free(m);

	m = rpc_create_SRT_req(field_id, dir);
	pSRT_req *t15 = &m->as.SRT_req;
	free(m);

	m = rpc_create_SRT_res(page_num, out_of, 2 * SZ_REC, records);
	pSRT_res *t16 = &m->as.SRT_res;
	free(m);

	m = rpc_create_BYE_req();
	pBYE_req *t17 = &m->as.BYE_req;
	free(m);

	m = rpc_create_BYE_res();
	pBYE_res *t18 = &m->as.BYE_res;
	hdr_test("t18 header", m->hdr, BYE_res, 0);	
	free(m);

	//rpc_dump_header(m);

	free(r1);
	free(r2);
	free(r3);

	ASSERT(1, "rpc seems to work as expected")

	R msg_shutdown();
}

#endif


//:~


