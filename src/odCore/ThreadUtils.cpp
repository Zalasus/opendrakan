
#include <odCore/ThreadUtils.h>

#define MAY_SET_NAME (USE_PTHREADS && ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12))))

#if MAY_SET_NAME
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif
    #include <pthread.h>
#endif

#include <cerrno>

#include <odCore/Exception.h>

namespace od
{

    namespace ThreadUtils
    {

        void setThreadName(std::thread &thread, const char *name)
        {
            #if MAY_SET_NAME
                int result = pthread_setname_np(static_cast<pthread_t>(thread.native_handle()), name);

                if(result == ERANGE)
                {
                    throw od::Exception("Thread name too long");

                }else if(result != 0)
                {
                    throw od::Exception("Failed to set thread name for unknown reason");
                }
            #endif
        }

    }

}
