//:~
 #include <stdlib.h>
 #include <string.h>
 #include "../___.h"
 #include "rpc.h"
G RPC_VERSION;
UI MSG_SIZES[2*100];
SIZETYPE ITEM_SIZE[2*100];
I MSG_TAIL_OFFSET[2*100];
S MSG_LABELS[]={"HEY","GET","DEL","UPD","ADD","FND","LST","***","BYE"};
sz SZ_HEY_req = SZ(pHEY_req), SZ_HEY_res = SZ(pHEY_res);;
sz SZ_GET_req = SZ(pGET_req), SZ_GET_res = SZ(pGET_res);;
sz SZ_DEL_req = SZ(pDEL_req), SZ_DEL_res = SZ(pDEL_res);;
sz SZ_UPD_req = SZ(pUPD_req), SZ_UPD_res = SZ(pUPD_res);;
sz SZ_ADD_req = SZ(pADD_req), SZ_ADD_res = SZ(pADD_res);;
sz SZ_FND_req = SZ(pFND_req), SZ_FND_res = SZ(pFND_res);;
sz SZ_LST_req = SZ(pLST_req), SZ_LST_res = SZ(pLST_res);;
//msg_prea_len(7,SRT);
sz SZ_BYE_req = SZ(pBYE_req), SZ_BYE_res = SZ(pBYE_res);;
sz SZ_SAY_req = SZ(pSAY_req), SZ_SAY_res = SZ(pSAY_res);;
sz SZ_ERR_req = SZ(pERR_req), SZ_ERR_res = SZ(pERR_res);;
MSG rpc_HEY_req(SIZETYPE username_cnt, S username) { MSG m = rpc_alloc(HEY_req, username_cnt, (V*)username); m->as.HEY_req = (pHEY_req){username_cnt}; R m;};
MSG rpc_HEY_res(SIZETYPE db_info_cnt, DB_INFO db_info) { MSG m = rpc_alloc(HEY_res, db_info_cnt, (V*)db_info); m->as.HEY_res = (pHEY_res){db_info_cnt}; R m;};
MSG rpc_GET_req(ID rec_id) { MSG m = rpc_alloc(GET_req, 0, NULL); m->as.GET_req = (pGET_req){rec_id}; R m;};
MSG rpc_GET_res(SIZETYPE record_cnt, Rec record) { MSG m = rpc_alloc(GET_res, record_cnt, (V*)record); m->as.GET_res = (pGET_res){record_cnt}; R m;};
MSG rpc_DEL_req(ID rec_id) { MSG m = rpc_alloc(DEL_req, 0, NULL); m->as.DEL_req = (pDEL_req){rec_id}; R m;};
MSG rpc_DEL_res(ID rec_id) { MSG m = rpc_alloc(DEL_res, 0, NULL); m->as.DEL_res = (pDEL_res){rec_id}; R m;};
MSG rpc_UPD_req(SIZETYPE records_cnt, Rec records) { MSG m = rpc_alloc(UPD_req, records_cnt, (V*)records); m->as.UPD_req = (pUPD_req){records_cnt}; R m;};
MSG rpc_UPD_res(ID rec_id) { MSG m = rpc_alloc(UPD_res, 0, NULL); m->as.UPD_res = (pUPD_res){rec_id}; R m;};
MSG rpc_ADD_req(SIZETYPE records_cnt, Rec records) { MSG m = rpc_alloc(ADD_req, records_cnt, (V*)records); m->as.ADD_req = (pADD_req){records_cnt}; R m;};
MSG rpc_ADD_res(ID rec_id) { MSG m = rpc_alloc(ADD_res, 0, NULL); m->as.ADD_res = (pADD_res){rec_id}; R m;};
MSG rpc_FND_req(UI max_hits, SIZETYPE query_cnt, S query) { MSG m = rpc_alloc(FND_req, query_cnt, (V*)query); m->as.FND_req = (pFND_req){max_hits, query_cnt}; R m;};
MSG rpc_FND_res(SIZETYPE matches_cnt, FTI_MATCH matches) { MSG m = rpc_alloc(FND_res, matches_cnt, (V*)matches); m->as.FND_res = (pFND_res){matches_cnt}; R m;};
MSG rpc_LST_req(SIZETYPE paging_cnt, PAGING_INFO paging) { MSG m = rpc_alloc(LST_req, paging_cnt, (V*)paging); m->as.LST_req = (pLST_req){paging_cnt}; R m;};
MSG rpc_LST_res(SIZETYPE records_cnt, Rec records) { MSG m = rpc_alloc(LST_res, records_cnt, (V*)records); m->as.LST_res = (pLST_res){records_cnt}; R m;};
//msg_create_fn2(        SRT_req, UI, field_id, UI, dir);
//msg_create_fn_w_tail2( SRT_res, UI, page_num, UI, out_of, Rec, records);
MSG rpc_BYE_req() { MSG m = rpc_alloc(BYE_req, 0, NULL); R m;};
MSG rpc_BYE_res() { MSG m = rpc_alloc(BYE_res, 0, NULL); R m;};
MSG rpc_ERR_req(UI err_id, SIZETYPE msg_cnt, S msg) { MSG m = rpc_alloc(ERR_req, msg_cnt, (V*)msg); m->as.ERR_req = (pERR_req){err_id, msg_cnt}; R m;};
MSG rpc_ERR_res(UI err_id, SIZETYPE msg_cnt, S msg) { MSG m = rpc_alloc(ERR_res, msg_cnt, (V*)msg); m->as.ERR_res = (pERR_res){err_id, msg_cnt}; R m;};
MSG rpc_SAY_req(SIZETYPE msg_cnt, S msg) { MSG m = rpc_alloc(SAY_req, msg_cnt, (V*)msg); m->as.SAY_req = (pSAY_req){msg_cnt}; R m;};
MSG rpc_SAY_res(SIZETYPE msg_cnt, S msg) { MSG m = rpc_alloc(SAY_res, msg_cnt, (V*)msg); m->as.SAY_res = (pSAY_res){msg_cnt}; R m;};
I rpc_init() {
    RPC_VERSION = 1;
    DO(100*2, MSG_TAIL_OFFSET[i]=-1)
    MSG_SIZES[HEY_req]=SZ_HEY_req; MSG_SIZES[HEY_res]=SZ_HEY_res;
    MSG_SIZES[GET_req]=SZ_GET_req; MSG_SIZES[GET_res]=SZ_GET_res;
    MSG_SIZES[DEL_req]=SZ_DEL_req; MSG_SIZES[DEL_res]=SZ_DEL_res;
    MSG_SIZES[UPD_req]=SZ_UPD_req; MSG_SIZES[UPD_res]=SZ_UPD_res;
    MSG_SIZES[ADD_req]=SZ_ADD_req; MSG_SIZES[ADD_res]=SZ_ADD_res;
    MSG_SIZES[FND_req]=SZ_FND_req; MSG_SIZES[FND_res]=SZ_FND_res;
    MSG_SIZES[LST_req]=SZ_LST_req; MSG_SIZES[LST_res]=SZ_LST_res;
    //msg_set_size(SRT)
    MSG_SIZES[BYE_req]=SZ_BYE_req; MSG_SIZES[BYE_res]=SZ_BYE_res;
    MSG_SIZES[SAY_req]=SZ_SAY_req; MSG_SIZES[SAY_res]=SZ_SAY_res;
    MSG_SIZES[ERR_req]=SZ_ERR_req; MSG_SIZES[ERR_res]=SZ_ERR_res;
    MSG_TAIL_OFFSET[HEY_req] = offsetof(pHEY_req,cnt); ITEM_SIZE[HEY_req]=SZ(C);
    MSG_TAIL_OFFSET[HEY_res] = offsetof(pHEY_res,cnt); ITEM_SIZE[HEY_res]=SZ(pDB_INFO);
    MSG_TAIL_OFFSET[GET_res] = offsetof(pGET_res,cnt); ITEM_SIZE[GET_res]=SZ(pRec);
    MSG_TAIL_OFFSET[UPD_req] = offsetof(pUPD_req,cnt); ITEM_SIZE[UPD_req]=SZ(pRec);
    MSG_TAIL_OFFSET[ADD_req] = offsetof(pADD_req,cnt); ITEM_SIZE[ADD_req]=SZ(pRec);
    MSG_TAIL_OFFSET[FND_req] = offsetof(pFND_req,cnt); ITEM_SIZE[FND_req]=SZ(C);
    MSG_TAIL_OFFSET[FND_res] = offsetof(pFND_res,cnt); ITEM_SIZE[FND_res]=SZ(pFTI_MATCH);
    MSG_TAIL_OFFSET[LST_req] = offsetof(pLST_req,cnt); ITEM_SIZE[LST_req]=SZ(pPAGING_INFO);
    MSG_TAIL_OFFSET[LST_res] = offsetof(pLST_res,cnt); ITEM_SIZE[LST_res]=SZ(pRec);
    //msg_has_tail(SRT_res, pRec)
    MSG_TAIL_OFFSET[ERR_req] = offsetof(pERR_req,cnt); ITEM_SIZE[ERR_req]=SZ(C);
    MSG_TAIL_OFFSET[ERR_res] = offsetof(pERR_res,cnt); ITEM_SIZE[ERR_res]=SZ(C);
    MSG_TAIL_OFFSET[SAY_req] = offsetof(pSAY_req,cnt); ITEM_SIZE[SAY_req]=SZ(C);
    MSG_TAIL_OFFSET[SAY_res] = offsetof(pSAY_res,cnt); ITEM_SIZE[SAY_res]=SZ(C);
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
//:~
