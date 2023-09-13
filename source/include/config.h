#define TRACE_ALLOC LOG
#ifndef TRACE_ALLOC
	#define TRACE_ALLOC(_x)
#endif

#define TRACE_ATEXIT LOG
#ifndef TRACE_ATEXIT
	#define TRACE_ATEXIT(_x)
#endif

#define TRACE_ATRESET LOG
#ifndef TRACE_ATRESET
	#define TRACE_ATRESET(_x)
#endif

#define TRACE_INIT LOG
#ifndef TRACE_INIT
	#define TRACE_INIT(_x)
#endif
