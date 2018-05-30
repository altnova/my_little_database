//! \file cli.h \brief command-line interface

#pragma once

//! general config
#define CLI_PROMPT  "  \e[1;32m$\e[0m "
#define CLI_PROMPT_UPDATE "  \e[1;32mupdate$\e[0m "
#define CLI_PROMPT_CREATE "  \e[1;32mcreate$\e[0m "
#define VER "0.9.5"
#define CLI_PAGE_SIZE 30

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

ext I cli_set_cmd_handler(C cmd, CLI_CMD fn);
ext V cli_set_rec_handlers(REC_FN create_fn, REC_FN update_fn, REC_SET_FN set_fn);
ext V cli_set_edit_buf(Rec r);
ext I cli_dispatch_cmd(S cmd);
ext V cli_set_db_info(DB_INFO di);
ext S cli_get_prompt();
ext V cli_prompt();
ext V cli_lf(); //< linefeed
ext V cli_enter_edit_mode(S prompt, S mode, REC_FN rec_fn);
ext ID cli_parse_id(S s);
ext V cli_rec_print(Rec r);
ext V cli_print_del_res(ID rec_id, UJ res);
ext V cli_print_edit_res(ID rec_id, UJ res);
ext V cli_print_editor_head();
ext I cli_cmd_rec_show(S arg);
ext I cli_cmd_rec_add(S arg);
ext V cli_recalc_pager(S page);
ext V cli_print_page_head();
ext V cli_print_page_tail();
ext I cli_get_current_page_id();
ext I cli_get_current_page_size();
ext UJ cli_list_rec_each(Rec r, V*arg, UJ i);

//:~
