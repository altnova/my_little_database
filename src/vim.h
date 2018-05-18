//! \file vim.h \brief text editor defns

#define VERSION "0.0.1"
#define CTRL_KEY(k) ((k)&0x1f)

enum KEYS {
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,  
  PAGE_UP,
  PAGE_DOWN  
};

typedef struct config {
  I cx, cy;
  I screenrows;
  I screencols;
  I prompt_offset; //< from the left edge
  C status[80];  //< one up  
  C message[80]; //< bottom line
  struct termios orig_termios;
} vim_conf;

typedef struct scrbuf {
  S b;
  I len;
} pSCR;

#define SZ_SCR SZ(pSCR)
typedef pSCR* SCR;

#define SCR_INIT {NULL, 0}

#define EVENTLOOP(x)   W(1){x;}
