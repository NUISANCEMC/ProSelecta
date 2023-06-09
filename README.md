# ProSelectaCPP

A C++ implementation of the ProSelecta Neutrino Measurement prediction Environment version 0.1. See the [Specification](Specification.md).

## Build

```bash
cd /path/to/ProSelectaCPP
mkdir build; cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$(readlink -f Linux)
make install -j 8
```

## Example

See [examples/example_proselect.cxx](examples/example_proselect.cxx) for a simple example.

You can tell `ProSelectaCPP` where to find its processing environment with the `PROSELECTA_DIR` environment variable, which should point to the installation prefix or with the `--env` flag which should point directly to the directory containing the `env.h` header, expected to be in: `<install_prefix>/include/ProSelecta/env`.

```bash
Linux/bin/ProSelectaCPP --env Linux/include/ProSelecta/env -I Linux/include -f ../examples/example_proselect.cxx -i ../examples/neut.vect.hepmc --Annotate annotate --Filter filt --Project proj_q0 projev_q0 proj_q3 proj_Q2Lep proj_NEUTChannel proj_IsCCQE
```

to produce a table of selected event properties:

```csv
# evtnum, pass, proj_q0, projev_q0, proj_q3, proj_Q2Lep, proj_NEUTChannel, proj_IsCCQE
0, pass, 0.595248, 0.595248, 0.847, 0.36309, 13, 0
1, cut,  - ,  - ,  - ,  - ,  - ,  - 
2, pass, 0.0670755, 0.0670755, 0.368074, 0.130979, 1, 1
3, pass, 0.715677, 0.715677, 1.01071, 0.509335, 13, 0
4, cut,  - ,  - ,  - ,  - ,  - ,  - 
5, pass, 0.0498576, 0.0498576, 0.217403, 0.0447782, 1, 1
6, cut,  - ,  - ,  - ,  - ,  - ,  - 
7, pass, 0.945512, 0.945512, 1.66591, 1.88128, 1, 1
8, pass, 1.52397, 1.52397, 1.79681, 0.906068, 21, 0
9, cut,  - ,  - ,  - ,  - ,  - ,  - 
10, pass, 0.120557, 0.120557, 0.513592, 0.249243, 1, 1
11, pass, 0.766354, 0.766354, 1.24976, 0.974599, 11, 0
12, pass, 0.293108, 0.293108, 0.793474, 0.543689, 1, 1
13, cut,  - ,  - ,  - ,  - ,  - ,  - 
14, pass, 0.368622, 0.368622, 0.574321, 0.193962, 2, 0
15, pass, 0.0438378, 0.0438378, 0.221116, 0.0469707, 1, 1
16, cut,  - ,  - ,  - ,  - ,  - ,  - 
17, pass, 0.420517, 0.420517, 0.667029, 0.268093, 11, 0
18, pass, 0.419344, 0.419344, 0.494919, 0.0690959, 13, 0
19, cut,  - ,  - ,  - ,  - ,  - ,  - 
20, pass, 0.444759, 0.444759, 0.754872, 0.372022, 11, 0
...
```
The annotation step is required to interpret the specific simulation interaction channels (in this case NEUT) and annotate the HepMC3 events with attributes that the ProSelecta [Hard-Scatter Channels](Specification.md#hard-scatter-channels) functions use to work.

To run an arbitrary event processing function with prototype `void myfunc(HepMC3::GenEvent const&)` you can use the `--Analysis` argument instead of the `--Filter` argument.

## Python Example

See [examples/example_proselect.py](examples/example_proselect.py) for a simple example of defining functions in python.