//! \file msg.h \brief simple data retrieval protocol

#define MSG_MAX_LEN 1000

ext I   msg_init();
ext I   msg_shutdown();
ext I   msg_recv(I d);
ext UI  msg_size(MSG m);
ext V   msg_hdr_dump(MSG_HDR *h);

//:~


