
#ifndef INCLUDE_THREADUTILS_H_
#define INCLUDE_THREADUTILS_H_

#include <thread>

namespace od
{

    namespace ThreadUtils
    {

        /**
         * @brief Attempts to assign a name to the given thread.
         *
         * This is only to aid in debugging. On a platform that does not support
         * setting thread names, this will be a no-op.
         *
         * Linux imposes a 16 character limit on thread names. If the given name
         * exceeds that limit, this fuction will throw.
         */
        void setThreadName(std::thread &thread, const char *name);

    }

}

#endif
