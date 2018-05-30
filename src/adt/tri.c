//! \file tri.c \brief trie
//! \see http://www.mathcs.emory.edu/~cheung/Courses/323/Syllabus/Text/trie01.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../___.h"
#include "tri.h"

Z NODE tri_init_node(TRIE t, C key){
	LOG("tri_init_node");
	NODE n = (NODE)calloc(SZ_NODE,1);chk(n,NULL);
	n->key=key;
	t->mem+=SZ_NODE;
	R n;}

Z NODE tri_ins_at(TRIE t, NODE at, C key) {
	C idx = key-TRI_RANGE_OFFSET;
	NODE n=at->children[idx];
	if(!n){
		n = tri_init_node(t, key);
		P(!n, NULL)
		n->parent = at;
		at->children[idx] = n;
	}
	R n;}

Z NODE tri_paste(TRIE t, S key, UJ n, V*payload, C overwrite) {
	LOG("tri_ins");
	P(!n,NULL)
	DO(n,P(!IN(0,key[i]-TRI_RANGE_OFFSET,TRI_RANGE-1),
		(T(WARN,"unsupported characters in \e[1;32m(%.*s)\e[0m", n, key), NULL)))
	NODE curr = t->root;
	DO(n,curr = tri_ins_at(t, curr, key[i]))
	if(overwrite||!curr->payload){
		curr->payload = payload;
		t->cnt++;
	}
	curr->depth = n; //< depth <=> keylen
	R curr;}

V tri_destroy_node(NODE n, V*arg, I depth) {
	free(n);}

V tri_dump_node(NODE n, V*arg, I depth) {
	DO(depth,O(" "))
	O("%c (%d, %d)\n", n->key, tri_is_leaf(n), n->depth);}

C tri_is_leaf(NODE n) {
	R!!n->payload;}

TRIE tri_init() {
	LOG("tri_init");	
	TRIE t = (TRIE)malloc(SZ_TRIE);chk(t,NULL);
	t->mem+=SZ_TRIE;
	t->cnt = 0;
	t->root = tri_init_node(t,0);
	R t;}

NODE tri_insert(TRIE t, S key, UJ n, V*payload) {
	R tri_paste(t,key,n,payload,0);}

NODE tri_upsert(TRIE t, S key, UJ n, V*payload) {
	R tri_paste(t,key,n,payload,1);}

NODE tri_get(TRIE t, S key) {
	LOG("tri_get");
	sz l = scnt(key); P(!l,NULL)
	NODE curr = t->root;
	C c,idx;
	DO(l,//for(UJ i=0;i<l;i++){
		c=key[i];idx=c-TRI_RANGE_OFFSET; P(!IN(0,idx,TRI_RANGE-1),NULL)
		P(!curr, NULL)
		curr = curr->children[idx];)//}
	R curr;}

ZV tri_each_node(TRIE t, NODE curr, TRIE_EACH fn, V*arg, I depth) {
	fn(curr, arg, depth);
	DO(TRI_RANGE,
		NODE c = curr->children[i];
		if(c)tri_each_node(t,c,fn,arg,depth+1)
	)}

ZV tri_each_node_reverse(TRIE t, NODE curr, TRIE_EACH fn, V*arg, I depth) {
	DO(TRI_RANGE,
		NODE c = curr->children[i];
		if(c)tri_each_node_reverse(t,c,fn,arg,depth+1)
	)
	fn(curr, arg, depth);
}

V tri_each_from(TRIE t, NODE n, TRIE_EACH fn, V*arg) {
	if(!n)R;
	tri_each_node(t, n, fn, arg, 0);}

V tri_each(TRIE t, TRIE_EACH fn, V*arg) {
	tri_each_node(t, t->root, fn, arg, 0);}

V tri_each_reverse(TRIE t, TRIE_EACH fn, V*arg) {
	tri_each_node_reverse(t, t->root, fn, arg, 0);}

V tri_dump(TRIE t) {
	tri_each(t, tri_dump_node, NULL);}

V tri_dump_from(TRIE t, NODE n) {
	tri_each_from(t, n, tri_dump_node, NULL);}

sz tri_destroy(TRIE t){
	tri_each_reverse(t, tri_destroy_node, NULL);
	sz released = t->mem;
	free(t);
	R released;
}

#ifdef RUN_TESTS_TRI

V tri_test_each(NODE n, V*arg, I depth){
	LOG("tri_test_each");
	T(TEST, "key=(%c) depth=%3d arg=%ld", n->key, depth, (J)arg);
}

I main() {

	S keys[] = {"abbot", "abbey", "abacus", "abolition", "abolitions", "abortion", "abort", "zero"};

	LOG("tri_test");
	TRIE t=tri_init();
	X(!t,T(FATAL,"cannot init trie"),1);

	DO(8, X(!tri_insert(t, keys[i], scnt(keys[i]), (V*)1), T(FATAL, "can't insert %s", keys[i]), 1))
	T(TEST,"inserted %lu nodes", t->cnt);

	tri_dump(t);

	tri_each(t, tri_test_each, (V*)42);

	DO(8, X(!tri_get(t, keys[i]), T(FATAL, "can't find %s", keys[i]), 1))

	tri_destroy(t);

	R0;
}

#endif

//:~
