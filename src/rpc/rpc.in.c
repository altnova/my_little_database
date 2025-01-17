#include "rpc.def.h"

#ifndef RPC_HEADER

INCL_STDLIB
INCL_STRING

INCL_GLOBAL_HEADER
INCL_OWN_HEADER

G  RPC_VERSION;

UI MSG_SIZES[2*100];
SIZETYPE ITEM_SIZE[2*100];
I  MSG_TAIL_OFFSET[2*100];

S MSG_LABELS[]={"HEY","GET","DEL","UPD","ADD","FND","LST","EXP","BYE"};

msg_prea_len(0,HEY);
msg_prea_len(1,GET);
msg_prea_len(2,DEL);
msg_prea_len(3,UPD);
msg_prea_len(4,ADD);
msg_prea_len(5,FND);
msg_prea_len(6,LST);
msg_prea_len(7,EXP);
msg_prea_len(8,BYE);
msg_prea_len(9,SAY);
msg_prea_len(50,ERR);

msg_create_fn_w_tail0( HEY_req,  S, username);
msg_create_fn_w_tail0( HEY_res, DB_INFO,db_info);
msg_create_fn1(        GET_req, ID, rec_id);
msg_create_fn_w_tail0( GET_res, Rec,record);
msg_create_fn1(        DEL_req, ID, rec_id);
msg_create_fn1(        DEL_res, ID, rec_id);
msg_create_fn_w_tail0( UPD_req, Rec, records);
msg_create_fn1(        UPD_res, ID, rec_id);
msg_create_fn_w_tail0( ADD_req, Rec, records);
msg_create_fn1       ( ADD_res, ID, rec_id);
msg_create_fn_w_tail1( FND_req, UI, max_hits, S, query);
msg_create_fn_w_tail0( FND_res, FTI_MATCH, matches);
msg_create_fn_w_tail0( LST_req, PAGING_INFO, paging);
msg_create_fn_w_tail0( LST_res, Rec, records);
msg_create_fn2(        EXP_req, UI, sort_by, C, sort_dir);
msg_create_fn_w_tail0( EXP_res,  S, csv);
msg_create_fn0(        BYE_req);
msg_create_fn0(        BYE_res);
msg_create_fn_w_tail1( ERR_req, UI, err_id, S, msg);
msg_create_fn_w_tail1( ERR_res, UI, err_id, S, msg);
msg_create_fn_w_tail0( SAY_req,  S, msg);
msg_create_fn_w_tail0( SAY_res,  S, msg);


I rpc_init() {

    RPC_VERSION = RPC_VER;

    DO(100*2, MSG_TAIL_OFFSET[i]=-1)

    msg_set_size(HEY)
    msg_set_size(GET)
    msg_set_size(DEL)
    msg_set_size(UPD)
    msg_set_size(ADD)
    msg_set_size(FND)
    msg_set_size(LST)
    msg_set_size(EXP)
    msg_set_size(BYE)
    msg_set_size(SAY)
    msg_set_size(ERR)

    msg_has_tail(HEY_req, C)
    msg_has_tail(HEY_res, pDB_INFO)
    msg_has_tail(GET_res, pRec)
    msg_has_tail(UPD_req, pRec)
    msg_has_tail(ADD_req, pRec)
    msg_has_tail(FND_req, C)
    msg_has_tail(FND_res, pFTI_MATCH)
    msg_has_tail(LST_req, pPAGING_INFO)
    msg_has_tail(LST_res, pRec)
    msg_has_tail(EXP_res, C)
    msg_has_tail(ERR_req, C)
    msg_has_tail(ERR_res, C)
    msg_has_tail(SAY_req, C)
    msg_has_tail(SAY_res, C)

    R0;
}

G rpc_ver() {
  R RPC_VERSION;
}

V rpc_dump_header(MSG m) {
  LOG("rpc_dump_header");
  MSG_HDR h = m->hdr;
  T(TEST, "ptr -> %p", m);
  T(TEST, "ver -> %d", h.ver);
  T(TEST, "type -> %d", h.type);
//  T(TEST, "type -> %d (%s)", h.type, MSG_LABELS[h.type]);
  T(TEST, "len -> %d", h.len);
  I tail_offset = MSG_TAIL_OFFSET[h.type];
  UI preamble_size = MSG_SIZES[h.type];
  if(tail_offset<0)R;
  T(TEST, "preamble_len -> %d", preamble_size);
  T(TEST, "tail_offset -> %d", tail_offset);
  T(TEST, "tail_cnt -> %d", *(SIZETYPE*)(((V*)&m->as)+tail_offset));
  T(TEST, "tail -> %s", (S)(((V*)&m->as)+tail_offset+SZ(SIZETYPE)));
}

Z MSG rpc_alloc(I m_type, SIZETYPE tail_cnt, V*tail_src) {
    LOG("rpc_alloc");
    UJ m_len = MSG_SIZES[m_type];
    SIZETYPE el_size = ITEM_SIZE[m_type];
    G ver = RPC_VERSION;
    I tail_offset = MSG_TAIL_OFFSET[m_type];
    SIZETYPE tail_len = tail_cnt * el_size;
    MSG m = calloc(1, SZ_MSG_HDR+m_len+tail_len);chk(m,0);
    m->hdr = (MSG_HDR){ver, m_type, m_len+tail_len};
    if(tail_len&&tail_offset>=0) {
      V* tail_dest = ((V*)&m->as)+tail_offset+SZ(SIZETYPE);
      mcpy(tail_dest, tail_src, tail_len);
      //T(TEST, "tail copied, %d bytes at offset %d", tail_len, tail_offset+SZ(SIZETYPE));
    }
    //rpc_dump_header(m);
    R m;
}

SIZETYPE rpc_item_size(I m_type) {
  R ITEM_SIZE[m_type];}

SIZETYPE rpc_tail_offset(I m_type) {
  R MSG_TAIL_OFFSET[m_type];}


#else
//! \file rpc.h \brief rpc api


PRAGMA_ONCE

INCL_FTS_HEADER
/*!
 * message header
 */
DEFN_SIZETYPE UI

typedef struct msg_hdr { G ver, type; SIZETYPE len;} __attribute__((packed)) MSG_HDR;

typedef struct pagination {
  UI page_num, per_page, sort_by;
  C sort_dir;
} __attribute__((packed)) pPAGING_INFO;
typedef pPAGING_INFO *PAGING_INFO;

DEFN_HDR_SIZE SZ(MSG_HDR)

/*!
 * message type definitions
 */
mtype( 0, HEY,     _1(tARR(S,username)),
                   _1(tARR(DB_INFO,db_info)))

mtype( 1, GET,     _1(tID(rec_id)),
                   _1(tARR(pRec,record)))

mtype( 2, DEL,     _1(tID(rec_id)),
                   _1(tID(rec_id)))

mtype( 3, UPD,     _1(tARR(pRec,records)),
                   _1(tID(rec_id)))

mtype( 4, ADD,     _1(tARR(pRec,records)),
                   _1(tID(rec_id)))

mtype( 5, FND,     _2(tUI(max_hits), tARR(S,query)),
                   _1(tARR(pFTI_MATCH,matches)))

mtype( 6, LST,     _1(tARR(pPAGING_INFO,pagination)),
                   _1(tARR(pRec,records)))

mtype( 7, EXP,     _2(tUI(sort_by), tC(sort_dir)),
                   _1(tARR(S,csv)))

mtype( 8, BYE,     _0(),
                   _0())

mtype( 9, SAY,     _1(tARR(S,msg)),
                   _1(tARR(S,msg)))

mtype( 50, ERR,    _2(tUI(err_id), tARR(S,msg)),
                   _2(tUI(err_id), tARR(S,msg)))


/*!
 * message types to int
 */
enum msg_codes {
    msg_code(0,HEY),
    msg_code(1,GET),
    msg_code(2,DEL),
    msg_code(3,UPD),
    msg_code(4,ADD),
    msg_code(5,FND),
    msg_code(6,LST),
    msg_code(7,EXP),
    msg_code(8,BYE),
    msg_code(9,SAY),    
    msg_code(50,ERR)
};
/*!
 * common type
 */
typedef union {
    msg_ref(HEY)
    msg_ref(GET)
    msg_ref(DEL)
    msg_ref(UPD)
    msg_ref(ADD)
    msg_ref(FND)
    msg_ref(LST)
    msg_ref(EXP)
    msg_ref(BYE)
    msg_ref(SAY)    
    msg_ref(ERR)    
} pMSG;

/*!
 * assembled message
 */
typedef struct {
    MSG_HDR hdr;
    pMSG as; 
} *MSG;
/*!
 * message factory
 */
typedef MSG(*RPC_STREAM_FN)(SIZETYPE cnt, V*items);
msg_create_proto_w_tail0( HEY_req, _tail(S,username));
msg_create_proto_w_tail0( HEY_res, _tail(DB_INFO,info));
msg_create_proto1(        GET_req, _arg(ID,rec_id));
msg_create_proto_w_tail0( GET_res, _tail(Rec,record));
msg_create_proto1(        DEL_req, _arg(ID,rec_id));
msg_create_proto1(        DEL_res, _arg(ID,rec_id));
msg_create_proto_w_tail0( UPD_req, _tail(Rec,records));
msg_create_proto1(        UPD_res, _arg(ID,rec_id));
msg_create_proto_w_tail0( ADD_req, _tail(Rec,records));
msg_create_proto1       ( ADD_res, _arg(ID,rec_id));
msg_create_proto_w_tail1( FND_req, _arg(UI,max_hits), _tail(S,query));
msg_create_proto_w_tail0( FND_res, _tail(FTI_MATCH,matches));
msg_create_proto_w_tail0( LST_req, _tail(PAGING_INFO,pagination));
msg_create_proto_w_tail0( LST_res, _tail(Rec,records));
msg_create_proto2(        EXP_req, _arg(UI,sort_by), _arg(C,sort_dir));
msg_create_proto_w_tail0( EXP_res, _tail(S,csv));
msg_create_proto0(        BYE_req);
msg_create_proto0(        BYE_res);
msg_create_proto_w_tail0( SAY_req, _tail(S,msg));
msg_create_proto_w_tail0( SAY_res, _tail(S,msg));
msg_create_proto_w_tail1( ERR_req, _arg(UI,err_id), _tail(S,msg));
msg_create_proto_w_tail1( ERR_res, _arg(UI,err_id), _tail(S,msg));
/*!
 * public methods
 */
ext I rpc_init();
ext G rpc_ver();
ext V rpc_dump_header(MSG m);
ext SIZETYPE rpc_item_size(I m_type);
ext SIZETYPE rpc_tail_offset(I m_type);

Z MSG rpc_alloc(I m_type, SIZETYPE tail_len, V*tail_src);
#endif
//:~
