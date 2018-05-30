//! \file srv.h \brief tcp server api

#include "msg.h"

typedef I(*RECV_CALLBACK)(I d);

ext I srv_init();
ext V srv_set_stdin_callback(STDIN_CALLBACK fn);
ext I srv_listen(I addr, I port, RECV_CALLBACK fn);
ext V srv_shutdown();