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

#define tARR(t,x) t x[0]
#define tUI(x) UI x
#define tID(x) ID x
#define tSZ(x) SIZETYPE x
#define tG(x) G x

#define msg_code(code,label) msg_tx_ ## label = code, msg_rx_ ## label = 1 ## code

#define msg_prea_len(code, label) sz SZ_TX_ ## label = SZ(ptx_##label), SZ_RX_ ## label = SZ(prx_##label);

#define mtype(id,label,tx,rx) typedef struct msg_ ## id { MSG_HDR hdr; tx } ptx_ ## label; typedef ptx_ ## label *tx_ ## label;\
typedef struct msg_1 ## id { MSG_HDR hdr; rx } prx_ ## label; typedef prx_ ## label *rx_ ## label;

#define msg_ref(label) ptx_##label m_tx_##label; prx_##label m_rx_##label;

#define str(s) #s
#define msg_size_set(code,label,tx_argc,rx_argc) MSG_SIZES[code]=SZ_TX_ ## label; MSG_SIZES[code+100]=SZ_RX_ ## label;\
	MSG_ARGC[code]=tx_argc; MSG_ARGC[code+100]=rx_argc;\
	/* O("msize %s(%d,%d) -> %d,%d\n", str(label), code, code+100, MSG_SIZES[code], MSG_SIZES[code+100]); */

#define msg_set_args(type, args) case msg_##type: m->msg.m_##type = (p##type){args}; break;
#define _a(t) va_arg(a,t)