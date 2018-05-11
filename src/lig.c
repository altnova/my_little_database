//! test fira code retina ligatures
V*;
char*;
*asdf;

S syntext() {
	fn(*(*asdf),a);
	**asdf;
	a,&a;
	a,**a;
	(a)a;
	asdf(*x);
	LOG("asdf");

	I* asdf; I *asdf;	
	char *asdf; char *c;
	int* asdf; (int*)asdf;
	FILE *fp; FILE*fp;
	(V*)(asdf);
	
	*asdf;
	Rec*asdf;
	(&asdf + *asdf);
	*(asdf + asdf);
	(*asdf);
	&asdf[*asdf];
	*(*(*(*asdf)));
	&(&(&(&asdf)));
	&asdf;
	&(asdf+43);
	x[&abc];
	asdf&asdf;
	2*2;
}

//! Test Fira ligatures and fira-friendly C syntax

/*! \param a an integer argument.
	\param s a constant character pointer.
	\return The test results
	\sa QTstyle_Test(), ~QTstyle_Test(), testMeToo() and publicVar()
*/
V* ligtest() {
	/* hinted comment tags */ 
	// hinted double slash

	/*!
	 * \brief doxytags
	 * \return NULL
	 */
	x->asdf; //< doxy inline

	//! doxygen comments \brief doxytags \return NULL v \~ NIL 
	//! \| ab \$ bc \f} cd \f{ de \f] df \f[ fg \f$ gh
	//! \| a \\ b \. c \" d \@ e \> f \< g 
	//! \:: k \--- l \-- m \& n \% x \$ y \# z
	if(x==y||x!=y||x&&y||x>=y||x<=y){ //< visualize precedence 
		x++; x--; !!x; !x; ~x;
		x^y; x%y; x&y;
		x<<=1; y>>=2; /**< another doxy style */
		x?1:2;
	}
}
