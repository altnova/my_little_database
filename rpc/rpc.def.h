

#define RPC_VER 1

#define MSG_PREF msg
#define MSG_INF_RX _rx_
#define MSG_INF_TX _tx_
#define MSG_PREF_RX rx_
#define MSG_PREF_TX tx_
#define MSGIDTYPE G
#define HDRTYPE G

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

#define mtype(id,label,tx,rx) typedef struct msg_ ## id { MSG_HDR hdr; tx } __attribute__((packed)) ptx_ ## label; typedef ptx_ ## label *tx_ ## label;\
typedef struct msg_1 ## id { MSG_HDR hdr; rx } __attribute__((packed)) prx_ ## label; typedef prx_ ## label *rx_ ## label;

#define msg_ref(label) ptx_##label m_tx_##label; prx_##label m_rx_##label;

#define str(s) #s
#define msg_set_size(label,tx_argc,rx_argc) MSG_SIZES[OUT_##label]=SZ_TX_ ## label; MSG_SIZES[IN_##label]=SZ_RX_ ## label;\
	MSG_ARGC[OUT_##label]=tx_argc; MSG_ARGC[IN_##label]=rx_argc;\
	/* O("msize %s(%d,%d) -> %d,%d\n", str(label), code, code+100, MSG_SIZES[code], MSG_SIZES[code+100]); */

#define msg_has_tail(code,type)   MSG_TAIL_OFFSET[code] = offsetof(type,data_len);

#define msg_set_args_tx(type, args) case OUT_##type: m->msg.m_tx_##type = (ptx_##type){args}; break;
#define msg_set_args_rx(type, args) case  IN_##type: m->msg.m_rx_##type = (prx_##type){args}; break;
#define _a(t) va_arg(a,t)