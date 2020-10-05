
/**
 * This includes common std C types (size_t, ptrdiff_t etc.) and imports them
 * into the global namespace. This is to get rid of the various, random stdint.h
 * inclusions that are all over the engine right now.
 *
 * We might want to transition towards using the std-namespaced versions of
 * these types later.
 */

#ifndef INCLUDE_ODCORE_CTYPES_H_
#define INCLUDE_ODCORE_CTYPES_H_

#include <cstdint>
#include <cstddef>

using size_t    = std::size_t;
using ptrdiff_t = std::ptrdiff_t;

using uint8_t  = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using uint64_t = std::uint64_t;

using int8_t  = std::int8_t;
using int16_t = std::int16_t;
using int32_t = std::int32_t;
using int64_t = std::int64_t;

#endif
