//! \file str.h \brief string utilities

//! calculate wrapping of \param s to lines of \param n length
//! \param x exec should use \c line_start,line_len
/*! \c pts chunks to split current word into */\
/*! \c lead leading spaces in line */\
/*! \c tail remaining length of long word (tok_len>=n) */\
/*! \c room available space on the current line */\
#define str_wrap(s,n,x) {I lead, line_len=0, line_start=0; UJ text_len=scnt(s), tail, room;\
	stok(s,text_len," \t\n",0,\
		I pts = 1+tok_len/n;if(pts>1){tail=tok_len;room=n-(tok_pos-line_start);tok_len=room;if(room)pts++;}\
		for(I p=0;p<pts;++p){\
			/*if(pts>1)O("DO rnd=%d tok_pos=%lu tok_len=%lu line_len=%lu tail=%lu room=%lu %.*s\n", p, tok_pos, tok_len , line_len, tail, room, tok_len, s+tok_pos);*/\
			if(line_len+tok_len>=n){\
				{x;}line_start+=line_len;lead=tok_pos-line_start;line_start+=lead;line_len=0;\
			}\
			line_len=tok_pos-line_start+tok_len;\
			if(pts>1){tail-=tok_len; tok_pos+=tok_len; room=n; tok_len=MIN(tail, room);}\
		}\
	){x;}/* flush remainder */}

