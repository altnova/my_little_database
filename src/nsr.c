//! \file nsr.c \brief network server

#include "___.h"
#include "net/srv.h"
#include "fts.h"

#define PROMPT "server$ "

ZV nyi(I d) {
	msg_send_err(d, ERR_NOT_YET_IMPLEMENTED, "not yet implemented");}

ZI nsr_on_msg(I d, MSG_HDR *h, pMSG *m) {
	LOG("nsr_on_msg");
	C b[100];UJ res;ID rec_id;
	
	SW(h->type) {
		CS(HEY_req,;
			pHEY_req* m_hey = (pHEY_req*)m;
			I len = sprintf(b, "nice to meet you, %.5s", (S)m_hey->username);
			msg_send(d, rpc_SAY_res(len, (S)b));
			FTI_INFO i = mem_info();
			UJ info[3];
			info[0] = i->total_records;
			info[1] = i->total_tokens;
			info[2] = i->total_mem;
			msg_send(d, rpc_HEY_res(3*SZ(UJ), (UJ*)&info));
		)
    	CS(GET_req,
    		;pGET_req* m_get = (pGET_req*)m;
    		rec_id = m_get->rec_id;
    		Rec r = (Rec)malloc(SZ_REC);chk(r,1);
    		res = rec_get(r, rec_id);
    		X(res==NIL,msg_send_err(d,ERR_NO_SUCH_RECORD,"no such record"),-1);
    		msg_send(d, rpc_GET_res(SZ_REC, r));
    		free(r);
    	)
    	CS(DEL_req,
    		;pDEL_req* m_del = (pDEL_req*)m;
    		rec_id = m_del->rec_id;
    		res = rec_delete(rec_id);
    		X(res==NIL,msg_send_err(d,ERR_NO_SUCH_RECORD,"no such record"),-1);
    		msg_send(d, rpc_DEL_res(rec_id));
    	)
    	CS(UPD_req, ;pUPD_req* m_upd = (pUPD_req*)m; nyi(d);)
    	CS(ADD_req, ;pADD_req* m_add = (pADD_req*)m; nyi(d);)
    	CS(FND_req, ;pFND_req* m_fnd = (pFND_req*)m; nyi(d);)
    	CS(LST_req, ;pLST_req* m_lst = (pLST_req*)m; nyi(d);)
    	CS(SRT_req, ;pSRT_req* m_srt = (pSRT_req*)m; nyi(d);)
    	CS(BYE_req, ;pBYE_req* m_bye = (pBYE_req*)m; nyi(d);)
    	CS(SAY_req, ;pSAY_req* m_say = (pSAY_req*)m; nyi(d);)
    	CS(ERR_req, ;pERR_req* m_err = (pERR_req*)m; nyi(d);)

    	CD: msg_send_err(d, ERR_UNKNOWN_MSG_TYPE, "unknown command");
	}

	R0;	
}

I nsr_stdin(I d, UI l, S str) {
	LOG("nsr_stdin");
	if(l&&!mcmp(str,"s",1)) {
		MSG m = rpc_HEY_req(5,"server");
		snd(d, m, msg_size(m));
		T(TEST, "sent HEY %d bytes", msg_size(m));
		msg_hdr_dump(&m->hdr);
		free(m);}
	write(d, PROMPT, scnt(PROMPT));
	R0;}

V nsr_init() {
	srv_init();
	fts_init();
	msg_set_callback(nsr_on_msg);
	srv_set_stdin_callback(nsr_stdin);
	srv_listen(0, 5000, msg_recv);}

V nsr_shutdown() {
	srv_shutdown();
	fts_shutdown();
}

I main() {
	LOG("nsr");
	signal(SIGINT, nsr_shutdown); //< catch SIGINT and cleanup nicely
	nsr_init();
	nsr_shutdown();}

//:~
