#include "rpc.def.h"

#ifndef RPC_HEADER

INCL_STDLIB
INCL_STRING

INCL_GLOBAL_HEADER
INCL_CONFIG_HEADER
INCL_TRACE_HEADER
INCL_OWN_HEADER

G  RPC_VERSION;

UI MSG_SIZES[2*100];
I  MSG_TAIL_OFFSET[2*100];

S MSG_LABELS[]={"HEY","GET","DEL","UPD","ADD","FND","LST","SRT","BYE"};

msg_prea_len(0,HEY);
msg_prea_len(1,GET);
msg_prea_len(2,DEL);
msg_prea_len(3,UPD);
msg_prea_len(4,ADD);
msg_prea_len(5,FND);
msg_prea_len(6,LST);
msg_prea_len(7,SRT);
msg_prea_len(8,BYE);
msg_prea_len(9,SAY);
msg_prea_len(50,ERR);

msg_create_fn_w_tail0( HEY_req,  S, username);
msg_create_fn_w_tail0( HEY_res, UJ*,info);
msg_create_fn1(        GET_req, ID, rec_id);
msg_create_fn_w_tail0( GET_res, Rec,record);
msg_create_fn1(        DEL_req, ID, rec_id);
msg_create_fn1(        DEL_res, ID, rec_id);
msg_create_fn_w_tail1( UPD_req, UI, cnt, Rec, records);
msg_create_fn1(        UPD_res, UI, cnt);
msg_create_fn_w_tail1( ADD_req, UI, cnt, Rec, records);
msg_create_fn1       ( ADD_res, UI, cnt);
msg_create_fn_w_tail1( FND_req, UI, max_hits, S, query);
msg_create_fn_w_tail1( FND_res, UI, cnt, Rec, records);
msg_create_fn2(        LST_req, UI, page_num, UI, per_page);
msg_create_fn_w_tail2( LST_res, UI, page_num, UI, out_of, Rec, records);
msg_create_fn2(        SRT_req, UI, field_id, UI, dir);
msg_create_fn_w_tail2( SRT_res, UI, page_num, UI, out_of, Rec, records);
msg_create_fn0(        BYE_req);
msg_create_fn0(        BYE_res);
msg_create_fn_w_tail1( ERR_req, UI, err_id, S, msg);
msg_create_fn_w_tail1( ERR_res, UI, err_id, S, msg);
msg_create_fn_w_tail0( SAY_req,  S, msg);
msg_create_fn_w_tail0( SAY_res,  S, msg);


V rpc_init() {

    RPC_VERSION = RPC_VER;

    DO(100*2, MSG_TAIL_OFFSET[i]=-1)

    msg_set_size(HEY)
    msg_set_size(GET)
    msg_set_size(DEL)
    msg_set_size(UPD)
    msg_set_size(ADD)
    msg_set_size(FND)
    msg_set_size(LST)
    msg_set_size(SRT)
    msg_set_size(BYE)
    msg_set_size(SAY)
    msg_set_size(ERR)

    msg_has_tail(HEY_req)
    msg_has_tail(HEY_res)
    msg_has_tail(GET_res)
    msg_has_tail(UPD_req)
    msg_has_tail(ADD_req)
    msg_has_tail(FND_req)
    msg_has_tail(FND_res)
    msg_has_tail(LST_res)
    msg_has_tail(SRT_res)
    msg_has_tail(ERR_req)
    msg_has_tail(ERR_res)
    msg_has_tail(SAY_req)
    msg_has_tail(SAY_res)

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
  T(TEST, "tail_len -> %d", *(SIZETYPE*)(((V*)&m->as)+tail_offset));
  T(TEST, "tail -> %s", (S)(((V*)&m->as)+tail_offset+SZ(SIZETYPE)));

}

Z MSG rpc_alloc(I m_type, SIZETYPE tail_len, V*tail_src) {
    LOG("rpc_alloc");
    UJ m_len = MSG_SIZES[m_type];
    G ver = RPC_VERSION;
    I tail_offset = MSG_TAIL_OFFSET[m_type];
    MSG m = calloc(1, m_len+tail_len);chk(m,0);
    m->hdr = (MSG_HDR){ver, m_type, m_len+tail_len};
    if(tail_offset>=0) {
      V* tail_dest = ((V*)&m->as)+tail_offset+SZ(SIZETYPE);
      mcpy(tail_dest, tail_src, tail_len);
      //T(TEST, "tail copied, %d bytes at offset %d", tail_len, tail_offset+SZ(SIZETYPE));
    }
    //rpc_dump_header(m);
    R m;
}


#else
/*!
 * message header
 */
DEFN_SIZETYPE UI

typedef struct msg_hdr { G ver, type; SIZETYPE len;} __attribute__((packed)) MSG_HDR;

DEFN_HDR_SIZE SZ(MSG_HDR)

/*!
 * message type definitions
 */
mtype( 0, HEY,     _1(tARR(S,username)),
                   _1(tARR(UJ*,info)))

mtype( 1, GET,     _1(tID(rec_id)),
                   _1(tARR(pRec,record)))

mtype( 2, DEL,     _1(tID(rec_id)),
                   _1(tID(rec_id)))

mtype( 3, UPD,     _2(tUI(cnt), tARR(pRec,records)),
                   _1(tUI(cnt)))

mtype( 4, ADD,     _2(tUI(cnt), tARR(pRec,records)),
                   _1(tUI(cnt)))

mtype( 5, FND,     _2(tUI(max_hits), tARR(S,query)),
                   _2(tUI(cnt), tARR(pRec,records)))

mtype( 6, LST,     _2(tUI(page_num), tUI(per_page)),
                   _4(tUI(page_num), tUI(out_of), tUI(cnt), tARR(pRec,records)))

mtype( 7, SRT,     _2(tUI(field_id), tUI(dir)),
                   _4(tUI(page_num), tUI(out_of), tUI(cnt), tARR(pRec,records)))

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
    msg_code(7,SRT),
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
    msg_ref(SRT)
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

msg_create_proto_w_tail0( HEY_req, _tail(S,username));
msg_create_proto_w_tail0( HEY_res, _tail(UJ*,info));
msg_create_proto1(        GET_req, _arg(ID,rec_id));
msg_create_proto_w_tail0( GET_res, _tail(Rec,record));
msg_create_proto1(        DEL_req, _arg(ID,rec_id));
msg_create_proto1(        DEL_res, _arg(ID,rec_id));
msg_create_proto_w_tail1( UPD_req, _arg(UI,cnt), _tail(Rec,records));
msg_create_proto1(        UPD_res, _arg(UI,cnt));
msg_create_proto_w_tail1( ADD_req, _arg(UI,cnt), _tail(Rec,records));
msg_create_proto1       ( ADD_res, _arg(UI,cnt));
msg_create_proto_w_tail1( FND_req, _arg(UI,max_hits), _tail(S,query));
msg_create_proto_w_tail1( FND_res, _arg(UI,cnt), _tail(Rec,records));
msg_create_proto2(        LST_req, _arg(UI,page_num), _arg(UI,per_page));
msg_create_proto_w_tail2( LST_res, _arg(UI,page_num), _arg(UI,out_of), _tail(Rec,records));
msg_create_proto2(        SRT_req, _arg(UI,field_id), _arg(UI,dir));
msg_create_proto_w_tail2( SRT_res, _arg(UI,page_num), _arg(UI,out_of), _tail(Rec,records));
msg_create_proto0(        BYE_req);
msg_create_proto0(        BYE_res);
msg_create_proto_w_tail0( SAY_req, _tail(S,msg));
msg_create_proto_w_tail0( SAY_res, _tail(S,msg));
msg_create_proto_w_tail1( ERR_req, _arg(UI,errno), _tail(S,msg));
msg_create_proto_w_tail1( ERR_res, _arg(UI,errno), _tail(S,msg));
/*!
 * public methods
 */
ext V rpc_init();
ext G rpc_ver();
ext V rpc_dump_header(MSG m);

Z MSG rpc_alloc(I m_type, SIZETYPE tail_len, V*tail_src);
#endif
//:~
