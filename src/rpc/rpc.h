//:~
//! \file rpc.h \brief rpc api
 #pragma once
/*!
 * message header
 */
 #define SIZETYPE UI
typedef struct msg_hdr { G ver, type; SIZETYPE len;} __attribute__((packed)) MSG_HDR;
typedef struct pagination {
  UI page_num, per_page, sort_by, sort_dir;
} __attribute__((packed)) pPAGING_INFO;
typedef pPAGING_INFO *PAGING_INFO;
 #define SZ_MSG_HDR SZ(MSG_HDR)
/*!
 * message type definitions
 */
typedef struct msg_0 { SIZETYPE cnt; S username[0]; } __attribute__((packed)) pHEY_req; typedef struct msg_10 { SIZETYPE cnt; DB_INFO db_info[0]; } __attribute__((packed)) pHEY_res;
typedef struct msg_1 { ID rec_id; } __attribute__((packed)) pGET_req; typedef struct msg_11 { SIZETYPE cnt; pRec record[0]; } __attribute__((packed)) pGET_res;
typedef struct msg_2 { ID rec_id; } __attribute__((packed)) pDEL_req; typedef struct msg_12 { ID rec_id; } __attribute__((packed)) pDEL_res;
typedef struct msg_3 { SIZETYPE cnt; pRec records[0]; } __attribute__((packed)) pUPD_req; typedef struct msg_13 { ID rec_id; } __attribute__((packed)) pUPD_res;
typedef struct msg_4 { SIZETYPE cnt; pRec records[0]; } __attribute__((packed)) pADD_req; typedef struct msg_14 { ID rec_id; } __attribute__((packed)) pADD_res;
typedef struct msg_5 { UI max_hits; SIZETYPE cnt; S query[0]; } __attribute__((packed)) pFND_req; typedef struct msg_15 { SIZETYPE cnt; pRec records[0]; } __attribute__((packed)) pFND_res;
typedef struct msg_6 { SIZETYPE cnt; pPAGING_INFO pagination[0]; } __attribute__((packed)) pLST_req; typedef struct msg_16 { SIZETYPE cnt; pRec records[0]; } __attribute__((packed)) pLST_res;
//mtype( 7, SRT,     _2(tUI(field_id), tUI(dir)),
//                   _3(tUI(page_num), tUI(out_of), tARR(pRec,records)))
typedef struct msg_8 { G empty[0]; } __attribute__((packed)) pBYE_req; typedef struct msg_18 { G empty[0]; } __attribute__((packed)) pBYE_res;
typedef struct msg_9 { SIZETYPE cnt; S msg[0]; } __attribute__((packed)) pSAY_req; typedef struct msg_19 { SIZETYPE cnt; S msg[0]; } __attribute__((packed)) pSAY_res;
typedef struct msg_50 { UI err_id; SIZETYPE cnt; S msg[0]; } __attribute__((packed)) pERR_req; typedef struct msg_150 { UI err_id; SIZETYPE cnt; S msg[0]; } __attribute__((packed)) pERR_res;
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
    //msg_code(7,SRT),
    BYE_req = 8, BYE_res = 18,
    SAY_req = 9, SAY_res = 19,
    ERR_req = 50, ERR_res = 150
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
    //msg_ref(SRT)
    pBYE_req BYE_req; pBYE_res BYE_res;
    pSAY_req SAY_req; pSAY_res SAY_res;
    pERR_req ERR_req; pERR_res ERR_res;
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
ext MSG rpc_HEY_req(SIZETYPE username_cnt, S username);
ext MSG rpc_HEY_res(SIZETYPE info_cnt, DB_INFO info);
ext MSG rpc_GET_req(ID rec_id);
ext MSG rpc_GET_res(SIZETYPE record_cnt, Rec record);
ext MSG rpc_DEL_req(ID rec_id);
ext MSG rpc_DEL_res(ID rec_id);
ext MSG rpc_UPD_req(SIZETYPE records_cnt, Rec records);
ext MSG rpc_UPD_res(ID rec_id);
ext MSG rpc_ADD_req(SIZETYPE records_cnt, Rec records);
ext MSG rpc_ADD_res(ID rec_id);
ext MSG rpc_FND_req(UI max_hits, SIZETYPE query_cnt, S query);
ext MSG rpc_FND_res(SIZETYPE records_cnt, Rec records);
ext MSG rpc_LST_req(SIZETYPE pagination_cnt, PAGING_INFO pagination);
ext MSG rpc_LST_res(SIZETYPE records_cnt, Rec records);
//msg_create_proto2(        SRT_req, _arg(UI,field_id), _arg(UI,dir));
//msg_create_proto_w_tail2( SRT_res, _arg(UI,page_num), _arg(UI,out_of), _tail(Rec,records));
ext MSG rpc_BYE_req();
ext MSG rpc_BYE_res();
ext MSG rpc_SAY_req(SIZETYPE msg_cnt, S msg);
ext MSG rpc_SAY_res(SIZETYPE msg_cnt, S msg);
ext MSG rpc_ERR_req(UI errno, SIZETYPE msg_cnt, S msg);
ext MSG rpc_ERR_res(UI errno, SIZETYPE msg_cnt, S msg);
/*!
 * public methods
 */
ext I rpc_init();
ext G rpc_ver();
ext V rpc_dump_header(MSG m);
Z MSG rpc_alloc(I m_type, SIZETYPE tail_len, V*tail_src);
//:~
