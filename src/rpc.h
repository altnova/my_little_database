/*!
 * message header
 */
 #define SIZETYPE UI
typedef struct msg_hdr { G ver, type; SIZETYPE len;} __attribute__((packed)) MSG_HDR;
 #define SZ_MSG_HDR SZ(MSG_HDR)
/*!
 * message type definitions
 */
typedef struct msg_0 { SIZETYPE data_len; S username[0]; } __attribute__((packed)) pHEY_req; typedef struct msg_10 { SIZETYPE data_len; UJ* info[0]; } __attribute__((packed)) pHEY_res;
typedef struct msg_1 { ID rec_id; } __attribute__((packed)) pGET_req; typedef struct msg_11 { SIZETYPE data_len; pRec record[0]; } __attribute__((packed)) pGET_res;
typedef struct msg_2 { ID rec_id; } __attribute__((packed)) pDEL_req; typedef struct msg_12 { ID rec_id; } __attribute__((packed)) pDEL_res;
typedef struct msg_3 { UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) pUPD_req; typedef struct msg_13 { UI cnt; } __attribute__((packed)) pUPD_res;
typedef struct msg_4 { UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) pADD_req; typedef struct msg_14 { UI cnt; } __attribute__((packed)) pADD_res;
typedef struct msg_5 { UI max_hits; SIZETYPE data_len; S query[0]; } __attribute__((packed)) pFND_req; typedef struct msg_15 { UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) pFND_res;
typedef struct msg_6 { UI page_num; UI per_page; } __attribute__((packed)) pLST_req; typedef struct msg_16 { UI page_num; UI out_of; UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) pLST_res;
typedef struct msg_7 { UI field_id; UI dir; } __attribute__((packed)) pSRT_req; typedef struct msg_17 { UI page_num; UI out_of; UI cnt; SIZETYPE data_len; pRec records[0]; } __attribute__((packed)) pSRT_res;
typedef struct msg_8 { UI msgs_sent; } __attribute__((packed)) pBYE_req; typedef struct msg_18 { UI msgs_rcvd; } __attribute__((packed)) pBYE_res;
/*!
 * message types to int
 */
enum msg_codes {
    HEY_req = 0, HEY_res = 10,
    GET_req = 1, GET_res = 11,
    DEL_req = 2, DEL_res = 12,
    UPD_req = 3, UPD_res = 13,
    ADD_req = 4, ADD_res = 14,
    FND_req = 5, FND_res = 15,
    LST_req = 6, LST_res = 16,
    SRT_req = 7, SRT_res = 17,
    BYE_req = 8, BYE_res = 18
};
/*!
 * common type
 */
typedef union {
    pHEY_req HEY_req; pHEY_res HEY_res;
    pGET_req GET_req; pGET_res GET_res;
    pDEL_req DEL_req; pDEL_res DEL_res;
    pUPD_req UPD_req; pUPD_res UPD_res;
    pADD_req ADD_req; pADD_res ADD_res;
    pFND_req FND_req; pFND_res FND_res;
    pLST_req LST_req; pLST_res LST_res;
    pSRT_req SRT_req; pSRT_res SRT_res;
    pBYE_req BYE_req; pBYE_res BYE_res;
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
ext MSG rpc_create_HEY_req(SIZETYPE username_len, S username);
ext MSG rpc_create_HEY_res(SIZETYPE info_len, UJ* info);
ext MSG rpc_create_GET_req(ID rec_id);
ext MSG rpc_create_GET_res(SIZETYPE record_len, Rec record);
ext MSG rpc_create_DEL_req(ID rec_id);
ext MSG rpc_create_DEL_res(ID rec_id);
ext MSG rpc_create_UPD_req(UI cnt, SIZETYPE records_len, Rec records);
ext MSG rpc_create_UPD_res(UI cnt);
ext MSG rpc_create_ADD_req(UI cnt, SIZETYPE records_len, Rec records);
ext MSG rpc_create_ADD_res(UI cnt);
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
Z MSG rpc_alloc(I m_type, SIZETYPE tail_len, V*tail_src);
//:~
