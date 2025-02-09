#pragma once

#include "serverpp/detail/export.hpp"  // IWYU pragma: export

#include <span>
#include <string>
#include <cstdint>

namespace serverpp {

// A byte in serverpp is represented consistently as an unsigned 8-bit integer.
using byte = std::uint8_t;

// A stream of bytes in Server++ is exposed as a non-owning span.  It is
// expected that the data will live for no longer than any function in which
// it is found.  For that reason, these spans should never be stored directly.
// If storage is necessary, it much be converted into a longer-term data
// structure.
using bytes = std::span<byte const>;

// Where necessary, bytes are stored in this type, which has the small string
// optimization, meaning that most cases will not cause an allocation.
using byte_storage = std::basic_string<byte>;

// The port identifier for a server is represented by an unsigned 16-bit
// integer.
using port_identifier = std::uint16_t;

}  // namespace serverpp
