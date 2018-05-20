//! \file str.h \brief string utilities

#define str_wrap(s,n,x) {I lead,line_len = 0,line_start=0,line_end=0;UJ text_len=scnt(s);\
	stok(s,text_len," \t\n", 0,\
		if(line_len+tok_len>=n)\
			{{x;}line_start+=line_len;lead=tok_pos-line_start;line_start+=lead;line_len=0;}\
		line_len=tok_pos-line_start+tok_len;\
	){x;}}
