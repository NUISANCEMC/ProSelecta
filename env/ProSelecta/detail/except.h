#pragma once

#include <stdexcept>
#include <string>

namespace ps::detail {

struct exception : public std::exception {
  std::string msg;

  exception(std::string const &m) : msg(m) {}
  const char *what() const noexcept { return msg.c_str(); }
};

} // namespace ps::detail

#define NEW_PS_EXCEPT(exceptname)                                              \
  struct exceptname : public ps::detail::exception {                           \
    using ps::detail::exception::exception;                                    \
  }

