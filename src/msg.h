//! \file msg.h \brief simple data retrieval protocol

/*
typedef union {
	I first;
	I second;
} INTS;

typedef union {
	RECID rec_id;
	INTS  ints;   
} ARGS;

// msg->args->ints->first
// msg->args->ints->second
// msg->args->rec_id

typedef struct msg_fmt {
	G magic;		   // 42
	C dir[3];          // dir: REQ|RES|ERR|NYI
	C cmd[3];		   // cmd: HLO|GET|DEL|UPD|ADD|FND|LST|SRT|BYE
 ARGS args;  		   // one recid or two ints
	I cnt;			   // number of items in the data section
	I size;			   // byte size of the data section
	G data[0];		   // trailing blob
} __attribute__((packed)) pMSG;
typedef pMSG* MSG;
#define SZ_MSG_HDR SZ(pMSG)

Z pMSG msgbuf;
*/
ext I msg_init();
ext I msg_shutdown();

//ext V print_hdr(MSG m);
//ext MSG make_msg(C a[3], C b[3], I a1, I a2, I cnt, I size, V*blob);
//ext I s_req_hlo(I d, I majver, I minver);
ext I recv_msg(I d);




//:~


