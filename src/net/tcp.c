//! \file tcp.c \brief comms

#include "../___.h"
#include "tcp.h"

#define LOCALHOST 0x7F000001
#define SNDBUF       0x10000
#define RCVBUF       0x10000
#define LSTN_BACKLOG    1000

ZI sig=0; Z Q queues[CN];
Z  fd_set mr,mw; Z I(*df[CN])();
ZI D1,D2,ACTIVE_CONNS=0;
ZJ timer = 1500;
Z  TCP_TIMER_FN timer_fn;
Z UJ ctr=0;

Z V Q0(I i){Q q=queues[i];queues[i]=q->next,free(q);}
Z Q Q1(I i){Q q=(Q)calloc(SZ(pQ),1);R q->size=0,q->obj=0,queues[i]=q;}

ZI dd[CN],               //< conn handles
   da[CN];               //< conn addrs

ZV hrr(){errno=EHOSTUNREACH;}
ZI qbl(){R errno==EWOULDBLOCK;}
ZI qip(){R errno==EINPROGRESS;}
ZI exe(I d){R fcntl(d,F_SETFD,FD_CLOEXEC)?-1:d;}
 I clo(I d){R fcntl(d,F_SETFD,FD_CLOEXEC),d;}
ZI snb(I d,I f){R fcntl(d,F_SETFL,f?O_NDELAY:0);}
ZI cls(I d){R close(d);}

V  ipv4(I a, S b){sprintf(b,"%d.%d.%d.%d",255&a>>24,255&a>>16,255&a>>8,255&a);}
I  port(I d){I n=16;R getsockname(d,(Sa)&sa,(UI*)&n),ntohs(sa.sin_port);}
S  host(I a){R a=htonl(a),(h=gethostbyaddr((S)&a,4,AF_INET))?h->h_name:"";}
I  addr(S s){I a;R!*s?LOCALHOST:ntohl((a=inet_addr((S)s))!=-1?a
	:(h=gethostbyname((S)s))?*(I*)h->h_addr_list[0]:(hrr(),-1));}

ZI sopt(I d,I o,I i){R setsockopt(d,o?SOL_SOCKET:IPPROTO_TCP,o?o:TCP_NODELAY,(S)&i,4);}

ZI sock(){I d;W(0==(d=socket(AF_INET,SOCK_STREAM,0))){};
	P(-1==d,d)sopt(d,0,1),sopt(d,SO_KEEPALIVE,1);sopt(d,SO_REUSEADDR,1);
	if(0)sopt(d,SO_SNDBUF,SNDBUF),sopt(d,SO_RCVBUF,RCVBUF);R clo(d),d;}

ZI cbnd(I f,I d,I a,I p){struct sockaddr_in sa={AF_INET};R sa.sin_port=htons((H)p),
	sa.sin_addr.s_addr=htonl(a),(f?connect:bind)(d,(Sa)&sa,16)&&!qip()?(cls(d),-1):exe(d);}

I  conn(I a,I p){R cbnd(1,sock(),a,p);}
I  lstn(I a,I p){I d=cbnd(0,sock(),a,p);R-1==d?d:listen(d,LSTN_BACKLOG)?(cls(d),-1):d;}
ZI accp(I d,I*a){I n=16;R d=accept((UI)d,(Sa)&sa,(UI*)&n),*a=ntohl(sa.sin_addr.s_addr),clo(d),d;}

ZI hget(I d){DO(CN,P(dd[i]==d,i))R CN;} //!< get connection index
ZI hadd(I d){I i=hget(-1);if(i<CN)dd[i]=d;R i;} //!< add connection handle

I  acc(I*p,I d){LOG("acc");I r = accp(d,p+1);*p=r;R0;}
I  rcv(I d,V*b,I n){LOG("rcv");I r = recv(d,b,n,0);T(TRACE, "rcv %d bytes\n", r);R r;}
I  snd(I d,V*b,I n){R send(d,b,n,0);}
V  end(I d){dd[hget(d)]=-1,cls(d);}

I  sd0(I d){
	LOG("sd0");
	I i=hget(d);P(i>=CN,0)
	--ACTIVE_CONNS,dd[i]=-1;
	if(d)cls(d);
	FD_CLR((UI)d,&mr);
	FD_CLR((UI)d,&mw);
	W(queues[i])Q0(i);
	free(queues[i]);
	T(TRACE, "client disconnected (%d)");
	R0;}

I  sd1(I d,I(*fn)(I)){LOG("sd1");
	if(d<0)snb(d=-d,1);
	I i=hadd(d);
	T(TRACE,"sd1 adding %d", d);
	X(i==CN,(cls(d),T(WARN,"maxconn")),-1);
	if(i>=D1)D1=i+1;
	if(++ACTIVE_CONNS,df[i]=fn,da[i]=0,d>=D2)D2=d+1;
	queues[i]=Q1(i);
	R d;}

I  sd2(I d,I(*fn)(I),I a){
	I x=sd1(-d,fn);P(!x,-1) 
	R da[hget(d)]=a,x;}

I  io(I f,I d,V*x,I n){I j=f?rcv(d,x,n):snd(d,x,n);R f&&!j?-1:qbl()-1;}

I  qsnd(I d){I i=hget(d),j;Q b;for(;b=queues[i];Q0(i)){LOG("qsnd");
	j=io(0,d,b->obj,b->size);}FD_CLR((UI)d,&mw);R j;}

I  poke(){LOG("poke");
	I d;DO(D1,if(d=dd[i],-1!=d){FD_SET((UI)d,&mr);
		if(queues[i]&&queues[i]->obj)FD_SET((UI)d,&mw);})
	R1;}

ZJ usec(){struct timeval tv;R gettimeofday(&tv,0),1000000*(J)(tv.tv_sec-10957*86400)+tv.tv_usec;}

I tcp_active_conns() {
	R ACTIVE_CONNS;}

/*ZV*sbf[CN];ZI sbc[CN];ZC sbl[CN];//bufs,cnts,lcks
V* sbuf(I d,I n){ //< create/destroy snd buf
	LOG("sbuf");
	I i=hget(d);
	P(n&&sbl[i],0);//< buffer is locked by another stream
	V*b=sbf[i];
	if(b&&!n){free(b);sbf[i]=0;T(TEST,"freed sbuf %d",d);R0;}
	P(b,b)sbf[i]=malloc(n);chk(sbf[i],0);
	sbl[i]=1; //< lock
	R sbf[i];}
V  sbpt(I d,V*o,I n){ //< put bytes
	I i = hget(d);
	V*b = sbf[i]+sbc[i];
	mcpy(b,o,n);}
V sblk(I d,C l){ //< set lock
	LOG("sblk");
	sbl[hget(d)]=l;}
V sbc1(I d,I n){ //< inc size
	LOG("sbc1");
	sbc[hget(d)]+=n;}
V sbfl(I d,I n){ //< flush
	LOG("sbc1");
	sbc[hget(d)]+=n;}
*/
	
I tcp_select(struct timeval tv) {
	tv.tv_sec=(I)1,tv.tv_usec=(I)0;
	R select(D2,&mr,&mw,0,&tv);}

I tcp_trigger_reads() {
	I d; LOG("tcp_trigger_reads");
	DO(D1,if(d=dd[i],-1!=d&&FD_ISSET(d,&mr))df[i](d);)
	R1;}

V tcp_set_timer(J t, TCP_TIMER_FN fn) {
	timer = t; ctr = 0;
	timer_fn = fn;}

I tcp_serve() {
	LOG("tcp_serve");
	struct timeval tv;
	I d,u=1000000;J j=usec(),j1=usec();
	W(ACTIVE_CONNS){
		if(sig)goto DONE;
		j=usec();
		if(j>=j1){
			if(timer_fn)timer_fn(ctr++);
		do j1+=timer?timer * (J)1000:u;W(j>j1);}

		if(poke(),j=j1-j,tv.tv_sec=(I)(j/u),tv.tv_usec=(I)(j%u),
			d=select(D2,&mr,&mw,0,&tv),d>0){//T(TEST,"tick");
			DO(D1,if(d=dd[i],-1!=d&&FD_ISSET(d,&mw))queues[i]?qsnd(d):0;)
			DO(D1,if(d=dd[i],-1!=d&&FD_ISSET(d,&mr))df[i](d))}}
	DONE:
	R0;	
}

I tcp_stdin(I d, STDIN_CALLBACK fn) {
	LOG("tcp_stdin"); I rd = 0, total=0;
	C b[4000];
	rd=read(d,b,4000);
	//T(TEST, "looping stdin... %d", rd);
	I r = 0;
	if(rd>=0) {
		//write(1, b, rd);
		r = fn(d, rd, (S)&b[0]);
	}
	R r;}

I tcp_init() {
	DO(CN,dd[i]=-1);
	R0;}

I tcp_shutdown() {
	LOG("tcp_shutdown");
	I d;
	DO(D1,if(d=dd[i],-1!=d){
		sd0(d); T(TRACE, "killed conn #%d", d);
	})
	R0;
}


//:~

