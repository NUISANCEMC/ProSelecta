#pragma once

#include <functional>
#include <string>
#include <type_traits>

namespace HepMC3 {
class GenEvent;
}

namespace ProSelecta_ftypes {
using sel_p = bool (*)(HepMC3::GenEvent const &);
using pro_p = double (*)(HepMC3::GenEvent const &);
using wgt_p = pro_p;

using sel = std::function<std::remove_pointer_t<sel_p>>;
using pro = std::function<std::remove_pointer_t<pro_p>>;
using wgt = pro;
} // namespace ProSelecta_ftypes