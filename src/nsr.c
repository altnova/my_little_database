//! \file nsr.c \brief network server

#include "___.h"
#include "net/srv.h"
#include "fts.h"

#define PROMPT "server$ "

ZV nyi(I d) {
	msg_send_err(d, ERR_NOT_YET_IMPLEMENTED, "not yet implemented");}

Z pRec recbuf[4];
Z UI recbufpos = 0;

Z UJ nsr_rec_each(Rec r, V*conn, UJ i) {
	LOG("nsr_rec_each");
	I d = *(I*)conn;
	mcpy(&recbuf[recbufpos++], r, SZ_REC);
	if(recbufpos==4) {
		msg_send(d, rpc_LST_res(4, (Rec)&recbuf));
		T(TRACE, "flush at %lu", i);
		recbufpos=0;
	} else {
		T(TRACE, "record %lu", i);
	}
	R0;
}

ZI nsr_on_msg(I d, MSG_HDR *h, pMSG *m) {
	LOG("nsr_on_msg");
	C b[100];UJ res;ID rec_id;
	
	SW(h->type) {
		CS(HEY_req,;
			pHEY_req* m_hey = (pHEY_req*)m;
			I len = snprintf(b, 100, "nice to meet you, %s", (S)m_hey->username);
			msg_send(d, rpc_SAY_res(len, (S)b));
			UPDATE_METRICS:
			;DB_INFO i = mem_db_info();
			msg_send(d, rpc_HEY_res(1, i));
		)
    	CS(GET_req,
    		;pGET_req* m_get = (pGET_req*)m;
    		rec_id = m_get->rec_id;
    		Rec r = (Rec)malloc(SZ_REC);chk(r,1);
    		res = rec_get(r, rec_id);
    		X(res==NIL,msg_send_err(d,ERR_NO_SUCH_RECORD,"no such record"),-1);
    		msg_send(d, rpc_GET_res(1, r));
    		free(r);
    	)
    	CS(DEL_req,
    		;pDEL_req* m_del = (pDEL_req*)m;
    		rec_id = m_del->rec_id;
    		res = rec_delete(rec_id);
    		X(res==NIL,msg_send_err(d,ERR_NO_SUCH_RECORD,"no such record"),-1);
    		msg_send(d, rpc_DEL_res(rec_id));
    		goto UPDATE_METRICS;
    	)
    	CS(UPD_req,
    		;pUPD_req* m_upd = (pUPD_req*)m;
    		X(m_upd->cnt!=1,msg_send_err(d,ERR_NOT_SUPPORTED,"updating multiple records is not implemented"),-1);
    		res = rec_update(m_upd->records);
    		X(res==NIL,msg_send_err(d,ERR_CMD_FAILED,"update failed"),-1);
    		msg_send(d, rpc_UPD_res(m_upd->cnt));
    	)
    	CS(ADD_req,
    		;pADD_req* m_add = (pADD_req*)m;
    		X(m_add->cnt!=1,msg_send_err(d,ERR_NOT_SUPPORTED,"adding multiple records is not implemented"),-1);
    		res = rec_create(m_add->records);
    		X(res==NIL,msg_send_err(d,ERR_CMD_FAILED,"create failed"),-1);
    		msg_send(d, rpc_ADD_res(m_add->records->rec_id));
    		goto UPDATE_METRICS;
    	)
    	CS(LST_req,
    		;pLST_req* m_lst = (pLST_req*)m;
    		PAGING_INFO p = m_lst->pagination; // page_num, per_page, sort_by, sort_dir
			UJ res = idx_page(nsr_rec_each, &d, p->page_num, p->per_page);
			msg_send(d, rpc_LST_res(recbufpos, (Rec)&recbuf)); // flush buffer remainder
			T(TRACE, "flush remainining %d", recbufpos);
			if(recbufpos)
				msg_send(d, rpc_LST_res(0, NULL)); // send stream terminator
			recbufpos=0;
    	)

    	CS(FND_req, ;pFND_req* m_fnd = (pFND_req*)m; nyi(d);)
    	CS(BYE_req, ;pBYE_req* m_bye = (pBYE_req*)m; nyi(d);)
    	CS(SAY_req, ;pSAY_req* m_say = (pSAY_req*)m; nyi(d);)
    	CS(ERR_req, ;pERR_req* m_err = (pERR_req*)m; nyi(d);)

    	CD: msg_send_err(d, ERR_UNKNOWN_MSG_TYPE, "unknown command");
	}

	R0;	
}

I nsr_stdin(I d, UI l, S str) {
	LOG("nsr_stdin");
	str[l-1]=0;//terminate
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
