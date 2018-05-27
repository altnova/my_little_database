//! message header
typedef struct msg_hdr { G ver, type; UI len;} MSG_HDR;
Z UI MSG_SIZES[2*100];
Z G MSG_ARGC[2*100];
//! message definitions
typedef struct msg_0 { MSG_HDR hdr; UI ver_hi; UI ver_lo; } ptx_HEY; typedef ptx_HEY *tx_HEY;typedef struct msg_10 { MSG_HDR hdr; UI ver_hi; UI ver_lo; } prx_HEY; typedef prx_HEY *rx_HEY;
typedef struct msg_1 { MSG_HDR hdr; ID rec_id; } ptx_GET; typedef ptx_GET *tx_GET;typedef struct msg_11 { MSG_HDR hdr; ID rec_id; SIZETYPE el_size; pRec record[0]; } prx_GET; typedef prx_GET *rx_GET;
typedef struct msg_2 { MSG_HDR hdr; ID rec_id; } ptx_DEL; typedef ptx_DEL *tx_DEL;typedef struct msg_12 { MSG_HDR hdr; ID rec_id; } prx_DEL; typedef prx_DEL *rx_DEL;
typedef struct msg_3 { MSG_HDR hdr; ID rec_id; UI cnt; SIZETYPE el_size; pRec records[0]; } ptx_UPD; typedef ptx_UPD *tx_UPD;typedef struct msg_13 { MSG_HDR hdr; UI cnt; } prx_UPD; typedef prx_UPD *rx_UPD;
typedef struct msg_4 { MSG_HDR hdr; UI cnt; SIZETYPE el_size; pRec records[0]; } ptx_ADD; typedef ptx_ADD *tx_ADD;typedef struct msg_14 { MSG_HDR hdr; UI cnt; } prx_ADD; typedef prx_ADD *rx_ADD;
typedef struct msg_5 { MSG_HDR hdr; UI max_hits; SIZETYPE query_len; C query[0]; } ptx_FND; typedef ptx_FND *tx_FND;typedef struct msg_15 { MSG_HDR hdr; UI cnt; SIZETYPE el_size; pRec hits[0]; } prx_FND; typedef prx_FND *rx_FND;
typedef struct msg_6 { MSG_HDR hdr; UI page_num; UI per_page; } ptx_LST; typedef ptx_LST *tx_LST;typedef struct msg_16 { MSG_HDR hdr; UI page_num; UI out_of; UI cnt; SIZETYPE el_size; pRec records[0]; } prx_LST; typedef prx_LST *rx_LST;
typedef struct msg_7 { MSG_HDR hdr; UI field_id; UI dir; } ptx_SRT; typedef ptx_SRT *tx_SRT;typedef struct msg_17 { MSG_HDR hdr; UI page_num; UI out_of; UI cnt; SIZETYPE el_size; pRec records[0]; } prx_SRT; typedef prx_SRT *rx_SRT;
typedef struct msg_8 { MSG_HDR hdr; UI msgs_sent; } ptx_BYE; typedef ptx_BYE *tx_BYE;typedef struct msg_18 { MSG_HDR hdr; UI msgs_rcvd; } prx_BYE; typedef prx_BYE *rx_BYE;
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
    msg_tx_HEY = 0, msg_rx_HEY = 10,
    msg_tx_GET = 1, msg_rx_GET = 11,
    msg_tx_DEL = 2, msg_rx_DEL = 12,
    msg_tx_UPD = 3, msg_rx_UPD = 13,
    msg_tx_ADD = 4, msg_rx_ADD = 14,
    msg_tx_FND = 5, msg_rx_FND = 15,
    msg_tx_LST = 6, msg_rx_LST = 16,
    msg_tx_SRT = 7, msg_rx_SRT = 17,
    msg_tx_BYE = 8, msg_rx_BYE = 18
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
    MSG_SIZES[0]=SZ_TX_HEY; MSG_SIZES[0 +100]=SZ_RX_HEY; MSG_ARGC[0]=2; MSG_ARGC[0 +100]=2; // UI,UI / UI/UI
    MSG_SIZES[1]=SZ_TX_GET; MSG_SIZES[1 +100]=SZ_RX_GET; MSG_ARGC[1]=1; MSG_ARGC[1 +100]=3; // ID / ID,SZ,{}
    MSG_SIZES[2]=SZ_TX_DEL; MSG_SIZES[2 +100]=SZ_RX_DEL; MSG_ARGC[2]=1; MSG_ARGC[2 +100]=1; // ID / ID
    MSG_SIZES[3]=SZ_TX_UPD; MSG_SIZES[3 +100]=SZ_RX_UPD; MSG_ARGC[3]=4; MSG_ARGC[3 +100]=1; // ID,UI,SZ,{} / UI
    MSG_SIZES[4]=SZ_TX_ADD; MSG_SIZES[4 +100]=SZ_RX_ADD; MSG_ARGC[4]=3; MSG_ARGC[4 +100]=1; // UI,SZ,{} / UI
    MSG_SIZES[5]=SZ_TX_FND; MSG_SIZES[5 +100]=SZ_RX_FND; MSG_ARGC[5]=3; MSG_ARGC[5 +100]=3; // UI,SZ,{} / UI,SZ,{}
    MSG_SIZES[6]=SZ_TX_LST; MSG_SIZES[6 +100]=SZ_RX_LST; MSG_ARGC[6]=2; MSG_ARGC[6 +100]=5; // UI,UI / UI,UI,UI,SZ,{}
    MSG_SIZES[7]=SZ_TX_SRT; MSG_SIZES[7 +100]=SZ_RX_SRT; MSG_ARGC[7]=2; MSG_ARGC[7 +100]=5; // UI,UI / UI,UI,UI,SZ,{}
    MSG_SIZES[8]=SZ_TX_BYE; MSG_SIZES[8 +100]=SZ_RX_BYE; MSG_ARGC[8]=1; MSG_ARGC[8 +100]=1; // UI / UI
}
ext MSG rpc_make(I m_type, ...) {
  UI m_len = MSG_SIZES[m_type];
  G ver = 42;
  MSG m = calloc(1, m_len);
  va_list a;va_start(a, m_type);
  //I payload_pos = 0, payload_sz = 0;
  m->hdr = (MSG_HDR){ver, m_type, m_len};
  SW(m_type){
    case msg_tx_HEY: m->msg.m_tx_HEY = (ptx_HEY){(va_arg(a,UI),va_arg(a,UI))}; break;
    case msg_rx_HEY: m->msg.m_rx_HEY = (prx_HEY){(va_arg(a,UI),va_arg(a,UI))}; break;
    case msg_tx_GET: m->msg.m_tx_GET = (ptx_GET){(va_arg(a,ID))}; break;
    case msg_rx_GET: m->msg.m_rx_GET = (prx_GET){(va_arg(a,ID),va_arg(a,SIZETYPE))}; break;
    case msg_tx_DEL: m->msg.m_tx_DEL = (ptx_DEL){(va_arg(a,ID))}; break;
    case msg_rx_DEL: m->msg.m_rx_DEL = (prx_DEL){(va_arg(a,ID))}; break;
    case msg_tx_UPD: m->msg.m_tx_UPD = (ptx_UPD){(va_arg(a,ID),va_arg(a,SIZETYPE),va_arg(a,SIZETYPE))}; break;
    case msg_rx_UPD: m->msg.m_rx_UPD = (prx_UPD){(va_arg(a,UI))}; break;
    case msg_tx_ADD: m->msg.m_tx_ADD = (ptx_ADD){(va_arg(a,UI),va_arg(a,SIZETYPE))}; break;
    case msg_rx_ADD: m->msg.m_rx_ADD = (prx_ADD){(va_arg(a,UI))}; break;
    case msg_tx_FND: m->msg.m_tx_FND = (ptx_FND){(va_arg(a,UI),va_arg(a,SIZETYPE))}; break;
    case msg_rx_FND: m->msg.m_rx_FND = (prx_FND){(va_arg(a,UI),va_arg(a,SIZETYPE))}; break;
    case msg_tx_LST: m->msg.m_tx_LST = (ptx_LST){(va_arg(a,UI),va_arg(a,UI))}; break;
    case msg_rx_LST: m->msg.m_rx_LST = (prx_LST){(va_arg(a,UI),va_arg(a,UI),va_arg(a,UI),va_arg(a,SIZETYPE))}; break;
    case msg_tx_SRT: m->msg.m_tx_SRT = (ptx_SRT){(va_arg(a,UI),va_arg(a,UI))}; break;
    case msg_rx_SRT: m->msg.m_rx_SRT = (prx_SRT){(va_arg(a,UI),va_arg(a,UI),va_arg(a,UI),va_arg(a,SIZETYPE))}; break;
    case msg_tx_BYE: m->msg.m_tx_BYE = (ptx_BYE){(va_arg(a,UI))}; break;
    case msg_rx_BYE: m->msg.m_rx_BYE = (prx_BYE){(va_arg(a,UI))}; break;
  }
  //if(size>0) mcpy(m, m+SZ_MSG_HDR, size);
  R m;}
