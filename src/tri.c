//! \file tri.c \brief trie

#include <stdlib.h>
#include <string.h>
#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "tri.h"

Z NODE tri_init_node(UJ id,C key){
	LOG("tri_init_node");
	NODE n = (NODE)calloc(SZ_NODE,1);chk(n,NULL);
	n->id=id; n->key=key;
	R n;}

Z NODE tri_ins_at(TRIE t, NODE at, C key) {
	C idx = key-TRI_RANGE_OFFSET;
	NODE n=at->children[idx];
	if(!n){
		n = tri_init_node(at->id+1,key);
		at->children[idx] = n;
		t->cnt++;
	}
	R n;}

ZV tri_destroy_node(TRIE t, NODE n) {
	DO(TRI_RANGE,
		NODE c = n->children[i];
		if(c)tri_destroy_node(t,c))
	free(n);}

C tri_is_leaf(NODE n){
	R!!n->payload;}

V tri_destroy(TRIE t){
	tri_destroy_node(t,t->root);
	free(t);}

TRIE tri_init() {
	LOG("tri_init");	
	TRIE t = (TRIE)malloc(SZ_TRIE);chk(t,NULL);
	t->cnt = 0;
	t->root = tri_init_node(0,1);
	R t;}

NODE tri_ins(TRIE t, S key, V*payload) {
	LOG("tri_ins");
	sz l = scnt(key); P(!l,NULL)
	DO(l,P(!IN(0,key[i]-TRI_RANGE_OFFSET,TRI_RANGE-1),NULL))
	NODE curr = t->root;
	DO(l,curr = tri_ins_at(t, curr, key[i]))
	curr->payload = payload;
	R curr;}

NODE tri_get(TRIE t, S key) {
	LOG("tri_get");
	sz l = scnt(key); P(!l,NULL)
	NODE curr = t->root;
	C c,idx;
	DO(l,
		c=key[i];idx=c-TRI_RANGE_OFFSET; P(!IN(0,idx,TRI_RANGE-1),NULL)
		curr = curr->children[idx])
	R curr;}

V tri_dump(TRIE t, NODE curr, I depth) {
	C c,idx;
	DO(depth,O(" "))
	O("%c (%lu)=%d\n", curr->key, curr->id, tri_is_leaf(curr));
	DO(TRI_RANGE,
		NODE c = curr->children[i];
		if(c)tri_dump(t,c,depth+1)
	)}

#ifdef RUN_TESTS_TRI

I main() {

	S keys[] = {"abbot", "abbey", "abacus", "abolition", "abolitions", "abortion", "abort", "zero"};

	LOG("tri_test");
	TRIE t=tri_init();
	X(!t,T(FATAL,"cannot init trie"),1);

	DO(8, X(!tri_ins(t, keys[i], 1), T(FATAL, "can't insert %s", keys[i]), 1))
	T(TEST,"inserted %lu nodes", t->cnt);

	tri_dump(t,t->root,0);

	DO(8, X(!tri_get(t, keys[i]), T(FATAL, "can't find %s", keys[i]), 1))
//	T(TEST, "abo -> %d", !!tri_get(t, "abo"));
//	T(TEST, "abbot -> %d", !!tri_get(t, "abbot"));
//	T(TEST, "abolition -> %d", !!tri_get(t, "abbot"));

	tri_destroy(t);
	exit(0);

	C bits[9];
	O("%lu", SZ(long double));
	exit(0);
	DO(256,
		bits_char(i, bits);
		O("%lu b=%s\n", i, bits)
	)
}

#endif