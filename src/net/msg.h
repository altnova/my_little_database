//! \file msg.h \brief simple data retrieval protocol

#pragma once

#include "../rpc/rpc.h"
#include "tcp.h"

#define MSG_MAX_LEN 1000000

typedef I(*ON_MSG_CALLBACK)(I d, MSG_HDR *h, pMSG *m);

typedef struct {
	MSG hdr; //< stream chunk prefix
	V* chunk;
	UI cnt;
	SIZETYPE el_size;
	UI chunk_size;
	I d; //< socket
} pMSG_STREAM;
typedef pMSG_STREAM *MSG_STREAM;
#define SZ_MSG_STREAM SZ(pMSG_STREAM)

ext I   msg_init();
ext I   msg_shutdown();
ext I   msg_recv(I d);
ext I   msg_send(I d, MSG m);
ext I   msg_send_err(I d, I err_id, S msg);
ext UI  msg_size(MSG m);
ext V   msg_set_callback(ON_MSG_CALLBACK fn);
ext V   msg_hdr_dump(MSG_HDR *h);
ext I   msg_is_err(MSG_HDR *h);
//ext V   msg_stream(V*obj,UI osz,I d,UJ i,RPC_STREAM_FN rpc_fn, C is_last);
ext MSG_STREAM  msg_stream_start(I d,RPC_STREAM_FN rpc_fn, UI chunk_size);
ext UJ msg_stream_send(V* ptrs[], SIZETYPE cnt, MSG_STREAM st);
ext V  msg_stream_end(MSG_STREAM st);

//:~


