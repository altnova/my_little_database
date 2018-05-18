//! \file vim.c \brief simple text editor
//! \see https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html

#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>
#include "___.h"
#include "trc.h"
#include "vim.h"
#include "vec.h"

vim_conf CFG;

ZS vim_prompt(S prompt, I x_offset, S initial_value);
ZV vim_die(S err);

//! screen buffer
ZI vim_draw(SCR scr, S chars, I len) {
	LOG("vim_draw");
	S new = realloc(scr->b, scr->len + len);chk(new,1);
	mcpy(&new[scr->len], chars, len);
	scr->b = new;
	scr->len += len;
	R0;}

ZV vim_clear_scr(SCR s) {
	free(s->b);}

//! term -> canonical mode
ZV vim_raw_off() {
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &CFG.orig_termios)==-1)
		vim_die("tcsetattr failed");}

//! term -> non-canonical mode
ZV vim_raw_on() {
	tcgetattr(STDIN_FILENO, &CFG.orig_termios);
	atexit(vim_raw_off);
	struct termios raw = CFG.orig_termios;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~( OPOST );
	raw.c_cflag |= ( CS8 );
	raw.c_lflag &= ~( ECHO | ICANON | IEXTEN | ISIG );
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);}

//! comms
ZI vim_send(S src, I len) {
	R write(STDOUT_FILENO, src, len);}

ZI vim_recv(S dst, I len) {
	I nread = 0;
	W((nread=read(STDIN_FILENO, dst, len)) != len)
		if(nread==-1&&errno!=EAGAIN)vim_die("rcv error");
	R nread;}

ZV vim_die(S err) {
	vim_send("\x1b[2J", 4);
	vim_send("\x1b[H", 3);
	perror(err);
	exit(1);}

ZI vim_read_key() {
	C c; I nread=vim_recv(&c,1);
	P(c!='\x1b', c); // return as is

	//! parse escape sequence
	C seq[3];
	if(vim_recv(&seq[0],1)!=1)R '\x1b';
	if(vim_recv(&seq[1],1)!=1)R '\x1b';
	if (seq[0]=='[') {
		if(('0'<=seq[1])&&(seq[1]<='9')) {
			if(vim_recv(&seq[2],1)!=1)
				R '\x1b';
			if (seq[2]=='~') {
				SW(seq[1]){
				CS('1',R HOME_KEY;)
				CS('3',R DEL_KEY;)
				CS('4',R END_KEY;)
				CS('5',R PAGE_UP;)
				CS('6',R PAGE_DOWN;)
				CS('7',R HOME_KEY;)
				CS('8',R END_KEY;)
			}}
		} else {
			SW(seq[1]){
			CS('A',R ARROW_UP;)
			CS('B',R ARROW_DOWN;)
			CS('C',R ARROW_RIGHT;)
			CS('D',R ARROW_LEFT;)
			CS('H',R HOME_KEY;)
			CS('F',R END_KEY;)
		}}
	} else if(seq[0]=='O') {
		SW(seq[1]){
		CS('H',R HOME_KEY;)
		CS('F',R END_KEY;)
	}}

	R c;
}

ZV vim_clear_line(S dest) {
	dest[0] = '\0';
}

ZI vim_set_line(S dest, S fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	I len = vsnprintf(dest, 80, fmt, ap);
	va_end(ap);
	R len;}


ZI vim_get_cursor_pos() {
	C buf[32];
	UI i = 0;
	if(vim_send("\x1b[6n",4)!=4)R -1;
	DO(SZ(buf),
		if(vim_recv(&buf[i],1)!=1||buf[i]=='R')break;)

	buf[i] = '\0';

	I x,y;
	P(buf[0]!='\x1b'||buf[1]!='[', -1)
	if(sscanf(&buf[2], "%d;%d", &y, &x)!=2)R -1;
	// TODO update cy,cx?
	R0;
}

ZI vim_get_dimensions(I* rows,I* cols) {
	struct winsize ws;
	P(ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws)==-1||ws.ws_col==0, -1)
	*cols = ws.ws_col;
	*rows = ws.ws_row;
	R0;
}

ZI vim_set_cursor_position(I r, I c) {
	C msg[40];
	I msglen = snprintf(msg, 40, "\x1b[%dC\x1b[%dB", r, c);
	I res = vim_send(msg,msglen)!=12;
	//getCursorPosition();
	CFG.cy = r;
	CFG.cx = c;
	R res;
}

ZV vim_draw_rows(SCR s) {
	DO(CFG.screenrows,
		if(i==CFG.screenrows/3){
			C banner[80];
			I len = snprintf(banner, SZ(banner),
				"editor -- version %s", VERSION);
			I padding = (CFG.screencols - len) / 2;
			if(padding){
				vim_draw(s, "~", 1);
				padding--;
			}
			W(padding--)vim_draw(s," ",1);      
			if(len>CFG.screencols)
				len=CFG.screencols;
			vim_draw(s, banner, len);
		} else {
			vim_draw(s, "~", 1);
		}

		vim_draw(s, "\x1b[K", 3);
		vim_draw(s, "\r\n", 2);
	)
}

ZV vim_draw_status_line(SCR s) {
	vim_draw(s, "\x1b[7m", 4); // green
	I len = scnt(CFG.status);
	O("CFG.status cnt %d\n\n\n", len);	
	if (len>CFG.screencols)
		len=CFG.screencols;
	vim_draw(s, CFG.status, len);
	W(len<CFG.screencols){
		vim_draw(s, " ", 1); // trailing whitespace
		len++;}
	vim_draw(s, "\x1b[m", 3); // restore color
	vim_draw(s, "\r\n", 2);}

ZV vim_draw_message_line(SCR s) {
	vim_draw(s, "\x1b[K", 3);
	I msglen = scnt(CFG.message);
	if(msglen>CFG.screencols)
		msglen=CFG.screencols;
	vim_draw(s, CFG.message, msglen);}

ZV vim_redraw() {
	LOG("vim_redraw");

	pSCR pscr = SCR_INIT;
	SCR scr = &pscr;;

	C buf[32];

	vim_draw(scr, "\x1b[?25l", 6); // hide cursor
	vim_draw(scr, "\x1b[2J", 4); // wipe screen
	vim_draw(scr, "\x1b[H", 3);

	vim_draw_rows(scr);
	vim_draw_status_line(scr);
	vim_draw_message_line(scr);

	snprintf(buf, SZ(buf), "\x1b[%d;%dH", CFG.cy+1, CFG.cx+1); // set cursor

	vim_draw(scr, buf, strlen(buf));
	vim_draw(scr, "\x1b[?25h", 6); // show cursor
	vim_send(scr->b, scr->len); // flush screen buf

	vim_clear_scr(scr);
}

ZV vim_move_cursor(I key) {
	SW(key){
		CS(ARROW_LEFT,
			if(CFG.cx!=MAX(CFG.prompt_offset,0)) {
				CFG.cx--;
			})
		CS(ARROW_RIGHT,
			if(CFG.cx!=CFG.screencols - 1){
				CFG.cx++;
			})
		CS(ARROW_UP,
			if(CFG.cy!=0){
				CFG.cy--;
			})
		CS(ARROW_DOWN,
			if(CFG.cy!=CFG.screenrows-1){
				CFG.cy++;
			})
	}
}

ZS vim_prompt(S prompt, I x_offset, S initial_value) {
	LOG("vim_prompt");
	S result = "TODO";
	
	sz len = scnt(initial_value); //< source can have data
	sz buf_size = len?len:1;
	VEC buffer = vec_init(buf_size,C); // allocate line buffer
	DO(len, vec_add(buffer, initial_value[i]))

	I currpos = len;
	vim_set_cursor_position(CFG.screenrows+2, x_offset+currpos); // jump to end
	CFG.prompt_offset = x_offset;

	EVENTLOOP(
		vim_set_line(CFG.message, prompt, (S)buffer->data);
		//mcpy(CFG.message+x_offset, (S)buffer->data, CFG.screenrows-x_offset); // echo changes
		vim_redraw(); I c = vim_read_key();

		SW(c){
			CS(CTRL_KEY('a'),
				vim_set_line(CFG.status, "Ctrl+a line start (nyi)");
				currpos = 0;
				CFG.cx = x_offset;
				continue;
			) 
			CS(CTRL_KEY('e'),
				vim_set_line(CFG.status, "Ctrl+e line end (nyi)");
				currpos = vec_size(buffer);
				CFG.cx = x_offset + currpos;
				continue;				
			)
			CS(CTRL_KEY('w'),
				vim_set_line(CFG.status, "Ctrl+w zap last word (nyi)");
				
			) // TODO 
			CS(CTRL_KEY('k'),
				vim_set_line(CFG.status, "Ctrl+k kill tail (nyi)");
				
			) // TODO 
			CS(CTRL_KEY('y'),
				vim_set_line(CFG.status, "Ctrl+y yank kill buffer (nyi)");
				
			) // TODO 

			CS(CTRL_KEY('f'),// Meta??
				vim_set_line(CFG.status, "Meta+f word forward (nyi)")) // TODO 
			CS(CTRL_KEY('b'),// Meta??
				vim_set_line(CFG.status, "Meta+b word backward (nyi)")) // TODO 
			CS('\t',
				vim_set_line(CFG.status, "TAB show completions (nyi)")) // TODO 
			CS(CTRL_KEY('_'),
				vim_set_line(CFG.status, "Ctrl+_ undo (nyi)")) 			// TODO 
		}
		if(c==ARROW_UP||c==ARROW_DOWN) {
			vim_set_line(CFG.status, "UP/DOWN browse history (nyi)"); // TODO 
		}

		if(c==ARROW_LEFT||c==ARROW_RIGHT) {
			if(c==ARROW_RIGHT&&(currpos)==vec_size(buffer))
				continue;
			vim_move_cursor(c);
		}

		if(c==DEL_KEY||c==BACKSPACE||c==CTRL_KEY('h')){
			vec_del_at(buffer, currpos, 1);
			if(c!=DEL_KEY) {
				vim_move_cursor(ARROW_LEFT);
				currpos--;
			}
		}

		else if (c == '\x1b') { 				// escape:
			vec_del_all(buffer);				// truncate input
			currpos = 0;
			goto DONE;

		} else if(c == '\r'){ 					// enter
			goto DONE;

		} else if(!iscntrl(c)&&c<128){			// detect printable better?
			vec_add(buffer,c);					// TODO should be insert_at
			vim_move_cursor(ARROW_RIGHT);
			currpos++;
		}
	)

	DONE:
	if(vec_size(buffer) != 0) {
		//TODO
		// do something meaningful with the line bufer
	}
	CFG.prompt_offset = 0; // reset to default
	vim_clear_line(CFG.message); 
	vim_set_cursor_position(0,0);
	vim_redraw();
	vec_destroy(buffer);

	R result;
}

/*

void editorRowDelChar(erow *row, int at) {
	if (at < 0 || at >= row->size) return;
	memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
	row->size--;
	editorUpdateRow(row);
	CFG.dirty++;
}

void editorRowInsertChar(erow *row, int at, int c) {
	if (at < 0 || at > row->size) at = row->size;
	row->chars = realloc(row->chars, row->size + 2);
	memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
	row->size++;
	row->chars[at] = c;
	editorUpdateRow(row);
}
*/

ZV vim_del_char() {
	// TODO
	//if (CFG.cy == CFG.numrows) return;
	//erow *row = &CFG.row[CFG.cy];
	if (CFG.cx > 0) {
	//  editorRowDelChar(row, CFG.cx - 1);
		CFG.cx--;
	}
}

ZV vim_ins_char(I c) {
	// TODO
	//if (CFG.cy == CFG.numrows) {editorAppendRow("", 0);}
	//editorRowInsertChar(&CFG.row[CFG.cy], CFG.cx, c);
	CFG.cx++;
}

ZV vim_save_changes() {
	//todo
	S answer = vim_prompt("Save changes? (Y/N): %s", 21, "");
	if(answer==NULL) {	
		vim_set_line(CFG.status, "Save aborted.");
		R;
	}
	// TODO
	vim_set_line(CFG.status, "Record saved, %d bytes written to disk", 42);
}

V vim_insert_newline() {
	vim_set_line(CFG.status, "vim_insert_newline -> %s", "TODO");
}

ZI vim_dispatch_events() {

	I event = vim_read_key();
	SW(event){
		CS('\r',
			vim_insert_newline())

		CS(CTRL_KEY('q'),
			vim_send("\x1b[2J", 4);
			vim_send("\x1b[H", 3);
			R1) // should break main loop

		CS(CTRL_KEY('s'),
			vim_save_changes())

		CS(HOME_KEY,
			CFG.cx = 0)

		CS(END_KEY,
			CFG.cx = CFG.screencols - 1)

		case BACKSPACE:
		case CTRL_KEY('h'):
		case DEL_KEY:
			if (event==DEL_KEY)
				vim_move_cursor(ARROW_RIGHT);
			vim_del_char();
			break;

		case PAGE_UP:
		case PAGE_DOWN:
			DO(CFG.screenrows,
				vim_move_cursor(event==PAGE_UP?ARROW_UP:ARROW_DOWN))
			break;

		case ARROW_UP:
		case ARROW_DOWN:
		case ARROW_LEFT:
		case ARROW_RIGHT:
			vim_move_cursor(event);
			break;
		CD:
			vim_ins_char(event);          
	}

	R0; // main loop continue
}

V vim_init() {
	CFG.cx = 0;
	CFG.cy = 0;	
	if(vim_get_dimensions(&CFG.screenrows, &CFG.screencols) == -1)
		vim_die("vim_get_dimensions");
	CFG.screenrows -= 2; // two bottom lines are status and msg lines
	vim_clear_line(CFG.status);
	vim_clear_line(CFG.message);
}

I main() {
	vim_raw_on();

	vim_init();
	vim_set_line(CFG.status, "HELP: Ctrl-Q = quit");

	S cmd = vim_prompt("    $ %s", 6, "mary had a little lamb");

	EVENTLOOP(
		vim_redraw();
		if(vim_dispatch_events())break;
	)

	vim_raw_off();
	R0;
}

