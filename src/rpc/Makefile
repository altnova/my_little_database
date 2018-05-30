all: rpc

rpc:
	rm -f rpc.c rpc.h
	cpp-8 -C -P -trigraphs rpc.in.c > rpc.c
	cpp-8 -C -P -trigraphs -DRPC_HEADER rpc.in.c > rpc.h
	echo "[+] rpc definitions rebuilt"

#//:~
