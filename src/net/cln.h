//! \file cln.h \brief tcp client

#include "msg.h"

typedef I(*ON_CONNECT_CALLBACK)(I c);

ext I cln_init();

ext V cln_set_stdin_callback(STDIN_CALLBACK fn);
ext V cln_set_on_connect_callback(ON_CONNECT_CALLBACK fn);
ext V cln_set_prompt(S p);

ext I cln_connect(I addr, I port);

ext I cln_shutdown();

//:~
