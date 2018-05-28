//! message header
typedef struct msg_hdr { G ver, type; UI len;} __attribute__((packed)) MSG_HDR;
Z G RPC_VERSION;
Z UI MSG_SIZES[2*100];
Z G MSG_ARGC[2*100];
Z UI MSG_TAIL_OFFSET[2*100];
S MSG_LABELS[]={"HEY","GET","DEL","UPD","ADD","FND","LST","SRT","BYE"};
//! message definitions
typedef struct msg_0 { MSG_HDR hdr; SIZETYPE data_len; C username[0]; } __attribute__((packed)) ptx_HEY; typedef ptx_HEY *tx_HEY;typedef struct msg_10 { MSG_HDR hdr; SIZETYPE data_len; UJ info[0]; } __attribute__((packed)) prx_HEY; typedef prx_HEY *rx_HEY;
typedef struct msg_1 { MSG_HDR hdr; ID rec_id; } __attribute__((packed)) ptx_GET; typedef ptx_GET *tx_GET;typedef struct msg_11 { MSG_HDR hdr; SIZETYPE data_len; pRec record[0]; } __attribute__((packed)) prx_GET; typedef prx_GET *rx_GET;
typedef struct msg_2 { MSG_HDR hdr; ID rec_id; } __attribute__((packed)) ptx_DEL; typedef ptx_DEL *tx_DEL;typedef struct msg_12 { MSG_HDR hdr; ID rec_id; } __attribute__((packed)) prx_DEL; typedef prx_DEL *rx_DEL;
typedef struct msg_3 { MSG_HDR hdr; UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) ptx_UPD; typedef ptx_UPD *tx_UPD;typedef struct msg_13 { MSG_HDR hdr; UI cnt; } __attribute__((packed)) prx_UPD; typedef prx_UPD *rx_UPD;
typedef struct msg_4 { MSG_HDR hdr; UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) ptx_ADD; typedef ptx_ADD *tx_ADD;typedef struct msg_14 { MSG_HDR hdr; UI cnt; } __attribute__((packed)) prx_ADD; typedef prx_ADD *rx_ADD;
typedef struct msg_5 { MSG_HDR hdr; UI max_hits; SIZETYPE data_len; C query[0]; } __attribute__((packed)) ptx_FND; typedef ptx_FND *tx_FND;typedef struct msg_15 { MSG_HDR hdr; UI cnt; SIZETYPE data_len; pRec hits[0]; } __attribute__((packed)) prx_FND; typedef prx_FND *rx_FND;
typedef struct msg_6 { MSG_HDR hdr; UI page_num; UI per_page; } __attribute__((packed)) ptx_LST; typedef ptx_LST *tx_LST;typedef struct msg_16 { MSG_HDR hdr; UI page_num; UI out_of; UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) prx_LST; typedef prx_LST *rx_LST;
typedef struct msg_7 { MSG_HDR hdr; UI field_id; UI dir; } __attribute__((packed)) ptx_SRT; typedef ptx_SRT *tx_SRT;typedef struct msg_17 { MSG_HDR hdr; UI page_num; UI out_of; UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) prx_SRT; typedef prx_SRT *rx_SRT;
typedef struct msg_8 { MSG_HDR hdr; UI msgs_sent; } __attribute__((packed)) ptx_BYE; typedef ptx_BYE *tx_BYE;typedef struct msg_18 { MSG_HDR hdr; UI msgs_rcvd; } __attribute__((packed)) prx_BYE; typedef prx_BYE *rx_BYE;
//! message preamble lenghts
sz SZ_TX_HEY = SZ(ptx_HEY), SZ_RX_HEY = SZ(prx_HEY);
sz SZ_TX_GET = SZ(ptx_GET), SZ_RX_GET = SZ(prx_GET);
sz SZ_TX_DEL = SZ(ptx_DEL), SZ_RX_DEL = SZ(prx_DEL);
sz SZ_TX_UPD = SZ(ptx_UPD), SZ_RX_UPD = SZ(prx_UPD);
sz SZ_TX_ADD = SZ(ptx_ADD), SZ_RX_ADD = SZ(prx_ADD);
sz SZ_TX_FND = SZ(ptx_FND), SZ_RX_FND = SZ(prx_FND);
sz SZ_TX_LST = SZ(ptx_LST), SZ_RX_LST = SZ(prx_LST);
sz SZ_TX_SRT = SZ(ptx_SRT), SZ_RX_SRT = SZ(prx_SRT);
sz SZ_TX_BYE = SZ(ptx_BYE), SZ_RX_BYE = SZ(prx_BYE);
//! message codes
enum msg_codes {
    OUT_HEY = 0, IN_HEY = 10,
    OUT_GET = 1, IN_GET = 11,
    OUT_DEL = 2, IN_DEL = 12,
    OUT_UPD = 3, IN_UPD = 13,
    OUT_ADD = 4, IN_ADD = 14,
    OUT_FND = 5, IN_FND = 15,
    OUT_LST = 6, IN_LST = 16,
    OUT_SRT = 7, IN_SRT = 17,
    OUT_BYE = 8, IN_BYE = 18
};
typedef union {
    ptx_HEY m_tx_HEY; prx_HEY m_rx_HEY;
    ptx_GET m_tx_GET; prx_GET m_rx_GET;
    ptx_DEL m_tx_DEL; prx_DEL m_rx_DEL;
    ptx_UPD m_tx_UPD; prx_UPD m_rx_UPD;
    ptx_ADD m_tx_ADD; prx_ADD m_rx_ADD;
    ptx_FND m_tx_FND; prx_FND m_rx_FND;
    ptx_LST m_tx_LST; prx_LST m_rx_LST;
    ptx_SRT m_tx_SRT; prx_SRT m_rx_SRT;
    ptx_BYE m_tx_BYE; prx_BYE m_rx_BYE;
} pMSG;
typedef struct {
    MSG_HDR hdr;
    pMSG msg;
} *MSG;
ext V rpc_init() {
    RPC_VERSION = 1;
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
ext MSG rpc_create(I m_type, ...) {
  LOG("rpc_create");
  UI m_len = MSG_SIZES[m_type];
  G ver = RPC_VERSION;
  MSG m = calloc(1, m_len);chk(m,0);
  va_list a;va_start(a, m_type);
  UI tail_offset = MSG_TAIL_OFFSET[m_type];
  m->hdr = (MSG_HDR){ver, m_type, m_len};
  SW(m_type){
    case OUT_HEY: m->msg.m_tx_HEY = (ptx_HEY){{}}; break;
    case IN_HEY: m->msg.m_rx_HEY = (prx_HEY){{}}; break;
    case OUT_GET: m->msg.m_tx_GET = (ptx_GET){(va_arg(a,ID))}; break;
    case IN_GET: m->msg.m_rx_GET = (prx_GET){{}}; break;
    case OUT_DEL: m->msg.m_tx_DEL = (ptx_DEL){(va_arg(a,ID))}; break;
    case IN_DEL: m->msg.m_rx_DEL = (prx_DEL){(va_arg(a,ID))}; break;
    case OUT_UPD: m->msg.m_tx_UPD = (ptx_UPD){(va_arg(a,ID))}; break;
    case IN_UPD: m->msg.m_rx_UPD = (prx_UPD){(va_arg(a,UI))}; break;
    case OUT_ADD: m->msg.m_tx_ADD = (ptx_ADD){(va_arg(a,UI))}; break;
    case IN_ADD: m->msg.m_rx_ADD = (prx_ADD){(va_arg(a,UI))}; break;
    case OUT_FND: m->msg.m_tx_FND = (ptx_FND){(va_arg(a,UI))}; break;
    case IN_FND: m->msg.m_rx_FND = (prx_FND){(va_arg(a,UI))}; break;
    case OUT_LST: m->msg.m_tx_LST = (ptx_LST){(va_arg(a,UI),va_arg(a,UI))}; break;
    case IN_LST: m->msg.m_rx_LST = (prx_LST){(va_arg(a,UI),va_arg(a,UI),va_arg(a,UI))}; break;
    case OUT_SRT: m->msg.m_tx_SRT = (ptx_SRT){(va_arg(a,UI),va_arg(a,UI))}; break;
    case IN_SRT: m->msg.m_rx_SRT = (prx_SRT){(va_arg(a,UI),va_arg(a,UI),va_arg(a,UI))}; break;
    case OUT_BYE: m->msg.m_tx_BYE = (ptx_BYE){(va_arg(a,UI))}; break;
    case IN_BYE: m->msg.m_rx_BYE = (prx_BYE){(va_arg(a,UI))}; break;
    CD: T(WARN, "unknown message code: %d", m_type);
  }
  if(tail_offset) {
    SIZETYPE tail_len = va_arg(a,SIZETYPE);
    V* tail_src = va_arg(a,V*);
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
