//! \file set.c \brief set

#include <stdlib.h>
#include <string.h>

#include "___.h"
#include "cfg.h"
#include "trc.h"
#include "bag.h"
#include "hsh.h"
#include "set.h"

SET set_init() {
	LOG("set_init");
	SET s = (SET)calloc(SZ_SET,1);chk(s,NULL);
	s->table = hsh_init(2,3);
	s->bag = bag_init(2);
	s->cnt = 0;
	R s;}

V set_adjust_ptrs(BKT bkt, V*diff, HTYPE i) { bkt->payload -= (J)diff; }

V* set_add(SET s, V*key, sz key_sz, V*payload, sz payload_sz) {
	LOG("set_add");
	BKT b = hsh_ins(s->table, key, key_sz, NULL);
	P(b->payload,b->payload)
	payload = bag_add(s->bag, payload, payload_sz);
	if(s->bag->offset)
		hsh_each(s->table, set_adjust_ptrs, (V*)s->bag->offset);
	s->cnt++;
	R b->payload = payload;
}

V set_destroy(SET s) {
	hsh_destroy(s->table);
	bag_destroy(s->bag);
	free(s);
}

V* set_get(SET h, V*key, sz key_sz) {
	R hsh_get_payload(h->table, key, key_sz);
}

#ifdef RUN_TESTS_SET

I main() {
	LOG("set_test");

	S keys[] = {"abbot", "abbey", "abacus", "abolition", "abolitions", "abortion", "abort", "zero"};
	S payloads[] = {"ABBOT", "ABBEY", "ABACUS", "ABOLITION", "ABOLITIONS", "ABORTION", "ABORT", "ZERO"};

	SET s=set_init();
	X(!s,T(FATAL,"cannot init set"),1);

	S obj;
	TSTART();
	DO(8,
		T(TEST, "adding:");
		obj = (S)set_add(s, keys[i], scnt(keys[i])+1, payloads[i], scnt(payloads[i])+1);
		X(!obj, T(FATAL, "can't add to set"), 1);
		T(TEST, " %s", keys[i]))
	TEND();


	T(TEST,"adding: zero zero2");
	set_add(s, "zero", 5, "ZERO", 5);
	set_add(s, "zero2", 6, "ZERO", 5);

	DO(8,
		obj = (S)set_get(s, keys[i], scnt(keys[i])+1);
		T(TEST, "retrieved %s -> %s (%p)", keys[i], obj, obj);
	)

	O("\n");
	obj = (S)set_get(s, "zero", 5);
	T(TEST, "retrieved %s -> %s (%p)", "zero", obj, obj);

	obj = (S)set_get(s, "zero2", 6);
	T(TEST, "retrieved %s -> %s (%p)", "zero2", obj, obj);


	set_destroy(s);

	R0;
}

#endif


//:~