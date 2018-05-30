//! \file ncl.c \brief network client

#include <pwd.h>
#include "___.h"
#include "net/cln.h"
#include "rec.h"
#include "cli.h"

#define PORT 5000

I c=-1; // current connection

ZS ncl_username() {
	struct passwd*u=getpwuid(getuid());
	R (S)u->pw_name;
}

ZI ncl_on_connect(I d) {
	LOG("ncl_on_connect");
	T(INFO, "connected to server on port %d", PORT);
	S u = ncl_username();
	c = d;
	R msg_send(d, rpc_HEY_req(scnt(u)+1, u));
}

ZI ncl_on_stdin(I c, UI l, S str) {
	LOG("stdin_callback");
	cli_dispatch_cmd(str);
	cln_set_prompt(cli_get_prompt());
	R0;}

I ncl_cmd_show(S arg) { // show
	LOG("ncl_cmd_show");
	ID rec_id = cli_parse_id(arg);
	P(rec_id==NIL,1);
	msg_send(c, rpc_GET_req(rec_id));
	R0;}

UJ ncl_cmd_edit_send(Rec r) {
	LOG("ncl_cmd_edit_send");
	msg_send(c, rpc_UPD_req(1, SZ_REC, r));
	R0;}
I ncl_cmd_edit(S arg) { // edit
	LOG("ncl_cmd_edit");
	cli_enter_edit_mode(CLI_PROMPT_UPDATE, "update", ncl_cmd_edit_send);
	ncl_cmd_show(arg);
	R0;}

I ncl_cmd_add(S arg) { // add
	LOG("ncl_cmd_add");
	T(TEST,"nyi");
R0;}

I ncl_cmd_del(S arg) { // del
	LOG("ncl_cmd_del");
	ID rec_id = cli_parse_id(arg);
	P(rec_id==NIL,1);
	msg_send(c, rpc_DEL_req(rec_id));
	R0;}

I ncl_cmd_import(S arg) { // import
	LOG("ncl_cmd_import");
	T(TEST,"nyi");
R0;}
I ncl_cmd_export(S arg) { // export
	LOG("ncl_cmd_export");
	T(TEST,"nyi");
R0;}
I ncl_cmd_list(S arg) { // list
	LOG("ncl_cmd_list");
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
I ncl_cmd_search(S arg) { // debug
	LOG("ncl_cmd_search");
	T(TEST,"nyi");
R0;}

ZI ncl_on_msg(I d, MSG_HDR *h, pMSG *m) {
	LOG("cln_on_msg");
	SW(h->type){
		CS(SAY_res,;
			pSAY_res *m_say = (pSAY_res*)m;
			T(TEST, "rcvd SAY(%d): %.*s", m_say->data_len, m_say->data_len, m_say->msg);
		)
		CS(HEY_res,;
			pHEY_res *m_hey = (pHEY_res*)m;
			cli_set_db_info((DB_INFO)m_hey->db_info);
			cli_init();
		)
		CS(GET_res,;
			pGET_res *m_get = (pGET_res*)m;
			cli_set_edit_buf((Rec)m_get->record); // for edit cmd
			cli_cmd_rec_show(NULL);
		)
		CS(UPD_res,;
			pUPD_res *m_upd = (pUPD_res*)m;
			cli_print_edit_res(m_upd->cnt, 0);
		)
		CS(DEL_res,;
			pDEL_res *m_del = (pDEL_res*)m;
			cli_print_del_res(m_del->rec_id,0);
		)

		CD: msg_send_err(d, ERR_UNKNOWN_MSG_TYPE, "unknown message type");
	}
	R0;	}

I main() {
	LOG("ncl");

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
	cli_set_rec_handlers(NULL, NULL, rec_set);
	cln_init();

	msg_set_callback(ncl_on_msg);
	cln_set_stdin_callback(ncl_on_stdin);
	cln_set_on_connect_callback(ncl_on_connect);

	cln_connect(0, PORT);

	cli_shutdown(0);
	R cln_shutdown();
}
