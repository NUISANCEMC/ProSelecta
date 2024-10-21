#pragma once

#include "TSystem.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace ps::detail {
struct exception : public std::exception {
  std::stringstream msgstrm;
  std::string msg;

  exception() : msgstrm(), msg() {
    auto CLING_DEBUG = std::getenv("CLING_DEBUG");
    if (CLING_DEBUG) {
      gSystem->StackTrace();
    } else {
      std::cerr << "Failed generating a useful Stack Trace as environment "
                   "variables CLING_DEBUG != 1, define this in the environment "
                   "and re-run the calling process (which might be jupyter) to "
                   "see the stacktrace for this failure."
                << std::endl;
    }
    msg = msgstrm.str();
  }
  exception(std::string const &m) : msgstrm(), msg() {
    auto CLING_DEBUG = std::getenv("CLING_DEBUG");
    if (CLING_DEBUG) {
      gSystem->StackTrace();
    } else {
      std::cerr << "Failed generating a useful Stack Trace as environment "
                   "variables CLING_DEBUG != 1, define this in the environment "
                   "and re-run the calling process (which might be jupyter) to "
                   "see the stacktrace for this failure."
                << std::endl;
    }
    msgstrm << m;
    msg = msgstrm.str();
  }
  exception(exception const &other) : msgstrm(), msg() {
    auto CLING_DEBUG = std::getenv("CLING_DEBUG");
    if (CLING_DEBUG) {
      gSystem->StackTrace();
    } else {
      std::cerr << "Failed generating a useful Stack Trace as environment "
                   "variables CLING_DEBUG != 1, define this in the environment "
                   "and re-run the calling process (which might be jupyter) to "
                   "see the stacktrace for this failure."
                << std::endl;
    }
    msgstrm << other.msg;
    msg = msgstrm.str();
  }
  const char *what() const noexcept { return msg.c_str(); }

  template <typename T> exception &operator<<(T const &obj) {
    msgstrm << obj;
    msg = msgstrm.str();
    return (*this);
  }
};

} // namespace ps::detail

#define NEW_PS_EXCEPT(exceptname)                                              \
  struct exceptname : public ps::detail::exception {                           \
    using ps::detail::exception::exception;                                    \
  }
