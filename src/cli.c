//! \file cli.c \brief command-line interface

#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <locale.h>

#include "___.h"
#include "cli.h"

//! printing utilities
enum colors { C_GREY = 37, C_BLUE = 36, C_YELL = 33, C_GREEN = 32 };
#define LEFT_OFFSET "  "
#define NL() O("\n");
#define TB() O(LEFT_OFFSET); /* left offset */
#define CH(c,n) (DO(n, O(c)))
#define HR(n) TB();CH("\u2501",n);NL(); /* horisontal ruler */
#define BLUE(s) O("\e[36m%s\e[0m", s)
#define GREY(s) O("\e[37m%s\e[0m", s)
#define YELL(s) O("\e[1;33m%s\e[0m", s)
#define GREEN(s) O("\e[1;32m%s\e[0m", s)
#define COLOR_START_BOLD(col) O("\e[1;%dm", col)
#define COLOR_START(col) O("\e[%dm", col)
#define COLOR_END() O("\e[0m")

//! boxing utilities
ZI BOX_BOLD = 0; //< \see https://en.wikipedia.org/wiki/Box-drawing_character
ZV BOX_LEFT() {TB();O(BOX_BOLD?"\u2503 ":"\u2502 ");}
ZV BOX_RIGHT(I pad) {CH(" ", pad+1);O(BOX_BOLD?"\u2503":"\u2502");}
ZV BOX_START(I w,I*cols, I col_cnt) {
	LOG("BOX_START");
	TB(); CH(BOX_BOLD?"\u250f":"\u250c",1);
	I pos = 0, cnt = 0;
	if(col_cnt>0) {
		DO(col_cnt-1,
			pos = cols[i]+1;
			//T(TEST, "colwidth[%d] = %d", i, pos);
			CH(BOX_BOLD?"\u2501":"\u2500",pos);
			O(BOX_BOLD?"\u2533":"\u252c");
			cnt += pos;)
		CH(BOX_BOLD?"\u2501":"\u2500",cols[col_cnt-1]-2);
	} else {
		CH(BOX_BOLD?"\u2501":"\u2500",w+2);
	}
	CH(BOX_BOLD?"\u2513":"\u2510",1); NL();}
ZV BOX_END(I w,I*cols, I col_cnt) {
	TB(); CH(BOX_BOLD?"\u2517":"\u2514",1);
	I pos = 0, cnt = 0;
	if(col_cnt>0) {
		DO(col_cnt-1,
			pos = cols[i]+1;
			CH(BOX_BOLD?"\u2501":"\u2500",pos);
			O(BOX_BOLD?"\u253b":"\u2534");
			cnt += pos;)
		CH(BOX_BOLD?"\u2501":"\u2500",cols[col_cnt-1]-2);
	} else {
		CH(BOX_BOLD?"\u2501":"\u2500",w+2);
	}
	CH(BOX_BOLD?"\u251b":"\u2518",1); NL();}	
ZV BOX_SPAN(I w) {TB(); CH(BOX_BOLD?"\u2523":"\u251c",1); CH(BOX_BOLD?"\u2501":"\u2500",w+2); CH(BOX_BOLD?"\u252b":"\u2524",1); NL();}
ZV BOX_EMPTY_LINE(I w) {BOX_LEFT();CH(" ", w);BOX_RIGHT(0);NL();}
ZI NUM(I n) {
	I r = 0;
    if(n < 0) {
        r += O("-");
        r += NUM(-n);R r;}
    if(n < 1000) {
        r += O("%41d", n);R r;}
    r += NUM(n/1000);
    r += O(",%03d", n%1000);
    R r;}

#define WIPE(x,n) DO(n, x[i]=0)

//! database commands
I cli_cmd_rec_show(S arg);
ZI cli_cmd_rec_edit(S arg);
I cli_cmd_rec_add(S arg);
ZI cli_cmd_rec_del(S arg); 
ZI cli_cmd_csv_import(S arg);
ZI cli_cmd_csv_export(S arg);
ZI cli_cmd_rec_list(S arg);
ZI cli_cmd_rec_sort(S arg);
ZI cli_cmd_debug(S arg);
//!                        :                 *                 +                -                <                   >                   !                 ^                 ~
Z CLI_CMD cmds[] =        {cli_cmd_rec_show, cli_cmd_rec_edit, cli_cmd_rec_add, cli_cmd_rec_del, cli_cmd_csv_import, cli_cmd_csv_export, cli_cmd_rec_list, cli_cmd_rec_sort, cli_cmd_debug};
#define CLI_DB_COMMANDS ":*+-<>!^~"
Z CLI_CMD cli_cmd_search;

ZI initialized=0;
ZS current_prompt;
ZI rows, cols; //< terminal dimensions
ZC fldbuf[FLDMAX];

//! editing state
ZI       editing = 0;
Z Rec    edit_buf;
Z REC_FN edit_fn;
Z S      edit_mode;

//! rec handlers
Z REC_FN  rec_create_fn;
Z REC_FN  rec_update_fn;
Z REC_SET_FN rec_set_fn;

//! pager state
ZI current_page_id = 0;
ZI current_total_pages = 0;
ZI current_width = 0;
ZI current_column_widths[3];

Z pDB_INFO db_info;

ZV cli_set_prompt(S p){
	current_prompt = p;}

S cli_get_prompt(){
	R current_prompt;}

ZI cli_is_db_cmd(C c) {
	S r=schr(CLI_DB_COMMANDS,c);
	P(!r,-1)
	R r-CLI_DB_COMMANDS;}

I cli_set_cmd_handler(C cmd, CLI_CMD fn) {
	I i = cli_is_db_cmd(cmd);
	if(i<0){cli_cmd_search=fn;R0;}
	cmds[i] = fn;
	R0;}

V cli_set_rec_handlers(REC_FN create_fn, REC_FN update_fn, REC_SET_FN set_fn) {
	rec_create_fn = create_fn;
	rec_update_fn = update_fn;
	rec_set_fn = set_fn;}

ZV cli_hint() {
	NL();
	TB();GREY("Enter search terms or commands, ");
	YELL("?");  GREY(" for help, ");
	YELL("\\"); GREY(" to exit");NL();NL();}

ZV cli_banner() {
	NL();NL();
	HR(53);
	TB();GREEN("Amazon Kindle Database");CH(" ",25);O("v%s\n", VER);
	HR(53);
	NL();}

ZV cli_help_db() {
	NL();
	TB();GREEN("database commands:\n");
    HR(53);
    TB();YELL(":");BLUE("id  "); O("show");CH(" ",9);
    	 YELL("+");BLUE("        "); O("create");CH(" ",9);
    	 //YELL("<");BLUE("i.csv");O("   import");NL();
    	 NL();
    TB();YELL("*");BLUE("id  "); O("edit");CH(" ",9);
    	 YELL("-");BLUE("id      "); O("delete");CH(" ",9);
    	 //YELL(">");BLUE("o.csv");O("   export");NL();
    	 NL();
    TB();YELL("!");BLUE("    "); O("list");CH(" ",9);
    	 YELL("<");BLUE("i.csv");O("   import");
    	 //YELL("^");BLUE("    "); O("resort");CH(" ",9);
    	 //YELL(">");BLUE("o.csv");O("   export");
    	 NL();
    TB();YELL("^");BLUE("    "); O("sort");CH(" ",9);
    	 YELL(">");BLUE("o.csv");O("   export");
		 NL();}

ZV cli_usage() {
	cli_banner();
	TB();GREEN("indexed fields:");CH(" ",17);
	DO(FTI_FIELD_COUNT,
		COLOR_START(C_BLUE);O(" %s", rec_field_names[i]);COLOR_END();
		if(!((I)(i+1)%3)){O("\n");TB();CH(" ",17+15);}
	)
	NL();
	TB();O("total books:%41lu\n", db_info.total_records);
	TB();O("total words:%41lu\n", db_info.total_words);
	TB();O("total alloc:%41lu\n", db_info.total_mem);

	NL();
	TB();GREEN("search modes:\n");
    HR(53);
	TB();O("fuzzy:");CH(" ", 30);BLUE("war peace tolstoy");NL();
	TB();O("exact:");CH(" ", 32);YELL("\""); BLUE("War and Peace");YELL("\"");NL();
	TB();O("field:");CH(" ", 33);YELL("title:"); BLUE("algernon");NL();
	TB();O("prefix:");CH(" ", 38);BLUE("dostoev");YELL("*");NL();
	TB();O("completions:");CH(" ", 35);BLUE("music"); YELL("?");NL();
	cli_help_db();
	cli_hint();
}

ZV cli_update_term_size() {
	struct winsize ws;
	ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws);
	cols = ws.ws_col;
	rows = ws.ws_row;}

ZI cli_warn(S msg){
	TB();O("%s",msg);NL();
	R1;}

ID cli_parse_id(S str) {
	X(!scnt(str), cli_warn("empty record id"), NIL);
	errno = 0;
	J res = strtol(str, NULL, 10);
	X(errno||res<0, cli_warn("bad record id"), NIL);
	R(ID)res;}

ZI cli_fld_format(S fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	I len = vsnprintf(fldbuf, FLDMAX, fmt, ap);
	va_end(ap);
	R len;}

V cli_rec_print(Rec r){
	LOG("cli_rec_print");
	I width = cols * .9;
	I clen, gap, line_cnt=0;
	// start table
	BOX_START(width,NULL,0);
	// rec_id: wrapped title + publisher, year
	str_wrap(r->title, width/2,
		BOX_LEFT();
		COLOR_START_BOLD(C_YELL);
		clen = O("%.*s", line_len, r->title+line_start);COLOR_END();	
		if(!line_cnt++) {
			clen += cli_fld_format("%s, %hd", r->publisher, r->year);
			gap = width-clen;
			CH(" ", gap);COLOR_START(C_GREY);O("%s", fldbuf);COLOR_END();
		} else {
			gap = width-clen;	
			CH(" ",gap);
		}
		BOX_RIGHT(0);NL();
	)
	BOX_EMPTY_LINE(width);
	// author + pagecount
	BOX_LEFT();
	COLOR_START(C_GREY);clen = O("by ");COLOR_END();
	COLOR_START_BOLD(C_GREY);clen += O("%s", r->author);COLOR_END();
	clen += cli_fld_format("%hd pages", r->pages);
	gap = width-clen;
	CH(" ", gap);COLOR_START(C_GREY);O("%s", fldbuf);COLOR_END();
	BOX_RIGHT(0);NL();
	// divider
	BOX_SPAN(width);
	// subject, wrapped to width
	str_wrap(r->subject, width,
		BOX_LEFT();
		//T(TEST, "str_wrap line_start=%lu line_len=%d", line_start, line_len);
		O("%.*s", line_len, r->subject+line_start);
		BOX_RIGHT(width-line_len);NL();
	)
	// terminate table
	BOX_END(width, NULL,0);}

V cli_print_editor_head() {
	cli_rec_print(edit_buf);
	NL();TB();
	DO(FTI_FIELD_COUNT,
		COLOR_START_BOLD(C_YELL);O("%lu: ", i+1);COLOR_END();
		COLOR_START(C_BLUE);O("%s   ", rec_field_names[i]);COLOR_END();)
	NL();TB();

	I pad = 6;
	YELL("field:");BLUE("value");O(" - update field");
	CH(" ", pad);YELL("=");O(" - save changes");
	CH(" ", pad);YELL("\\");O(" - cancel ");O("%s", edit_mode);
	NL();NL();}

V cli_enter_edit_mode(S prompt, S mode, REC_FN rec_fn){
	editing = 1;
	edit_fn = rec_fn;
	edit_mode = mode;
	cli_set_prompt(prompt);}

ZV cli_leave_edit_mode() {
	editing = 0;
	cli_set_prompt(CLI_PROMPT);}

V cli_print_edit_res(ID rec_id, UJ res) {
	if(res==NIL-1)R; // pending server response
	NL();TB();O("record %lu: %s %s", rec_id, edit_mode, (res==NIL)?"fail":"ok");NL();NL();}

ZI cli_parse_cmd_edit(S q) {
	LOG("cli_parse_cmd_edit");
	cli_update_term_size();
	I qlen = scnt(q);
	T(TEST, "qlen=%d, q=%s last=%d", qlen, q, q[qlen-1]);
	if(!qlen||*q==10)R0; //< LF
	//if(qlen>1&&q[qlen-1]==10)q[qlen-1]=0;qlen--; // strip LF
	if(qlen==1&&*q=='='){
		UJ res = edit_fn(edit_buf);
		cli_print_edit_res(edit_buf->rec_id, res);
		cli_leave_edit_mode();
		R1;}
	if(qlen==1&&*q=='\\'){
		NL();TB();O("record %lu: %s %s", edit_buf->rec_id, edit_mode, "canceled");NL();NL();
		cli_leave_edit_mode();
		R1;}
	S colon_pos = schr(q, ':');
	if(!colon_pos)R0;
	*colon_pos='\0';
	I fld_id = atoi(q);
	if(!fld_id)R0;
	fld_id--;
	if(!rec_field_names[fld_id])R0;
	TB();O("%s -> (%s)\n", rec_field_names[fld_id], colon_pos+1);
	H num;

	if(fld_id<2) {
		num = (H)atoi(colon_pos+1);	
		rec_set_fn(edit_buf, fld_id, &num);
	} else
		rec_set_fn(edit_buf, fld_id, colon_pos+1);
	
	R0;}

Z UJ cli_list_rec_each(Rec r, V*arg, UJ i) {
	I width = cols * .9;
	I clen, tlen, gap, line_cnt=0;
	I title_max = width * .7;
	I author_max = width-title_max;
	BOX_LEFT();
	COLOR_START_BOLD(C_GREY);clen = O("%5lu", r->rec_id);COLOR_END();
	BOX_RIGHT(0);clen+=2;
	COLOR_START(C_YELL);clen += tlen = O("%.*s", title_max-3, r->title);COLOR_END();
	if(tlen==(title_max-3)){ //< title truncated, append "..."
		COLOR_START(C_GREY);CH(".", 3); tlen+=3;COLOR_END();clen+=3;}
	CH(" ", title_max-tlen); clen += title_max-tlen;
	BOX_RIGHT(0);clen+=2;
	COLOR_START(C_GREY);clen += O("%.*s", author_max, r->author);COLOR_END();
	gap = width-clen;
	//CH(" ", gap);COLOR_START(C_GREY);O("%s", fldbuf);COLOR_END();
	BOX_RIGHT(gap);NL();
	R0;}

V cli_recalc_pager(S page) {
	UJ page_id;
	UJ total_recs = db_info.total_records;
	current_total_pages = 1+total_recs/CLI_PAGE_SIZE;

	if(scnt(page) < 1)
		page_id = ++current_page_id; // ! - next
	else if(scnt(page)==1&&page[0]=='!') { // !! - prev 
		if(current_page_id>1)
			page_id = --current_page_id;
		else
			page_id = current_total_pages; // wrap to last
	} else {
		page_id = cli_parse_id(page);	// !number - jump
		if(page_id==NIL||page_id<1)
			page_id = current_page_id;; // stay where we are
	}
	if(page_id > current_total_pages)
		page_id = current_total_pages; // do not wrap

	current_page_id = page_id; // save current position

	current_width = cols * .9;
	I title_max = current_width * .7;
	I author_max = current_width-title_max-6;
	//T(TEST, "colwidths: %d %d %d", current_width, title_max, author_max);R1;
	current_column_widths[0] = 6;
	current_column_widths[1] = title_max;
	current_column_widths[2] = author_max;
}

V cli_print_page_head(){
	BOX_START(current_width, (I*)&current_column_widths, 3);
}

V cli_print_page_tail() {
	BOX_END(current_width, (I*)&current_column_widths, 3);
	// print paging help
	TB();O("        [%d/%d]", current_page_id, current_total_pages);
	CH(" ", 3);YELL("!");O(" next");
	CH(" ", 3);YELL("!!");O(" prev");
	CH(" ", 4);YELL("!");BLUE("page");O(" jump");
	NL();NL();}

ZI cli_cmd_rec_sort(S arg){
	LOG("cli_cmd_rec_sort");
	T(TEST, "nyi");
	R0;}

ZI cli_cmd_debug(S arg){
	LOG("cli_cmd_debug");
	I l = scnt(arg);
	T(INFO, "not yet implemented");
	R0;}

I cli_cmd_rec_add(S arg){
	LOG("cli_cmd_rec_add");
	edit_buf->year = 2018;
	edit_buf->pages = 0;
	scpy(edit_buf->author, "author", 6);
	scpy(edit_buf->publisher, "publisher", 9);
	scpy(edit_buf->title, "title", 5);
	scpy(edit_buf->subject, "subject", 7);
	cli_enter_edit_mode(CLI_PROMPT_CREATE, "create", rec_create_fn);
	cli_print_editor_head();
	R0;}

ZI cli_cmd_csv_import(S arg){O("nyi cli_cmd_csv_import\n");R0;}
ZI cli_cmd_csv_export(S arg){O("nyi cli_cmd_csv_export\n");R0;}

ZI cli_parse_cmd_main(S q) {
	LOG("cli_on_stdin_main");
	cli_update_term_size(); //< adjust cols/rows

	I qlen = scnt(q);
	if(!qlen){cli_hint();goto NEXT;}

	if(qlen>1&&q[qlen-1]==10){
		q[qlen-1]=0;qlen--; // strip LF
	}
	//T(TEST, "qlen=%d, q=%s last=%d", qlen, q, q[qlen-1]);

	I pos = cli_is_db_cmd(*q);
	if(pos>=0){
		//T(TEST, "pos=%d", pos);
		I res = cmds[pos](q+1);
		if(res){TB();O("command error\n");}
		goto NEXT;
	}
	SW(qlen){
		CS(1,
			SW(*q){
				CS('\\',R-1;)
				CS(10, cli_hint()) //< LF
				CS('?', cli_usage())
			}
			goto NEXT;
		)
		CS(2, if(!mcmp("\\\\", q, 2))R-1;)
	}
	if(q[qlen-1]=='?'){
		q[qlen-1]=0;
		NL();TB();
		T(INFO, "not yet implemented");
		//fti_print_completions_for(q);
		NL();NL();
		goto NEXT;
	}

	// not a known command, start search
	cli_cmd_search(q);

	NEXT:
	WIPE(q, qlen);
	R0;
}

V cli_set_edit_buf(Rec r) {
	mcpy(edit_buf,r,SZ_REC);
}

V cli_set_db_info(DB_INFO di) {
	mcpy(&db_info,di,SZ_DB_INFO);
}

I cli_dispatch_cmd(S cmd) {
	I r;
	if(editing) {
		r = cli_parse_cmd_edit(cmd);
		if(!r)
			cli_print_editor_head();
		WIPE(cmd, scnt(cmd));
		R0;
	}
	r = cli_parse_cmd_main(cmd);
	WIPE(cmd, scnt(cmd));
	R r;}

V cli_print_del_res(ID rec_id, UJ res) {
	NL();TB();O("record %lu: delete %s", rec_id, (res==NIL)?"failed":"ok");NL();NL();
}

I cli_get_current_page_id() {
	R current_page_id;
}

I cli_get_current_page_size() {
	R CLI_PAGE_SIZE;
}

#ifndef CLI_STANDALONE
I cli_cmd_rec_show(S arg){
	if(!editing)
		cli_rec_print(edit_buf);
	else
		cli_print_editor_head();
	R0;}

ZI cli_cmd_rec_list(S arg){R0;}
ZI cli_cmd_rec_del(S arg){R0;}
ZI cli_cmd_rec_edit(S arg){R0;}

I cli_init() {
	P(initialized,0);
	LOG("cli_init");
	edit_buf = (Rec)calloc(1,SZ_REC);chk(edit_buf,1);
	cli_set_prompt(CLI_PROMPT);
	cli_banner();
	cli_hint();
	initialized=1;
	R0;
}

V cli_shutdown(I itr) {
	free(edit_buf);}

#else

I cli_cmd_rec_show(S arg){
	LOG("cli_cmd_rec_show");
	ID rec_id = cli_parse_id(arg);
	P(rec_id==NIL,1)
	Rec r = (Rec)malloc(SZ_REC);chk(r,1);
	P(rec_get(r, rec_id)==NIL, cli_warn("no such record"))
	cli_rec_print(r);
	free(r);
	R0;}

ZI cli_cmd_rec_list(S arg){
	LOG("cli_cmd_rec_list");
	cli_recalc_pager(arg);
	cli_print_page_head();
	UJ res = idx_page(cli_list_rec_each, NULL, current_page_id-1, CLI_PAGE_SIZE); // read records
	cli_print_page_tail();
	R0;}

ZI cli_cmd_rec_del(S arg){
	LOG("cli_cmd_rec_del");
	ID rec_id = cli_parse_id(arg);
	P(rec_id==NIL,1)
	UJ res = rec_delete(rec_id);
	cli_print_del_res(rec_id, res);
	R0;}

ZI cli_cmd_rec_edit(S arg){
	LOG("cli_cmd_rec_edit");
	ID rec_id = cli_parse_id(arg);
	P(rec_id==NIL,1)
	P(rec_get(edit_buf, rec_id)==NIL, cli_warn("no such record"));
	cli_enter_edit_mode("  update$ ", "update", rec_update_fn);
	cli_print_editor_head();
	R0;}

ZI cli_cmd_search_local(S arg){
	fts_search(arg, (FTI_SEARCH_CALLBACK)NULL); // TODO
	fts_dump_result();
	R0;}

I cli_init() {
	LOG("cli_init");
	edit_buf = (Rec)calloc(1,SZ_REC);chk(edit_buf,1);
	T(TEST,"init complete");
	cli_set_prompt(CLI_PROMPT);
	R0;
}

V cli_shutdown(I itr) {
	free(edit_buf);
	fts_shutdown();
    I res = fti_shutdown();exit(res);}

I main(I ac, S* av) {
	LOG("cli_main");
	srand(time(NULL)); //< random seed
	setlocale(LC_NUMERIC, ""); //< format numbers
	signal(SIGINT, cli_shutdown); //< catch SIGINT and cleanup nicely

	cli_init();

	cli_set_cmd_handler('?', cli_cmd_search_local);
	cli_set_rec_handlers(rec_create, rec_update, rec_set);

	cli_banner();
	cli_hint();

	P(fti_init(),1);
	P(fts_init(),1);

	cli_set_db_info(mem_db_info());

	C q[LINE_BUF];

	//! start main loop
	USR_LOOP(usr_input_str(q, current_prompt, "invalid characters"),
		if(-1==cli_dispatch_cmd(q))break;
	)
	cli_shutdown(0);
}

#endif

//:~
