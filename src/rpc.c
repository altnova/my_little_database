 #include <stdlib.h>
 #include <string.h>
 #include "___.h"
 #include "cfg.h"
 #include "trc.h"
 #include "rpc.h"
Z G RPC_VERSION;
UI MSG_SIZES[2*100];
Z G MSG_ARGC[2*100];
I MSG_TAIL_OFFSET[2*100];
S MSG_LABELS[]={"HEY","GET","DEL","UPD","ADD","FND","LST","SRT","BYE"};
sz SZ_TX_HEY = SZ(ptx_HEY), SZ_RX_HEY = SZ(prx_HEY);
sz SZ_TX_GET = SZ(ptx_GET), SZ_RX_GET = SZ(prx_GET);
sz SZ_TX_DEL = SZ(ptx_DEL), SZ_RX_DEL = SZ(prx_DEL);
sz SZ_TX_UPD = SZ(ptx_UPD), SZ_RX_UPD = SZ(prx_UPD);
sz SZ_TX_ADD = SZ(ptx_ADD), SZ_RX_ADD = SZ(prx_ADD);
sz SZ_TX_FND = SZ(ptx_FND), SZ_RX_FND = SZ(prx_FND);
sz SZ_TX_LST = SZ(ptx_LST), SZ_RX_LST = SZ(prx_LST);
sz SZ_TX_SRT = SZ(ptx_SRT), SZ_RX_SRT = SZ(prx_SRT);
sz SZ_TX_BYE = SZ(ptx_BYE), SZ_RX_BYE = SZ(prx_BYE);
V rpc_init() {
    RPC_VERSION = 1;
    DO(100*2, MSG_TAIL_OFFSET[i]=-1)
    G TAIL = 2;
    //                arc_out   argc_in
    MSG_SIZES[OUT_HEY]=SZ_TX_HEY; MSG_SIZES[IN_HEY]=SZ_RX_HEY; MSG_ARGC[OUT_HEY]=0+TAIL; MSG_ARGC[IN_HEY]=0+TAIL; // UI,UI,{} / UI,UI,{}
    MSG_SIZES[OUT_GET]=SZ_TX_GET; MSG_SIZES[IN_GET]=SZ_RX_GET; MSG_ARGC[OUT_GET]=1; MSG_ARGC[IN_GET]=0+TAIL; // ID / {}
    MSG_SIZES[OUT_DEL]=SZ_TX_DEL; MSG_SIZES[IN_DEL]=SZ_RX_DEL; MSG_ARGC[OUT_DEL]=1; MSG_ARGC[IN_DEL]=1; // ID / ID
    MSG_SIZES[OUT_UPD]=SZ_TX_UPD; MSG_SIZES[IN_UPD]=SZ_RX_UPD; MSG_ARGC[OUT_UPD]=1+TAIL; MSG_ARGC[IN_UPD]=1; // UI,{} / UI
    MSG_SIZES[OUT_ADD]=SZ_TX_ADD; MSG_SIZES[IN_ADD]=SZ_RX_ADD; MSG_ARGC[OUT_ADD]=1+TAIL; MSG_ARGC[IN_ADD]=1; // UI,{} / UI
    MSG_SIZES[OUT_FND]=SZ_TX_FND; MSG_SIZES[IN_FND]=SZ_RX_FND; MSG_ARGC[OUT_FND]=1+TAIL; MSG_ARGC[IN_FND]=1+TAIL; // UI,{} / UI,{}
    MSG_SIZES[OUT_LST]=SZ_TX_LST; MSG_SIZES[IN_LST]=SZ_RX_LST; MSG_ARGC[OUT_LST]=2; MSG_ARGC[IN_LST]=3+TAIL; // UI,UI / UI,UI,UI,{}
    MSG_SIZES[OUT_SRT]=SZ_TX_SRT; MSG_SIZES[IN_SRT]=SZ_RX_SRT; MSG_ARGC[OUT_SRT]=2; MSG_ARGC[IN_SRT]=3+TAIL; // UI,UI / UI,UI,UI,{}
    MSG_SIZES[OUT_BYE]=SZ_TX_BYE; MSG_SIZES[IN_BYE]=SZ_RX_BYE; MSG_ARGC[OUT_BYE]=1; MSG_ARGC[IN_BYE]=1; // UI / UI
    MSG_TAIL_OFFSET[OUT_HEY] = offsetof(ptx_HEY,data_len);
    MSG_TAIL_OFFSET[IN_HEY] = offsetof(prx_HEY,data_len);
    MSG_TAIL_OFFSET[IN_GET] = offsetof(prx_GET,data_len);
    MSG_TAIL_OFFSET[OUT_UPD] = offsetof(ptx_UPD,data_len);
    MSG_TAIL_OFFSET[OUT_ADD] = offsetof(ptx_ADD,data_len);
    MSG_TAIL_OFFSET[OUT_FND] = offsetof(ptx_FND,data_len);
    MSG_TAIL_OFFSET[IN_FND] = offsetof(prx_FND,data_len);
    MSG_TAIL_OFFSET[IN_LST] = offsetof(prx_LST,data_len);
    MSG_TAIL_OFFSET[IN_SRT] = offsetof(prx_SRT,data_len);
}
Z MSG rpc_alloc(I m_type, SIZETYPE tail_len, V*tail_src) {
    LOG("rpc_alloc");
    UJ m_len = MSG_SIZES[m_type];
    G ver = RPC_VERSION;
    I tail_offset = MSG_TAIL_OFFSET[m_type];
    MSG m = calloc(1, m_len+tail_len);chk(m,0);
    m->hdr = (MSG_HDR){ver, m_type, m_len+tail_len};
    if(tail_offset>=0) {
      SIZETYPE*tl = (SIZETYPE*)(((V*)&m->msg)+tail_offset);
      *tl = tail_len;
      mcpy(((V*)tl)+SZ(SIZETYPE), tail_src, tail_len);
      T(TEST, "tail copied, %d bytes at offset %d", tail_len, tail_offset);
    }
    R m;
}
V rpc_dump_header(MSG m) {
  LOG("msg_dump_header");
  MSG_HDR h = m->hdr;
  T(TEST, "ver -> %d", h.ver);
  T(TEST, "type -> %d (%s)", h.type, MSG_LABELS[h.type]);
  T(TEST, "len -> %d", h.len);
  I tail_offset = MSG_TAIL_OFFSET[h.type];
  UI preamble_size = MSG_SIZES[h.type];
  if(tail_offset<0)R;
  T(TEST, "preamble_len -> %d", preamble_size);
  T(TEST, "tail_offset -> %d", tail_offset);
  T(TEST, "tail_len -> %d", *(SIZETYPE*)(((V*)&m->msg)+tail_offset));
  T(TEST, "tail -> %s", (S)(((V*)&m->msg)+tail_offset+SZ(SIZETYPE)));
}
MSG rpc_create_HEY_req(SIZETYPE username_len, S username) { MSG m = rpc_alloc(OUT_HEY, username_len, (V*)username); R m;};
MSG rpc_create_HEY_res(SIZETYPE info_len, UJ info) { MSG m = rpc_alloc(IN_HEY, info_len, (V*)info); R m;};
MSG rpc_create_GET_req(ID rec_id) { MSG m = rpc_alloc(OUT_GET, 0, NULL); m->msg.m_tx_GET = (ptx_GET){rec_id}; R m;};
MSG rpc_create_GET_res(SIZETYPE record_len, Rec record) { MSG m = rpc_alloc(IN_GET, record_len, (V*)record); R m;};
MSG rpc_create_DEL_req(ID rec_id) { MSG m = rpc_alloc(OUT_DEL, 0, NULL); m->msg.m_tx_DEL = (ptx_DEL){rec_id}; R m;};
MSG rpc_create_DEL_res(ID rec_id) { MSG m = rpc_alloc(IN_DEL, 0, NULL); m->msg.m_rx_DEL = (prx_DEL){rec_id}; R m;};
MSG rpc_create_UPD_req(UI cnt, SIZETYPE records_len, Rec records) { MSG m = rpc_alloc(OUT_UPD, records_len, (V*)records); m->msg.m_tx_UPD = (ptx_UPD){cnt}; R m;};
MSG rpc_create_UPD_res(UI cnt) { MSG m = rpc_alloc(IN_UPD, 0, NULL); m->msg.m_rx_UPD = (prx_UPD){cnt}; R m;};
MSG rpc_create_ADD_req(UI cnt, SIZETYPE records_len, Rec records) { MSG m = rpc_alloc(OUT_ADD, records_len, (V*)records); m->msg.m_tx_ADD = (ptx_ADD){cnt}; R m;};
MSG rpc_create_ADD_res(UI cnt, SIZETYPE records_len, Rec records) { MSG m = rpc_alloc(IN_ADD, records_len, (V*)records); m->msg.m_rx_ADD = (prx_ADD){cnt}; R m;};
MSG rpc_create_FND_req(UI max_hits, SIZETYPE query_len, S query) { MSG m = rpc_alloc(OUT_FND, query_len, (V*)query); m->msg.m_tx_FND = (ptx_FND){max_hits}; R m;};
MSG rpc_create_FND_res(UI cnt, SIZETYPE records_len, Rec records) { MSG m = rpc_alloc(IN_FND, records_len, (V*)records); m->msg.m_rx_FND = (prx_FND){cnt}; R m;};
MSG rpc_create_LST_req(UI page_num, UI per_page) { MSG m = rpc_alloc(OUT_LST, 9, NULL); m->msg.m_tx_LST = (ptx_LST){page_num, per_page}; R m;};
MSG rpc_create_LST_res(UI page_num, UI out_of, SIZETYPE records_len, Rec records) { MSG m = rpc_alloc(IN_LST, records_len, (V*)records); m->msg.m_rx_LST = (prx_LST){page_num, out_of}; R m;};
MSG rpc_create_SRT_req(UI field_id, UI dir) { MSG m = rpc_alloc(OUT_SRT, 9, NULL); m->msg.m_tx_SRT = (ptx_SRT){field_id, dir}; R m;};
MSG rpc_create_SRT_res(UI page_num, UI out_of, SIZETYPE records_len, Rec records) { MSG m = rpc_alloc(IN_SRT, records_len, (V*)records); m->msg.m_rx_SRT = (prx_SRT){page_num, out_of}; R m;};
MSG rpc_create_BYE_req() { MSG m = rpc_alloc(OUT_BYE, 0, NULL); R m;};
MSG rpc_create_BYE_res() { MSG m = rpc_alloc(IN_BYE, 0, NULL); R m;};
//:~
