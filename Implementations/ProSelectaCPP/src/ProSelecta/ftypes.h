#pragma once

namespace HepMC3 {
class GenEvent;
}

namespace ProSelecta_ftypes {
typedef bool (*filter)(HepMC3::GenEvent const &);
typedef double (*project)(HepMC3::GenEvent const &);
typedef double (*weight)(HepMC3::GenEvent const &);
} // namespace ftypes
