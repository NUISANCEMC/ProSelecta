# ProSelectaCPP

A C++ implementation of the ProSelecta Neutrino Measurement prediction Environment version 0.1

## Build

```bash
cd /path/to/ProSelectaCPP
mkdir build; cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$(readlink -f Linux)
make install -j 8
```

## Example

See [examples/example_proselect.cxx](examples/example_proselect.cxx) for a simple example copied below.

```cpp
extern "C" {

bool filt(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::kMuon);
  if (!nu || !mu) {
    return false;
  }

  return true;
}

double proj_q0(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::kMuon);
  return ps::q0(nu, mu) * ps::GeV;
}

double proj_q3(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::kMuon);
  return ps::q3(nu, mu) * ps::GeV;
}

double proj_Q2Lep(HepMC3::GenEvent const &ev) {
  auto nu = ps::GetBeam(ev, ps::kNuMu);
  auto mu = ps::GetOutPartHM(ev, ps::kMuon);
  return ps::Q2Lep(nu, mu) * ps::GeV2;
}

}
```

Currently you need to tell ProSelecta where its own headers are with a `-I /path/to/prefix`, but we can fix this in the future. Run like:

```bash
Linux/bin/ProSelectaCPP -I Linux/include -f ../examples/example_proselect.cxx -i ../examples/neut.vect.hepmc --Filter filt --Project proj_q0 --Project proj_q3 --Project proj_Q2Lep
```

to produce a table of selected event properties:

```csv
# evtnum, pass, proj_q0, proj_q3, proj_Q2Lep
0, pass, 0.595248, 0.847, 0.36309
1, cut,  - ,  - ,  - 
2, pass, 0.0670755, 0.368074, 0.130979
3, pass, 0.715677, 1.01071, 0.509335
4, cut,  - ,  - ,  - 
5, pass, 0.0498576, 0.217403, 0.0447782
6, cut,  - ,  - ,  - 
7, pass, 0.945512, 1.66591, 1.88128
8, pass, 1.52397, 1.79681, 0.906068
9, cut,  - ,  - ,  - 
10, pass, 0.120557, 0.513592, 0.249243
...
```