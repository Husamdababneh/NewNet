




#include "base.h"
#include "base_types.h"

#include "memory.h"

#if defined(NO_CRT)
    #if OS_WINDOWS == 1  && COMPILER_CL == 1
        #include "os/win32/win32_nocrt.cpp"
        #include "os/win32/win32_common.cpp"
    #endif // OS_WINDOWS == 1  && COMPILER_CL == 1
	#if OS_LINUX == 1  && COMPILER_CLANG == 1
		#include <sys/mman.h>
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <fcntl.h>
        #include "os/linux/linux_common.cpp"
    #endif // OS_LINUX == 1
#endif // defined(NO_CRT)

#include "memory.cpp"
#include "str.cpp"
#include "io.cpp"
#include "math.cpp"




