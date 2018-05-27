#include "rpc.def.h"

//! message header
typedef struct msg_hdr { G ver, type; UI len;} MSG_HDR;

Z UI MSG_SIZES[2*100];
Z G MSG_ARGC[2*100];

//! message definitions
mtype( 0, HEY,     _2(tUI(ver_hi), tUI(ver_lo)),
                   _2(tUI(ver_hi), tUI(ver_lo)))

mtype( 1, GET,     _1(tID(rec_id)),
                   _3(tID(rec_id), tSZ(el_size), tARR(pRec,record)))

mtype( 2, DEL,     _1(tID(rec_id)),
                   _1(tID(rec_id)))

mtype( 3, UPD,     _4(tID(rec_id), tUI(cnt), tSZ(el_size), tARR(pRec,records)),
                   _1(tUI(cnt)))

mtype( 4, ADD,     _3(tUI(cnt), tSZ(el_size), tARR(pRec,records)),
                   _1(tUI(cnt)))

mtype( 5, FND,     _3(tUI(max_hits), tSZ(query_len), tARR(C,query)),
                   _3(tUI(cnt), tSZ(el_size), tARR(pRec,hits)))

mtype( 6, LST,     _2(tUI(page_num), tUI(per_page)),
                   _5(tUI(page_num), tUI(out_of), tUI(cnt), tSZ(el_size), tARR(pRec,records)))

mtype( 7, SRT,     _2(tUI(field_id), tUI(dir)),
                   _5(tUI(page_num), tUI(out_of), tUI(cnt), tSZ(el_size), tARR(pRec,records)))

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
    msg_size_set(0,HEY,2,2) // UI,UI / UI/UI
    msg_size_set(1,GET,1,3) // ID / ID,SZ,{}
    msg_size_set(2,DEL,1,1) // ID / ID
    msg_size_set(3,UPD,4,1) // ID,UI,SZ,{} / UI
    msg_size_set(4,ADD,3,1) // UI,SZ,{} / UI
    msg_size_set(5,FND,3,3) // UI,SZ,{} / UI,SZ,{}
    msg_size_set(6,LST,2,5) // UI,UI / UI,UI,UI,SZ,{}
    msg_size_set(7,SRT,2,5) // UI,UI / UI,UI,UI,SZ,{}
    msg_size_set(8,BYE,1,1) // UI / UI
}

ext MSG rpc_make(I m_type, ...) {
  UI m_len = MSG_SIZES[m_type];
  G ver = 42;
  MSG m = calloc(1, m_len);
  va_list a;va_start(a, m_type);
  //I payload_pos = 0, payload_sz = 0;
  m->hdr = (MSG_HDR){ver, m_type, m_len};
  SW(m_type){
    msg_set_args(tx_HEY, (_a(UI),_a(UI)))
    msg_set_args(rx_HEY, (_a(UI),_a(UI)))
    msg_set_args(tx_GET, (_a(ID)))
    msg_set_args(rx_GET, (_a(ID),_a(SIZETYPE)))
    msg_set_args(tx_DEL, (_a(ID)))
    msg_set_args(rx_DEL, (_a(ID)))
    msg_set_args(tx_UPD, (_a(ID),_a(SIZETYPE),_a(SIZETYPE)))
    msg_set_args(rx_UPD, (_a(UI)))
    msg_set_args(tx_ADD, (_a(UI),_a(SIZETYPE)))
    msg_set_args(rx_ADD, (_a(UI)))
    msg_set_args(tx_FND, (_a(UI),_a(SIZETYPE)))
    msg_set_args(rx_FND, (_a(UI),_a(SIZETYPE)))
    msg_set_args(tx_LST, (_a(UI),_a(UI)))
    msg_set_args(rx_LST, (_a(UI),_a(UI),_a(UI),_a(SIZETYPE)))
    msg_set_args(tx_SRT, (_a(UI),_a(UI)))
    msg_set_args(rx_SRT, (_a(UI),_a(UI),_a(UI),_a(SIZETYPE)))
    msg_set_args(tx_BYE, (_a(UI)))
    msg_set_args(rx_BYE, (_a(UI)))
  }
  //if(size>0) mcpy(m, m+SZ_MSG_HDR, size);
  R m;}




