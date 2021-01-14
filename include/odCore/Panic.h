
/**
 * @file
 *
 * This models Rust's panic mechanism. Error handling via exceptions is rather
 * cumbersome and inefficient. 99% of the time when we throw exceptions in OD,
 * it is because we want the program to crash with a defined error message.
 * This does exactly that, while providing a facility to compose an error
 * message like you would with `std::cerr`. Additionally, the source file and
 * line of the panic are captured.
 *
 * Use the OD_PANIC() macro to trigger a panic. You can use the return value of
 * the macro as a stream to write an error message like this:
 * ```
 * if(i > 10)
 *     OD_PANIC() << "Invalid i: " << i;
 * ```
 */

#ifndef INCLUDE_ODCORE_PANIC_H_
#define INCLUDE_ODCORE_PANIC_H_

#include <exception>
#include <sstream>
#include <string>

namespace od
{

    class PanicMessageProxy
    {
    public:

        PanicMessageProxy(const char *file, const char *function, int line);
        [[noreturn]] ~PanicMessageProxy();

        template <typename T>
        PanicMessageProxy &operator<<(const T &v)
        {
            mStream << v;
            return *this;
        }


    private:

        std::ostringstream &mStream; // obtained from TLS

    };

    static constexpr const char *PANIC_UNREACHABLE_MSG = "Reached supposedly unreachable code";
    static constexpr const char *PANIC_UNIMPLEMENTED_MSG = "Unimplemented";
}


#define OD_PANIC() od::PanicMessageProxy(__FILE__, __func__, __LINE__)

#define OD_UNREACHABLE() OD_PANIC() << od::PANIC_UNREACHABLE_MSG;
#define OD_UNIMPLEMENTED() OD_PANIC() << od::PANIC_UNIMPLEMENTED_MSG;
#define OD_CHECK_ARG_NONNULL(arg) if((arg) == nullptr) OD_PANIC() << "Argument " #arg " was nullptr";

#endif // INCLUDE_ODCORE_PANIC_H_
