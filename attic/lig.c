//! \file lig.c debug Fira/Hasklig ligatures, ligature-firendly C syntax and Doxygen tags

//! \brief test pointer highlighting
S syntext() {
	*a;							//< deref
	**a;						//< double indirection
	*(a+b);						//< group deref
	fn(*(*a),*a,(V*)a);			//< ptrs in function args	
	*fn(a,b);&fn(a,b);			//< ref/deref function call
	typedef V(*FN)(I*);			//< function typedef
	a,*a;*a;()*a;				//< ptrs after punctuation
	a,&a;&a;()&a;				//< ref after punctuation
	a,**a;()**a;				//< double ptrs after punctuation
	I* a; I *a;					//< ptrs to abbreviated types
	char *x; FILE *f;			//< ptrs to basic types
	(I*)a;(int*)a;(V*)a;		//< casts to pointers
	({(t*)fn(a,i);})			//< nested cast
	bucket*a=NULL;				//< compressed notation
	a[&a][*a];					//< ref/deref in brackets
	*(*(*(*asdf)));				//< deref in nested groups
	&(&(&(&asdf)));				//< ref in nested groups

	//! non-pointers
	a&a; a&4; 5&a;				//< \c bitwise-and
	x * y;						//< \c multiplication
	x*y+z; x*(y+z);				//< \c confusing for humans...
	x * y+z; x * (y+z);			//< ...always do that instead!

	L:goto L;					//< power <=> responsibility
}

//! \brief test Fira ligatures
V* ligtest(V*a,V*b) {
	/* <- hinted comment tags -> */ 
	// <- hinted double slash

	O(...) //< keyword.operator.variadic.c

	struct->field; //< punctuation.accessor.c

	//! operator soup 
	if(x==y||x!=y||x&&y||x>=y||x<=y){ //< keyword.operator.comparison.c
		x++; x--; //< keyword.operator.increment.c
		!!x; !x; //< keyword.operator.negation.c
		~x; x^y; x%y; x&y, x|y; x<<=1; y>>=2; //< keyword.operator.arithmetic.c
		x=0; x+=0; x*=1; x/=2; x%=3; x|=4; x^=5; //< keyword.operator.assignment.c
		x?1:2; //< keyword.operator.ternary.c
	}
}

/*! \brief test doxygen tags highlighting
 *  @param[out] dest  memory area to copy to
 *  @param[in]  src   memory area to copy from
 *  @param[in]  n     number of bytes to copy
 *  \param      x,y,z position in \e 3D space
 *  \return	   		  TRUE||FALSE
 *
 *  TODO			  fix more bugs!
 */

/** ligature soup
	!== !! !=  !!! !> #( && /=
	#_ #{ #? #> %= %> &% &* &+ &-
	&/ &= &&& &> $> *** *= *> ++
	+++ += +> -- -< << -= -> ->> 
	.. ... ..< .> .~ .= /* // /> 
	/= /== /// /** :: := :> :=> 
	>= <= |= \\ ~= |> <| >>> >=> 
	~~> ~>> ~> |+| ||| |= =:= @>
	www
*/

//:~

