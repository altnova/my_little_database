//! \file ncl.c \brief network client

#include <pwd.h>
#include "___.h"
#include "net/cln.h"
#include "rec.h"
#include "cli.h"

I c=-1; // current connection
I streaming = 0;
S adr = "localhost";
I a = 0;  //< addr
I p = NET_PORT; //<port

I ncl_shutdown();

ZS ncl_username() {
	struct passwd*u=getpwuid(getuid());
	R (S)u->pw_name;
}

ZI ncl_on_connect(I d) {
	LOG("ncl_on_connect");
	T(INFO, "connected to %s:%d",adr,p);
	S u = ncl_username();
	c = d;
	R msg_send(d, rpc_HEY_req(scnt(u)+1, u));
}

ZI ncl_on_stdin(I c, UI l, S str) {
	LOG("stdin_callback");
	str[l-1]=0;//terminate
	I res = cli_dispatch_cmd(str);
	if(res<0)R ncl_shutdown();
	cln_set_prompt(streaming?"":cli_get_prompt());
	R0;}

I ncl_cmd_show(S arg) { // show
	LOG("ncl_cmd_show");
	ID rec_id = cli_parse_id(arg);
	P(rec_id==NIL,1);
	streaming=1;
	msg_send(c, rpc_GET_req(rec_id));
	R0;}

UJ ncl_cmd_edit_send(Rec r) {
	LOG("ncl_cmd_edit_send");
	streaming=1;
	msg_send(c, rpc_UPD_req(1, r));
	R NIL-1;}//< defer ok

I ncl_cmd_edit(S arg) { // edit
	LOG("ncl_cmd_edit");
	cli_enter_edit_mode(CLI_PROMPT_UPDATE, "update", ncl_cmd_edit_send);
	ncl_cmd_show(arg);
	R0;}

UJ ncl_cmd_add_send(Rec r) {
	LOG("ncl_cmd_add_send");
	streaming=1;
	msg_send(c, rpc_ADD_req(1, r));
	R NIL-1;}//< defer ok

I ncl_cmd_add(S arg) { // add
	LOG("ncl_cmd_add");
	cli_cmd_rec_add(arg); // native fn is good enough
	R0;}

I ncl_cmd_del(S arg) { // del
	LOG("ncl_cmd_del");
	ID rec_id = cli_parse_id(arg);
	P(rec_id==NIL,1);
	streaming=1;
	msg_send(c, rpc_DEL_req(rec_id));
	R0;}

I ncl_cmd_list(S arg) { // list
	LOG("ncl_cmd_list");
	cli_recalc_pager(arg);
	pPAGING_INFO p = (pPAGING_INFO){ // page_num, per_page, sort_by, sort_dir
		cli_get_current_page_id()-1,
		cli_get_current_page_size(),
		0, 0};
	streaming=1;
	cli_print_page_head();
	msg_send(c, rpc_LST_req(1,&p));
	R0;}

I ncl_cmd_search(S q) { // query
	LOG("ncl_cmd_search");
	UI len = scnt(q);
	P(len<2,1)
	streaming=1;
	msg_send(c, rpc_FND_req(FTS_MAX_HITS, len+1, q));
	R0;}

I ncl_cmd_import(S arg) { // import
	LOG("ncl_cmd_import");
	T(TEST,"nyi");
R0;}
I ncl_cmd_export(S arg) { // export
	LOG("ncl_cmd_export");
	T(TEST,"nyi");
R0;}
I ncl_cmd_sort(S arg) { // sort
	LOG("ncl_cmd_sort");
	T(TEST,"nyi");
R0;}
I ncl_cmd_debug(S arg) { // debug
	LOG("ncl_cmd_debug");
	T(TEST,"nyi");
R0;}

ZI ncl_on_msg(I d, MSG_HDR *h, pMSG *m) {
	LOG("cln_on_msg");
	if(msg_is_err(h)) {
		pERR_res *e = (pERR_res*)m;
		cli_warn(e->err_id, (S)e->msg);
		streaming=0;
		R1;}	

	SW(h->type){
		CS(SAY_res,;
			pSAY_res *m_say = (pSAY_res*)m;
			T(TEST, "rcvd SAY(%d): %.*s", m_say->cnt, m_say->cnt, m_say->msg);
		)
		CS(HEY_res,;
			pHEY_res *m_hey = (pHEY_res*)m;
			cli_update_db_info((DB_INFO)m_hey->db_info);
			cli_init(); //< will only initialize once
		)
		CS(GET_res,;
			pGET_res *m_get = (pGET_res*)m;
			cli_set_edit_buf((Rec)m_get->record); // for edit cmd
			cli_cmd_rec_show(NULL);
			streaming=0;
			cli_prompt();
		)
		CS(UPD_res,;
			pUPD_res *m_upd = (pUPD_res*)m;
			cli_print_cmd_result(m_upd->rec_id,0);
			streaming=0;
			cli_prompt();
		)
		CS(DEL_res,;
			pDEL_res *m_del = (pDEL_res*)m;
			cli_print_cmd_result(m_del->rec_id,0);
			streaming=0;
		)
		CS(ADD_res,;
			pADD_res *m_add = (pADD_res*)m;
			cli_print_cmd_result(m_add->rec_id,0);
			streaming=0;
		)
		CS(LST_res,;
			pLST_res *m_lst = (pLST_res*)m;
			T(TRACE, "LST rcvd %d records", m_lst->cnt);
			if(m_lst->cnt) {
				DO(m_lst->cnt,
					cli_list_rec_each(&m_lst->records[i], NULL, i))
			} else {
				cli_print_page_tail(); // rcvd stream terminator
				streaming=0;
				cli_prompt();
			}
		)
		CD: msg_send_err(d, ERR_UNKNOWN_MSG_TYPE, "unknown message type");
	}
	R0;}

I ncl_shutdown() {
	cli_shutdown(0);
	cln_shutdown();
	//TBUF(0);
	R0;}

I main(I ac, S*av) {
	LOG("ncl");
	//TBUF(1);

	adr=av[1];
	a=addr(adr);
	p=atoi(av[2]);

	cli_set_hpu(adr,p,ncl_username());
	cli_set_cmd_handler(':', ncl_cmd_show); // show
	cli_set_cmd_handler('*', ncl_cmd_edit); // edit
	cli_set_cmd_handler('+', ncl_cmd_add); // add
	cli_set_cmd_handler('-', ncl_cmd_del); // del
	cli_set_cmd_handler('<', ncl_cmd_import); // import
	cli_set_cmd_handler('>', ncl_cmd_export); // export
	cli_set_cmd_handler('!', ncl_cmd_list); // list
	cli_set_cmd_handler('^', ncl_cmd_sort); // sort
	cli_set_cmd_handler('~', ncl_cmd_debug); // debug
	cli_set_cmd_handler('?', ncl_cmd_search); // search
	cli_set_rec_handlers(ncl_cmd_add_send, NULL, rec_set);
	cln_init();

	msg_set_callback(ncl_on_msg);
	cln_set_stdin_callback(ncl_on_stdin);
	cln_set_on_connect_callback(ncl_on_connect);

	cln_connect(a,p);

	R ncl_shutdown();
}
