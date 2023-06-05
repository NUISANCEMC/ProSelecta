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

bool filt(HepMC3::GenEvent const &ev){
  auto nu = GetBeam(ev,14);
  auto mu = GetOutPartHM(ev,13);
  if(!nu || !mu){
    return false;
  }

  return true;
}

double proj1(HepMC3::GenEvent const &ev){
  auto mu = GetOutPartHM(ev,13);
  return mu->momentum().e() * GeV;
}

double proj2(HepMC3::GenEvent const &ev){
  auto mu = GetOutPartHM(ev,13);
  return mu->momentum().pt() * GeV;
}

}
```

Currently you need to tell ProSelecta where its own headers are with a `-I /path/to/prefix`, but we can fix this in the future. Run like:

```bash
Linux/bin/ProSelectaCPP -I Linux/include -f ../examples/example_proselect.cxx -i ../examples/neut.vect.hepmc --Filter filt --Project proj1 --Project proj2
```

to produce a table of selected event properties:

```csv
# evtnum, pass, proj1, proj2
0, pass, 2.74868, 0.541607
1, cut,  - ,  - 
2, pass, 0.303215, 0.261519
3, pass, 0.451355, 0.374716
4, cut,  - ,  - 
5, pass, 0.154291, 0.111101
6, cut,  - ,  - 
7, pass, 3.1937, 1.18183
8, pass, 1.73888, 0.676686
9, cut,  - ,  - 
10, pass, 0.558551, 0.407745
...
```