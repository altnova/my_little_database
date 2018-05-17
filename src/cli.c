//! \file cli.c \brief command-line interface

#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "tok.h"
#include "usr.h"
#include "clk.h"

#define VER "0.9.5"
#define MI(i,label) O("\t%d. %s", i, label);
#define NL() O("\n");
#define TB() O("\t");
#define CH(c,n) DO(n, O(c))
#define LN(n) TB();CH("\u2501",n);NL();
#define BLUE(s) O("\e[36m%s\e[0m", s)
#define GREY(s) O("\e[37m%s\e[0m", s)
#define YELL(s) O("\e[1;33m%s\e[0m", s)
#define GREEN(s) O("\e[1;32m%s\e[0m", s)
#define WIPE(x,n) DO(n, x[i]=0)

#define CLI_DB_COMMANDS ":*+-<>"
#define CLI_PROMPT "\t\e[1;32m$\e[0m "

V cli_hint() {
	NL();
	TB();GREY("Enter search terms or commands, ");
	YELL("?");  GREY(" for help, ");
	YELL("\\"); GREY(" to exit");NL();NL();
}

V cli_banner() {
	NL();NL();
	LN(53);
	TB();GREEN("Amazon Kindle Database");CH(" ",25)O("v%s\n", VER);
	LN(53);
	NL();}

V cli_help_db() {
	GREEN("\n\tdatabase commands\n");
    LN(53);
    TB();YELL(":");BLUE("id  "); O("show");CH(" ",9);
    	 YELL("+");BLUE("    "); O("create");CH(" ",9);
    	 YELL("<");BLUE("i.csv");O("   import");NL();
    TB();YELL("*");BLUE("id  "); O("edit");CH(" ",9);
    	 YELL("-");BLUE("id  "); O("delete");CH(" ",9);
    	 YELL(">");BLUE("o.csv");O("   export");NL();
	NL();
}

V cli_usage() {

	cli_banner();

	FTI_INFO fti_info = tok_info();
	TB();GREEN("indexed fields:");CH(" ",17);
	DO(FTI_FIELD_COUNT,
		O("\e[36m %s\e[0m", rec_field_names[i]);
		if(!((I)(i+1)%3)){O("\n");CH(" ",40)}
	);
	NL();
	TB();O("total items:%41lu\n", fti_info->total_records);
	TB();O("total words:%41lu\n", fti_info->total_tokens);
	TB();O("total terms:%41lu\n", fti_info->total_terms);
	TB();O("total alloc:%41lu\n", fti_info->total_mem);
	NL();
	TB();GREEN("search modes:\n");
    LN(53);
	TB();O("fuzzy:");CH(" ", 30);BLUE("war peace tolstoy");NL();
	TB();O("exact:");CH(" ", 32);YELL("\""); BLUE("War and Peace");YELL("\"");NL();
	TB();O("prefix:");CH(" ", 38);BLUE("dostoev");YELL("*");NL();
	TB();O("field:");CH(" ", 33);YELL("title:"); BLUE("algernon");NL();
	TB();O("completions:");CH(" ", 35);BLUE("music"); YELL("?");NL();
	cli_help_db();
	cli_hint();
}

V cli_interrupt_handler(I itr) {
    tok_shutdown(); exit(0);}

ZI is_db_cmd(C c) {
	S r=schr(CLI_DB_COMMANDS,c);
	P(!r,-1)
	R r-CLI_DB_COMMANDS;
}

I main(I av, S* ac) {
	LOG("cli_main");
	srand(time(NULL));
	signal(SIGINT, cli_interrupt_handler);

	tok_init();
	cli_banner();
	cli_hint();

	//! start main loop
	C q[LINE_BUF]; USR_LOOP(usr_input_str(q, CLI_PROMPT, "inavalid characters"),
		I qlen = scnt(q);
		if(!qlen){cli_hint();continue;}

		I pos = is_db_cmd(*q);
		if(pos>=0)O("db command: %c %d\n", *q, pos);

		SW(qlen){
		 CS(1,
			SW(*q){
				CS('\\',goto EXIT;) //< LF
				CS(10, cli_hint()) //< LF
				CS('?', cli_usage())
			}
			WIPE(q,qlen);
			continue)
		 CS(2, if(!mcmp("\\\\", q, 2))goto EXIT)
		}		
		if(q[qlen-1]=='?') {
			q[qlen-1]=0;
			tok_print_completions_for(q);
			WIPE(q,qlen)
			continue;
		}
		DO(qlen,q[i]=0)
		tok_search(q)
	)

	EXIT:
	R tok_shutdown();
}