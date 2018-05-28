/*!
 * message header
 */
 #define SIZETYPE UI
typedef struct msg_hdr { G ver, type; SIZETYPE len;} __attribute__((packed)) MSG_HDR;
 #define SZ_MSG_HDR SZ(MSG_HDR)
/*!
 * message type definitions
 */
typedef struct msg_0 { SIZETYPE data_len; C username[0]; } __attribute__((packed)) ptx_HEY; typedef ptx_HEY *tx_HEY;typedef struct msg_10 { SIZETYPE data_len; UJ info[0]; } __attribute__((packed)) prx_HEY; typedef prx_HEY *rx_HEY;
typedef struct msg_1 { ID rec_id; } __attribute__((packed)) ptx_GET; typedef ptx_GET *tx_GET;typedef struct msg_11 { SIZETYPE data_len; pRec record[0]; } __attribute__((packed)) prx_GET; typedef prx_GET *rx_GET;
typedef struct msg_2 { ID rec_id; } __attribute__((packed)) ptx_DEL; typedef ptx_DEL *tx_DEL;typedef struct msg_12 { ID rec_id; } __attribute__((packed)) prx_DEL; typedef prx_DEL *rx_DEL;
typedef struct msg_3 { UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) ptx_UPD; typedef ptx_UPD *tx_UPD;typedef struct msg_13 { UI cnt; } __attribute__((packed)) prx_UPD; typedef prx_UPD *rx_UPD;
typedef struct msg_4 { UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) ptx_ADD; typedef ptx_ADD *tx_ADD;typedef struct msg_14 { UI cnt; } __attribute__((packed)) prx_ADD; typedef prx_ADD *rx_ADD;
typedef struct msg_5 { UI max_hits; SIZETYPE data_len; C query[0]; } __attribute__((packed)) ptx_FND; typedef ptx_FND *tx_FND;typedef struct msg_15 { UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) prx_FND; typedef prx_FND *rx_FND;
typedef struct msg_6 { UI page_num; UI per_page; } __attribute__((packed)) ptx_LST; typedef ptx_LST *tx_LST;typedef struct msg_16 { UI page_num; UI out_of; UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) prx_LST; typedef prx_LST *rx_LST;
typedef struct msg_7 { UI field_id; UI dir; } __attribute__((packed)) ptx_SRT; typedef ptx_SRT *tx_SRT;typedef struct msg_17 { UI page_num; UI out_of; UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) prx_SRT; typedef prx_SRT *rx_SRT;
typedef struct msg_8 { UI msgs_sent; } __attribute__((packed)) ptx_BYE; typedef ptx_BYE *tx_BYE;typedef struct msg_18 { UI msgs_rcvd; } __attribute__((packed)) prx_BYE; typedef prx_BYE *rx_BYE;
/*!
 * message types to int
 */
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
/*!
 * common type
 */
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
/*!
 * assembled message
 */
typedef struct {
    MSG_HDR hdr;
    pMSG msg;
} *MSG;
/*!
 * message factory
 */
ext MSG rpc_create_HEY_req(SIZETYPE username_len, S username);
ext MSG rpc_create_HEY_res(SIZETYPE info_len, UJ info);
ext MSG rpc_create_GET_req(ID rec_id);
ext MSG rpc_create_GET_res(SIZETYPE record_len, Rec record);
ext MSG rpc_create_DEL_req(ID rec_id);
ext MSG rpc_create_DEL_res(ID rec_id);
ext MSG rpc_create_UPD_req(UI cnt, SIZETYPE records_len, Rec records);
ext MSG rpc_create_UPD_res(UI cnt);
ext MSG rpc_create_ADD_req(UI cnt, SIZETYPE records_len, Rec records);
ext MSG rpc_create_ADD_res(UI cnt, SIZETYPE records_len, Rec records);
ext MSG rpc_create_FND_req(UI max_hits, SIZETYPE query_len, S query);
ext MSG rpc_create_FND_res(UI cnt, SIZETYPE records_len, Rec records);
ext MSG rpc_create_LST_req(UI page_num, UI per_page);
ext MSG rpc_create_LST_res(UI page_num, UI out_of, SIZETYPE records_len, Rec records);
ext MSG rpc_create_SRT_req(UI field_id, UI dir);
ext MSG rpc_create_SRT_res(UI page_num, UI out_of, SIZETYPE records_len, Rec records);
ext MSG rpc_create_BYE_req();
ext MSG rpc_create_BYE_res();
/*!
 * public methods
 */
ext V rpc_init();
ext V rpc_dump_header(MSG m);
//:~
