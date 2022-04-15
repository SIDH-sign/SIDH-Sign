.intel_syntax noprefix

// Format function and variable names for Mac OS X
#if defined(__APPLE__)
	#define fmt(f)	_##f
#else
	#define fmt(f)	f
#endif

#define reg_p1  rdi
#define reg_p2  rsi
#define reg_p3  rdx

//TODO Port new code from SIDH for possible running time improvement.