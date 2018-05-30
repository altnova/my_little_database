

#define RPC_VER 1

#define MSG_PREF msg
#define MSG_INF_RX _rx_
#define MSG_INF_TX _tx_
#define MSG_PREF_RX rx_
#define MSG_PREF_TX tx_
#define MSGIDTYPE G
#define HDRTYPE G

#define INCL_GLOBAL_HEADER ??=include "../___.h"
#define INCL_OWN_HEADER ??=include "rpc.h"

#define INCL_STDLIB ??=include <stdlib.h>
#define INCL_STRING ??=include <string.h>

#define PRAGMA_ONCE ??=pragma once

#define DEFN_SIZETYPE ??=define SIZETYPE
#define DEFN_HDR_SIZE ??=define SZ_MSG_HDR

#define _0() G empty[0];
#define _1(a) a;
#define _2(a,b) a; b;
#define _3(a,b,c) a; b; c;
#define _4(a,b,c,d) a; b; c; d;
#define _5(a,b,c,d,e) a; b; c; d; e;

#define tARR(t,x) SIZETYPE cnt; t x[0]
#define tUI(x) UI x
#define tID(x) ID x
#define tG(x) G x

#define msg_code(code,label) label##_req = code, label##_res = 1 ## code

#define msg_prea_len(code, label) sz SZ_ ## label ## _req = SZ(p##label##_req), SZ_ ## label ## _res = SZ(p ## label ## _res);

#define mtype(id,label,tx,rx) typedef struct msg_ ## id { tx } __attribute__((packed)) p ## label ## _req; /* typedef ptx_ ## label *tx_ ## label; */ \
typedef struct msg_1 ## id { rx } __attribute__((packed)) p ## label ## _res; /* typedef prx_ ## label *rx_ ## label; */

#define msg_ref(label) p##label##_req label##_req; p##label##_res label##_res;

#define str(s) #s
#define msg_set_size(label) MSG_SIZES[label##_req]=SZ_## label ##_req; MSG_SIZES[label##_res]=SZ_## label ##_res;\
	/*O("msize %s -> %d,%d\n", str(label), MSG_SIZES[OUT_##label], MSG_SIZES[IN_##label]);*/

#define msg_has_tail(code,eltype)   MSG_TAIL_OFFSET[code] = offsetof(p##code,cnt); ITEM_SIZE[code]=SZ(eltype);

#define msg_create_fn_w_tail0(fn, t_tail_type, a_tail) \
		MSG rpc_##fn(SIZETYPE a_tail##_cnt, t_tail_type a_tail) {\
		MSG m = rpc_alloc(fn, a_tail##_cnt, (V*)a_tail);\
		m->as.fn = (p##fn){a_tail##_cnt};\
		R m;}

#define msg_create_fn_w_tail1(fn, t1, a1, t_tail_type, a_tail)  \
		MSG rpc_##fn(t1 a1, SIZETYPE a_tail##_cnt, t_tail_type a_tail) {\
		MSG m = rpc_alloc(fn, a_tail##_cnt, (V*)a_tail);\
		m->as.fn = (p##fn){a1, a_tail##_cnt};\
		R m;}

#define msg_create_fn_w_tail2(fn, t1, a1, t2, a2, t_tail_type, a_tail)  \
		MSG rpc_##fn(t1 a1, t2 a2, SIZETYPE a_tail##_cnt, t_tail_type a_tail) {\
		MSG m = rpc_alloc(fn, a_tail##_cnt, (V*)a_tail);\
		m->as.fn = (p##fn){a1, a2, a_tail##_cnt};\
		R m;}

#define msg_create_fn0(fn)\
		MSG rpc_##fn() {\
		MSG m = rpc_alloc(fn, 0, NULL);\
		R m;}

#define msg_create_fn1(fn, t1, a1)  \
		MSG rpc_##fn(t1 a1) {\
		MSG m = rpc_alloc(fn, 0, NULL);\
		m->as.fn = (p##fn){a1};\
		R m;}

#define msg_create_fn2(fn, t1, a1, t2, a2)  \
		MSG rpc_##fn(t1 a1, t2 a2) {\
		MSG m = rpc_alloc(fn, 0, NULL);\
		m->as.fn = (p##fn){a1, a2};\
		R m;}		

#define _arg(t,n) t n
#define _tail(t,n) n##_##cnt, t n

#define msg_create_proto_w_tail0(fn, a_tail) \
		ext MSG rpc_##fn(SIZETYPE a_tail)

#define msg_create_proto_w_tail1(fn, a1, a_tail)  \
		ext MSG rpc_##fn(a1, SIZETYPE a_tail)

#define msg_create_proto_w_tail2(fn, a1, a2, a_tail)  \
		ext MSG rpc_##fn(a1, a2, SIZETYPE a_tail)

#define msg_create_proto0(fn)\
		ext MSG rpc_##fn()

#define msg_create_proto1(fn, a1)  \
		ext MSG rpc_##fn(a1)

#define msg_create_proto2(fn, a1, a2)  \
		ext MSG rpc_##fn(a1, a2)

//:~
