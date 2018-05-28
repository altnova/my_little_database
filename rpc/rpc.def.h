

#define RPC_VER 1

#define MSG_PREF msg
#define MSG_INF_RX _rx_
#define MSG_INF_TX _tx_
#define MSG_PREF_RX rx_
#define MSG_PREF_TX tx_
#define MSGIDTYPE G
#define HDRTYPE G

#define INCL_OWN_HEADER ??=include "rpc.h"
#define INCL_GLOBAL_HEADER ??=include "___.h"
#define INCL_CONFIG_HEADER ??=include "cfg.h"
#define INCL_TRACE_HEADER ??=include "trc.h"

#define INCL_STDLIB ??=include <stdlib.h>
#define INCL_STRING ??=include <string.h>

#define DEFN_SIZETYPE ??=define SIZETYPE
#define DEFN_HDR_SIZE ??=define SZ_MSG_HDR

#define _0()
#define _1(a) a;
#define _2(a,b) a; b;
#define _3(a,b,c) a; b; c;
#define _4(a,b,c,d) a; b; c; d;
#define _5(a,b,c,d,e) a; b; c; d; e;

#define tARR(t,x) SIZETYPE data_len; t x[0]
#define tUI(x) UI x
#define tID(x) ID x
#define tG(x) G x

#define msg_code(code,label) OUT_ ## label = code, IN_ ## label = 1 ## code

#define msg_prea_len(code, label) sz SZ_TX_ ## label = SZ(ptx_##label), SZ_RX_ ## label = SZ(prx_##label);

#define mtype(id,label,tx,rx) typedef struct msg_ ## id { tx } __attribute__((packed)) ptx_ ## label; typedef ptx_ ## label *tx_ ## label;\
typedef struct msg_1 ## id { rx } __attribute__((packed)) prx_ ## label; typedef prx_ ## label *rx_ ## label;

#define msg_ref(label) ptx_##label m_tx_##label; prx_##label m_rx_##label;

#define str(s) #s
#define msg_set_size(label,tx_argc,rx_argc) MSG_SIZES[OUT_##label]=SZ_TX_ ## label; MSG_SIZES[IN_##label]=SZ_RX_ ## label;\
	MSG_ARGC[OUT_##label]=tx_argc; MSG_ARGC[IN_##label]=rx_argc;\
	/*O("msize %s -> %d,%d\n", str(label), MSG_SIZES[OUT_##label], MSG_SIZES[IN_##label]);*/

#define msg_has_tail(code,type)   MSG_TAIL_OFFSET[code] = offsetof(type,data_len);

#define msg_create_fn_w_tail0(fn, code, type, a_tail_len, t_tail_type, a_tail_ptr) \
		MSG rpc_create_##fn(SIZETYPE a_tail_len, t_tail_type a_tail_ptr) {\
		MSG m = rpc_alloc(code, a_tail_len, (V*)a_tail_ptr);\
		/*m->msg.m_##type = (p##type){};*/\
		R m;}

#define msg_create_fn_w_tail1(fn, code, type, t1, a1, a_tail_len, t_tail_type, a_tail_ptr)  \
		MSG rpc_create_##fn(t1 a1, SIZETYPE a_tail_len, t_tail_type a_tail_ptr) {\
		MSG m = rpc_alloc(code, a_tail_len, (V*)a_tail_ptr);\
		m->msg.m_##type = (p##type){a1};\
		R m;}

#define msg_create_fn_w_tail2(fn, code, type, t1, a1, t2, a2, a_tail_len, t_tail_type, a_tail_ptr)  \
		MSG rpc_create_##fn(t1 a1, t2 a2, SIZETYPE a_tail_len, t_tail_type a_tail_ptr) {\
		MSG m = rpc_alloc(code, a_tail_len, (V*)a_tail_ptr);\
		m->msg.m_##type = (p##type){a1, a2};\
		R m;}

#define msg_create_fn0(fn, code, type)\
		MSG rpc_create_##fn() {\
		MSG m = rpc_alloc(code, 0, NULL);\
		/*m->msg.m_##type = (p##type){};*/\
		R m;}

#define msg_create_fn1(fn, code, type, t1, a1)  \
		MSG rpc_create_##fn(t1 a1) {\
		MSG m = rpc_alloc(code, 0, NULL);\
		m->msg.m_##type = (p##type){a1};\
		R m;}

#define msg_create_fn2(fn, code, type, t1, a1, t2, a2)  \
		MSG rpc_create_##fn(t1 a1, t2 a2) {\
		MSG m = rpc_alloc(code, 9, NULL);\
		m->msg.m_##type = (p##type){a1, a2};\
		R m;}		

#define msg_create_proto_w_tail0(fn, a_tail_len, t_tail_type, a_tail_ptr) \
		ext MSG rpc_create_##fn(SIZETYPE a_tail_len, t_tail_type a_tail_ptr)

#define msg_create_proto_w_tail1(fn, t1, a1, a_tail_len, t_tail_type, a_tail_ptr)  \
		ext MSG rpc_create_##fn(t1 a1, SIZETYPE a_tail_len, t_tail_type a_tail_ptr)

#define msg_create_proto_w_tail2(fn, t1, a1, t2, a2, a_tail_len, t_tail_type, a_tail_ptr)  \
		ext MSG rpc_create_##fn(t1 a1, t2 a2, SIZETYPE a_tail_len, t_tail_type a_tail_ptr)

#define msg_create_proto0(fn)\
		ext MSG rpc_create_##fn()

#define msg_create_proto1(fn, t1, a1)  \
		ext MSG rpc_create_##fn(t1 a1)

#define msg_create_proto2(fn, t1, a1, t2, a2)  \
		ext MSG rpc_create_##fn(t1 a1, t2 a2)


