#define VER_MAJOR       2
#define VER_MINOR       0
#define VER_REVISION    $WCREV$
#define VER_BUILD       $WCNOW=%y%j$
#define VER_BUILD_LONG  $WCNOW=%Y%m%d$

#ifndef STR
#define _STR(s)	#s
#define STR(s)	_STR(s)
#endif
