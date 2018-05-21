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
#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "tok.h"
#include "usr.h"
#include "clk.h"
#include "rec.h"
#include "str.h"

#define LEFT_OFFSET "  "
#define CLI_PROMPT  "  \e[1;32m$\e[0m "
#define CLI_DB_COMMANDS ":*+-<>"
enum colors {
	C_GREY = 37,
	C_BLUE = 36,
	C_YELL = 33,
	C_GREEN = 32
};
#define VER "0.9.5"
#define MI(i,label) O("\t%d. %s", i, label);	//< menu item
#define NL() O("\n");
#define TB() O(LEFT_OFFSET); /* left offset */
#define CH(c,n) (DO(n, O(c)))
#define HR(n) TB();CH("\u2501",n);NL(); /* horisontal ruler */
#define BLUE(s) O("\e[36m%s\e[0m", s)
#define GREY(s) O("\e[37m%s\e[0m", s)
#define GREY(s) O("\e[37m%s\e[0m", s)
#define YELL(s) O("\e[1;33m%s\e[0m", s)
#define GREEN(s) O("\e[1;32m%s\e[0m", s)
#define COLOR_START_BOLD(col) O("\e[1;%dm", col)
#define COLOR_START(col) O("\e[%dm", col)
#define COLOR_END() O("\e[0m")
#define WIPE(x,n) DO(n, x[i]=0)

typedef I(*CLI_CMD)(S arg);
ZI cli_rec_show(S arg);
ZI cli_rec_edit(S arg);
ZI cli_rec_add(S arg);
ZI cli_rec_del(S arg); 
ZI cli_csv_import(S arg);
ZI cli_csv_export(S arg);
CLI_CMD cmds[] = {cli_rec_show, cli_rec_edit, cli_rec_add, cli_rec_del, cli_csv_import, cli_csv_export};

ZI rows, cols; //< terminal dimensions
ZC fldbuf[FLDMAX];

ZV cli_hint() {
	NL();
	TB();GREY("Enter search terms or commands, ");
	YELL("?");  GREY(" for help, ");
	YELL("\\"); GREY(" to exit");NL();NL();
}

ZV cli_banner() {
	NL();NL();
	HR(53);
	TB();GREEN("Amazon Kindle Database");CH(" ",25);O("v%s\n", VER);
	HR(53);
	NL();
}

ZV cli_help_db() {
	NL();
	TB();GREEN("database commands:\n");
    HR(53);
    TB();YELL(":");BLUE("id  "); O("show");CH(" ",9);
    	 YELL("+");BLUE("    "); O("create");CH(" ",9);
    	 YELL("<");BLUE("i.csv");O("   import");NL();
    TB();YELL("*");BLUE("id  "); O("edit");CH(" ",9);
    	 YELL("-");BLUE("id  "); O("delete");CH(" ",9);
    	 YELL(">");BLUE("o.csv");O("   export");NL();
	NL();
}

ZV cli_usage() {

	cli_banner();

	FTI_INFO fti_info = tok_info();
	TB();GREEN("indexed fields:");CH(" ",17);
	DO(FTI_FIELD_COUNT,
		COLOR_START(C_BLUE);O(" %s", rec_field_names[i]);COLOR_END();
		if(!((I)(i+1)%3)){O("\n");TB();CH(" ",17+15);}
	)
	NL();
	TB();O("total books:%41lu\n", fti_info->total_records);
	TB();O("total words:%41lu\n", fti_info->total_tokens);
	TB();O("total terms:%41lu\n", fti_info->total_terms);
	TB();O("total alloc:%41lu\n", fti_info->total_mem);
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

ZV cli_update_dimensions() {
	struct winsize ws;
	ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws);
	cols = ws.ws_col;
	rows = ws.ws_row;
}

ZV cli_interrupt_handler(I itr) {
    tok_shutdown(); exit(0);}

ZI is_db_cmd(C c) {
	S r=schr(CLI_DB_COMMANDS,c);
	P(!r,-1)
	R r-CLI_DB_COMMANDS;
}

ZI cli_warn(S msg){
	TB();O("%s",msg);NL();
	R1;
}

Z ID cli_parse_id(S str) {
	errno = 0;
	J res = strtol(str, NULL, 10);
	P(errno||res<0, NIL)
	R(ID)res;
}

ZV BOX_START(I w) { TB(); CH("\u250f",1); CH("\u2501",w+2); CH("\u2513",1); NL(); }
ZV BOX_END(I w) { TB(); CH("\u2517",1); CH("\u2501",w+2); CH("\u251b",1); NL(); }
ZV BOX_SPAN(I w) { TB(); CH("\u2523",1); CH("\u2501",w+2); CH("\u252b",1); NL(); }

ZV BOX_LEFT() {TB();O("\u2503 ");}
ZV BOX_RIGHT(I pad) {CH(" ", pad);O("\u2503");}

ZI cli_fld_format(S fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	I len = vsnprintf(fldbuf, FLDMAX, fmt, ap);
	va_end(ap);
	R len;
}

I cli_rec_show(S arg){
	LOG("cli_rec_show");
	P(!scnt(arg), cli_warn("missing record id"))
	ID rec_id = cli_parse_id(arg);
	P(rec_id==NIL, cli_warn("bad record id"))
	Rec r = (Rec)malloc(SZ_REC);chk(r,1);
	P(rec_get(r, rec_id)==NIL, cli_warn("no such record"))

	I width = cols * .7;
	BOX_START(width);

	I len1, gap, line_cnt=0;
	str_wrap(r->title, width/2, {
		BOX_LEFT();COLOR_START_BOLD(C_YELL);
		len1 = O("%.*s", line_len, r->title+line_start);COLOR_END();
		if(!line_cnt++) {
			len1 += cli_fld_format("%s, %hd", r->publisher, r->year);
			gap = width-len1;
			CH(" ", gap);COLOR_START(C_GREY);O("%s", fldbuf);COLOR_END();
		} else {
			gap = width-len1;	
			CH(" ",gap);
		}
		BOX_RIGHT(1);NL();
	})
	BOX_LEFT();
	CH(" ", width);
	BOX_RIGHT(1);NL();

	BOX_LEFT();
	COLOR_START(C_GREY);len1 = O("by ");COLOR_END();
	COLOR_START_BOLD(C_GREY);len1 += O("%s", r->author);COLOR_END();
	I len2 = cli_fld_format("%hd pages", r->pages);
	gap = width-len1-len2;
	CH(" ", gap);COLOR_START(C_GREY);O("%s", fldbuf);COLOR_END();
	//I len = COLOR_START(C_GREY);O("%s, %d", r->publisher, r->year);COLOR_END();
	BOX_RIGHT(1);NL();

	BOX_SPAN(width);

	str_wrap(r->subject, width,
		BOX_LEFT();
		O("%.*s", line_len, r->subject+line_start);
		BOX_RIGHT(ABS(line_len-width)+1);NL();
	)
	BOX_END(width);
	free(r);
	R0;}

ZI cli_rec_edit(S arg){O("nyi cli_rec_edit\n");R0;}
ZI cli_rec_add(S arg){O("nyi cli_rec_add\n");R0;}
ZI cli_rec_del(S arg){O("nyi cli_rec_del\n");R0;}
ZI cli_csv_import(S arg){O("nyi cli_csv_import\n");R0;}
ZI cli_csv_export(S arg){O("nyi cli_csv_export\n");R0;}


I main(I ac, S* av) {
	LOG("cli_main");
	srand(time(NULL));
	signal(SIGINT, cli_interrupt_handler);

	tok_init();
	cli_banner();
	cli_hint();

	C q[LINE_BUF];

	//! start main loop
	USR_LOOP(usr_input_str(q, CLI_PROMPT, "inavalid characters"),
		cli_update_dimensions();

		I qlen = scnt(q);
		if(!qlen){cli_hint();continue;}

		I pos = is_db_cmd(*q);
		if(pos>=0){
			I res = cmds[pos](q+1);
			if(res){TB();O("command error\n");}
			goto NEXT;
		}
		SW(qlen){
			CS(1,
				SW(*q){
					CS('\\',goto EXIT;)
					CS(10, cli_hint()) //< LF
					CS('?', cli_usage())
				}
				goto NEXT;
			)
			CS(2, if(!mcmp("\\\\", q, 2))goto EXIT;)
		}
		if(q[qlen-1]=='?'){
			q[qlen-1]=0;
			tok_print_completions_for(q);
			goto NEXT;
		}
		tok_search(q);
		NEXT:
		WIPE(q,qlen);
	)

	EXIT:
	R tok_shutdown();
}