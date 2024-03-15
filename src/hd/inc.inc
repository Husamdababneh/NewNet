




#include "base.h"
#include "base_types.h"

#include "memory.cpp"
#include "str.cpp"
#include "io.cpp"
#include "math.cpp"


#if defined(NO_CRT)
    #if defined(OS_WINDOWS) && defined(COMPILER_CL)
        #include "os/win32/win32_nocrt.cpp"
    #endif // defined(OS_WINDOWS) && defined(COMPILER_CL)
#endif // defined(NO_CRT)


