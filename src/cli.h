//! \file cli.h \brief command-line interface

#pragma once

//! general config
#define CLI_PROMPT  "  \e[37m[%s@%s]\e[0m \e[1;32m$\e[0m "
#define CLI_PROMPT_UPDATE "  \e[37m[%s@%s]\e[0m \e[1;32mupdate$\e[0m "
#define CLI_PROMPT_CREATE "  \e[37m[%s@%s]\e[0m \e[1;32mcreate$\e[0m "
#define VER "0.0.1"
#define CLI_PAGE_SIZE 15

#include "utl/usr.h"
#include "utl/str.h"
#include "rec.h"

#ifdef CLI_STANDALONE
#include "fts.h"
#endif

//! command handler interface
typedef I(*CLI_CMD)(S arg);

ext I cli_init();
ext V cli_shutdown(I itr);

ext V cli_set_hpu(S host,I prt,S user);
ext I cli_set_cmd_handler(C cmd, CLI_CMD fn);
ext V cli_set_rec_handlers(REC_FN create_fn, REC_FN update_fn, REC_SET_FN set_fn);
ext V cli_set_edit_buf(Rec r);
ext I cli_dispatch_cmd(S cmd);
ext V cli_update_db_info(DB_INFO di);
ext S cli_get_prompt();
ext V cli_prompt();
ext I cli_warn(I err_id, S msg);
ext V cli_lf(); //< linefeed
ext V cli_enter_edit_mode(S prompt, S mode, REC_FN rec_fn);
ext ID cli_parse_id(S s);
ext V cli_rec_print(Rec r);
ext V cli_print_cmd_result(ID rec_id, UJ res);
ext V cli_print_editor_head();
ext I cli_cmd_rec_show(S arg);
ext I cli_cmd_rec_add(S arg);
ext V cli_recalc_paging(S page);
ext V cli_print_page_head();
ext V cli_print_page_tail();
ext I cli_get_current_page_id();
ext I cli_get_current_page_size();
ext UJ cli_list_rec_each(Rec r, V*arg, UJ i);

//:~
