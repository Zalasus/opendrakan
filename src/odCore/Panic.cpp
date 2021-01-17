
#include <odCore/Panic.h>

#include <thread>
#include <iostream>

namespace od
{

    static thread_local std::ostringstream tlsPanicMessageStream;


    PanicMessageProxy::PanicMessageProxy(const char *file, const char *function, int line)
    : mStream(tlsPanicMessageStream)
    {
        auto tid = std::this_thread::get_id();

        mStream.str("");
        mStream << "Thread 0x" << std::hex << tid << std::dec <<
            " panicked";

        if(file != nullptr)
        {
            mStream << " at " << file << "@" << line;
        }

        if(function != nullptr)
        {
            mStream << " (" << function << ")";
        }

        mStream << ":\n"; // ... user message
    }

    PanicMessageProxy::~PanicMessageProxy()
    {
        std::cerr << mStream.str() << std::endl;
        std::terminate();
    }

}
