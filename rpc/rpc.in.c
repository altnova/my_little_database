#include "rpc.def.h"

//! message header
typedef struct msg_hdr { G ver, type; UI len;} __attribute__((packed)) MSG_HDR;

Z  G RPC_VERSION;

Z UI MSG_SIZES[2*100];
Z  G MSG_ARGC[2*100];
Z UI MSG_TAIL_OFFSET[2*100];

S MSG_LABELS[]={"HEY","GET","DEL","UPD","ADD","FND","LST","SRT","BYE"};

//! message definitions
mtype( 0, HEY,     _1(tARR(C,username)),
                   _1(tARR(UJ,info)))

mtype( 1, GET,     _1(tID(rec_id)),
                   _1(tARR(pRec,record)))

mtype( 2, DEL,     _1(tID(rec_id)),
                   _1(tID(rec_id)))

mtype( 3, UPD,     _2(tUI(cnt), tARR(pRec,records)),
                   _1(tUI(cnt)))

mtype( 4, ADD,     _2(tUI(cnt), tARR(pRec,records)),
                   _1(tUI(cnt)))

mtype( 5, FND,     _2(tUI(max_hits), tARR(C,query)),
                   _2(tUI(cnt), tARR(pRec,hits)))

mtype( 6, LST,     _2(tUI(page_num), tUI(per_page)),
                   _4(tUI(page_num), tUI(out_of), tUI(cnt), tARR(pRec,records)))

mtype( 7, SRT,     _2(tUI(field_id), tUI(dir)),
                   _4(tUI(page_num), tUI(out_of), tUI(cnt), tARR(pRec,records)))

mtype( 8, BYE,     _1(tUI(msgs_sent)), _1(tUI(msgs_rcvd)))

//! message preamble lenghts
msg_prea_len(0,HEY)
msg_prea_len(1,GET)
msg_prea_len(2,DEL)
msg_prea_len(3,UPD)
msg_prea_len(4,ADD)
msg_prea_len(5,FND)
msg_prea_len(6,LST)
msg_prea_len(7,SRT)
msg_prea_len(8,BYE)

//! message codes
enum msg_codes {
    msg_code(0,HEY),
    msg_code(1,GET),
    msg_code(2,DEL),
    msg_code(3,UPD),
    msg_code(4,ADD),
    msg_code(5,FND),
    msg_code(6,LST),
    msg_code(7,SRT),
    msg_code(8,BYE)
};

typedef union {
    msg_ref(HEY)
    msg_ref(GET)
    msg_ref(DEL)
    msg_ref(UPD)
    msg_ref(ADD)
    msg_ref(FND)
    msg_ref(LST)
    msg_ref(SRT)
    msg_ref(BYE)
} pMSG;

typedef struct {
    MSG_HDR hdr;
    pMSG msg; 
} *MSG;

ext V rpc_init() {

    RPC_VERSION = RPC_VER;

    G TAIL = 2;
    //                arc_out   argc_in
    msg_set_size(HEY, 0+TAIL,   0+TAIL) // UI,UI,{} / UI,UI,{}
    msg_set_size(GET, 1,        0+TAIL) // ID / {}
    msg_set_size(DEL, 1,        1)      // ID / ID
    msg_set_size(UPD, 1+TAIL,   1)      // UI,{} / UI
    msg_set_size(ADD, 1+TAIL,   1)      // UI,{} / UI
    msg_set_size(FND, 1+TAIL,   1+TAIL) // UI,{} / UI,{}
    msg_set_size(LST, 2,        3+TAIL) // UI,UI / UI,UI,UI,{}
    msg_set_size(SRT, 2,        3+TAIL) // UI,UI / UI,UI,UI,{}
    msg_set_size(BYE, 1,        1)      // UI / UI

    msg_has_tail(OUT_HEY,ptx_HEY)
    msg_has_tail( IN_HEY,prx_HEY)
    msg_has_tail( IN_GET,prx_GET)
    msg_has_tail(OUT_UPD,ptx_UPD)
    msg_has_tail(OUT_ADD,ptx_ADD)
    msg_has_tail(OUT_FND,ptx_FND)
    msg_has_tail( IN_FND,prx_FND)
    msg_has_tail( IN_LST,prx_LST)
    msg_has_tail( IN_SRT,prx_SRT)
}

ext MSG rpc_create(I m_type, ...) {
  LOG("rpc_create");
  UI m_len = MSG_SIZES[m_type];
  G ver = RPC_VERSION;
  MSG m = calloc(1, m_len);chk(m,0);
  va_list a;va_start(a, m_type);
  UI tail_offset = MSG_TAIL_OFFSET[m_type];
  m->hdr = (MSG_HDR){ver, m_type, m_len};
  SW(m_type){
    msg_set_args_tx(HEY, {})
    msg_set_args_rx(HEY, {})
    msg_set_args_tx(GET, (_a(ID)))
    msg_set_args_rx(GET, {})
    msg_set_args_tx(DEL, (_a(ID)))
    msg_set_args_rx(DEL, (_a(ID)))
    msg_set_args_tx(UPD, (_a(ID)))
    msg_set_args_rx(UPD, (_a(UI)))
    msg_set_args_tx(ADD, (_a(UI)))
    msg_set_args_rx(ADD, (_a(UI)))
    msg_set_args_tx(FND, (_a(UI)))
    msg_set_args_rx(FND, (_a(UI)))
    msg_set_args_tx(LST, (_a(UI),_a(UI)))
    msg_set_args_rx(LST, (_a(UI),_a(UI),_a(UI)))
    msg_set_args_tx(SRT, (_a(UI),_a(UI)))
    msg_set_args_rx(SRT, (_a(UI),_a(UI),_a(UI)))
    msg_set_args_tx(BYE, (_a(UI)))
    msg_set_args_rx(BYE, (_a(UI)))
    CD: T(WARN, "unknown message code: %d", m_type);
  }
  if(tail_offset) {

    SIZETYPE tail_len = _a(SIZETYPE);
    V* tail_src = _a(V*);
    X(!tail_len, T(WARN, "msg type %d requires data, but no length is given"), 0)
    X(!tail_src, T(WARN, "msg type %d requires data, but no data source is given"), 0)
    m = realloc(m, m_len+SZ(SIZETYPE)+tail_len);chk(m,0);

    SIZETYPE*tl = (SIZETYPE*)(((V*)&m->msg)+tail_offset);
    *tl = tail_len;
    m->hdr.len += tail_len;
    mcpy(((V*)tl)+SZ(SIZETYPE), tail_src, tail_len);
    T(TRACE, "tail copied, %d bytes (expect %d)", m->msg.m_tx_HEY.data_len, tail_len);
  }
  R m;}




