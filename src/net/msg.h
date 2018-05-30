//! \file msg.h \brief simple data retrieval protocol

#pragma once

#include "../rpc/rpc.h"
#include "tcp.h"

#define MSG_MAX_LEN 1000000

enum msg_err_codes{
	ERR_INVALID_RPC_VERSION = 1,
	ERR_MSG_IS_TOO_BIG = 2,
	ERR_NOT_YET_IMPLEMENTED = 3,
	ERR_UNKNOWN_MSG_TYPE = 4,
	ERR_NO_SUCH_RECORD = 5,
	ERR_NOT_SUPPORTED = 6,
	ERR_CMD_FAILED = 7
};

typedef I(*ON_MSG_CALLBACK)(I d, MSG_HDR *h, pMSG *m);

ext I   msg_init();
ext I   msg_shutdown();
ext I   msg_recv(I d);
ext I   msg_send(I d, MSG m);
ext I   msg_send_err(I d, I err_id, S msg);
ext UI  msg_size(MSG m);
ext V   msg_set_callback(ON_MSG_CALLBACK fn);
ext V   msg_hdr_dump(MSG_HDR *h);

//:~


