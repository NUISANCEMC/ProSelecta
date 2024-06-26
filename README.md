# ProSelecta

This document corresponds to version 1 of the NUISANCE ProSelecta definition.

ProSelecta is a bit like an Analysis Descriptor Language (ADL), except that it doesn't attempt to define a new Domain Specific Language and instead defines a list of utility functions that an event procesing environment might implement to facilitate the declaration of a measurement's key steps.

ProSelecta is a specification for a minimal set of utilities to support describing the _event_ selection and projection steps of constructing a prediction of a neutrino-scattering measurement in a declarative way.

ProSelecta is also an implementation of this specification in C++ and using HepMC3 as the event format. To avoid needless code duplication, some of the HepMC3 event model is included as part of ProSelecta. While this limits the purity and generality of ProSelecta, we think that it is the right choice practicaly.

ProSelecta is also also a library providing an interface for writing dynamic event processors that JIT C++ source files written against the ProSelecta environment and makeing the functions defined within this source immediately callable for event processing.

A pseudo-python example of what this enables is shown below:

```python

import pyProSelecta as pps

# Usually this would be in an external file, but we can also include C++ source code in
# our python if we really want to
myfuncsrc = """
int isCCNumu(HepMC3::GenEvent const &evt){
  if(!ps::event::has_beam_part(evt, ps::pdg::kNuMu) || !ps::event::has_out_part(evt, ps::pdg::kMuon){
    return false;
  }
  return true;
}

double Enu_GeV(HepMC3::GenEvent const &evt){
  return ps::event::beam_part(evt, ps::pdg::kNuMu)->momentum().e() / unit::GeV;
}
"""

pps.load_text(myfuncsrc) #JIT c++ source code

selection = pps.select.get("isCCNumu") #get a python handle to the C++ selection function
projection = pps.select.get("Enu_GeV") #get a python handle to the C++ projection function

for event in hepmc3_file: #don't ask how we got this file
  if selection(event):
    enu = projection(event)
```

This example is contrived, but hopefully it highlights how a robust event processing environment, coupled to a JIT runtime can enable a range of dynamic, automated workflows.

## Attribution

ProSelecta is built on [HepMC3](https://gitlab.cern.ch/hepmc/HepMC3) and [cling](https://github.com/root-project/cling) technologies. If you use ProSelecta, please cite both. Find a bibtex entry [here](#citation-helper).

Designed for the [NUISANCE](https://github.com/NUISANCEMC/nuisance) framework.

## Table of Contents

* The Environment
  * Quick Start
  * HepMC3 Types
  * event
  * vect
  * part
* Auxilliary Definitions
  * Units
  * PDG MC Codes
  * Missing Datum
* Community Functions
* The Interpreter

# The ProSelecta Environment

This section provides high-level documentation for the ProSelecta Environment.

## Quick Start

1) Check for and fetch an incoming neutrino of any flavor. 

```c++
if(!ps::event::has_beam_part(evt, ps::pdg::kNeutralLeptons)){
  return false;
}
auto nu = ps::event::beam_part(evt, ps::pdg::kNeutralLeptons);
```

All single particle fetching functions throw if no particle can be found. 
If a ProSelecta function returns a `nullptr`, it should be reported as a bug.

2) Check that the event has at least one incoming muon neutrino and one outgoing muon:
```c++
if(!ps::event::has_beam_part(evt, 14) || !ps::event::has_out_part(evt, 13){
  return false;
}
```

3) Check the final state topology exactly matches: 1mu1p1pi:
```c++
if(!ps::event::out_part_topology_matches(evt, ps::pids{13,2212,211}, {1,1,1})){
  return false;
}
```

4) Get all outgoing protons and sort them by by 3momentum:
```c++
auto protons = ps::event::all_out_part(evt, 2212);
auto protons_sorted = ps::part::sort_ascending(ps::p3mod, protons);
```

Multi-particle search functions can return empty vectors. Accessing an
empty vector is undefined behavior.

5) Get the highest momentum outgoing proton and negative pion
```c++
auto [protons, pims] = ps::event::all_out_part(evt, ps::pids{2212,-211});
auto hmproton = ps::part::highest(ps::p3mod, protons);
auto hmpim = ps::part::highest(ps::p3mod, pims);
```

6) Get the transverse component of the vector sum of the final state muon and all protons
```c++
auto sum_pt = ps::event::sum(ps::momentum, ps::event::all_out_part(evt, ps::pids{13, 2212})).pt();
```

7) Get all protons with more than 0.05 GeV/c but less than 2 GeV/c of 3momentum:
```c++
auto p3mod_cut = (ps::p3mod > 0.05 * ps::unit::GeV)&&(ps::p3mod < 2 * ps::unit::GeV);
auto passing_protons = ps::event::filter(p3mod_cut, ps::event::all_out_part(evt, 2212));
```

8) Get the invariant mass of all final state protons and pions with more than 250 MeV/c of 3momentum:
```c++
auto invmass_protons_and_pions = 
  ps::event::sum(ps::momentum, 
    ps::event::filter(ps::p3mod > 250 * ps::unit::MeV, 
      ps::part::cat(ps::event::all_out_part(evt, ps::pids{2212, 211, -211, 111}))
    )
  ).m();
```

## event

The `ps::event` namespace, defined in [ProSelect/env/event.h](ProSelect/env/event.h) contains functions for getting particles and lists of particles from an event. For many of the functions described below there are convenience overrides for specifying either a list of particle identifiers to search for, or a single one. For example, both of the below are valid checks for whether final-state muons exist in an event. The first only checked for final-state muons, while the second checks for either final-state muons or anti-muons.

```c++
bool has_fs_mu = ps::event::has_out_part(evt, ps::pdg::kMuon);
bool has_fs_mu_or_amu = ps::event::has_out_part(evt, ps::pids{ps::pdg::kMuon, ps::pdg::kAMuon});
```

**N.B.** While it might be tempting to pass a initializer list of pids rather than an initialized instance of the `ps::pids` type, which is just a alias for `std::array<int, N>`, the C++17 template argument deduction cannot pick the right type in this instance and so we need to give the `ps::pids` as a type hint to avoid compiler errors.

For functions that fetch or count particles, passing a list of pids may result in a `std::array` of responses being returned. This can be very powerful, but can also trip you up if you aren't aware. Modifying the above example to count the number of final-state muons or anti-muons, rather than just check for any, we see:

```c++
auto num_fs_mu = ps::event::num_out_part(evt, ps::pdg::kMuon);
auto num_fs_mu_or_amu = ps::event::num_out_part(evt, ps::pids{ps::pdg::kMuon, ps::pdg::kAMuon});
```

The type of `num_fs_mu_or_amu` is `std::array<int,2>` where `num_fs_mu_or_amu[0]` holds the number of final-state muons and `num_fs_mu_or_amu[1]` holds the number of final-state anti-muons. This is useful thanks to [C++17's structured bindings](https://en.cppreference.com/w/cpp/language/structured_binding), which allows the following syntactic sugar, which is similar to python's tuple unpacking:

```c++
auto [num_fs_mu, num_fs_amu] = ps::event::num_out_part(evt, ps::pids{ps::pdg::kMuon, ps::pdg::kAMuon});
```

### Searching for final-state particles

```c++
//Returns true iff the event contains at least one final-state particle of any of the supplied
//  PIDs otherwise, false.
//Convenience overload for passing a single pid exists.
bool has_out_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs);

//Returns true iff the event contains exactly the number of specified final-state particles for
//  each PID otherwise, false. Additional, unspecified PIDs are not checked.
//Convenience overload for passing a single pid and associated count exists.
bool has_exact_out_part(HepMC3::GenEvent const &ev,
                        std::array<int, N> const &PIDs,
                        std::array<int, N> const &counts);

//Returns true iff the event contains exactly the number of specified final-state particles for
// each PID and no additional final state particles otherwise, false.
bool out_part_topology_matches(HepMC3::GenEvent const &ev,
                               std::array<int, N> const &PIDs,
                               std::array<int, N> const &counts);

//Returns true iff the event contains at least the number of specified final-state particles
//  for each PID otherwise, false. Additional, unspecified PIDs are not checked.
bool has_at_least_out_part(HepMC3::GenEvent const &ev,
                           std::array<int, N> const &PIDs,
                           std::array<int, N> const &counts);

//Returns an array of the number of final-state particles for each specified PID.
//Convenience overload for passing a single pid exists and returns an int rather 
//  than an std::array<int,1>.
//If no PIDs are passed, all final-state particles are counted.
auto num_out_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs);

//Returns the number of final-state particles with pid not specified in PIDs.
int num_out_part_except(HepMC3::GenEvent const &ev,
                        std::array<int, N> const &PIDs);

//Returns an array of vectors of GenParticlePtr to final-state particles
// for each specified PID.
//Convenience overload for passing a single pid exists and returns an std::vector rather 
//  than an std::array<std::vector,1>.
//If no PIDs are passed, all final-state particles are returned in a single std::vector.
auto all_out_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs);

//Returns as single std::vector of GenParticlePtr to final-state particles with pid not specified in PIDs.
auto all_out_part_except(HepMC3::GenEvent const &ev,
                         std::array<int, N> const &PIDs);
```

### Searching for beam and target particles

Since the [NuHepMC]() standard considers events with anything other than a single beam and a single target particle invalid, the below functions that fetch particles will throw if they find no matching particles **or** if they find more than one matching particle.

```c++
//Returns true iff the event contains at least one beam particle of any of the supplied
//  PIDs otherwise, false.
//Convenience overload for passing a single pid exists.
bool has_beam_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs);

//Returns a GenParticlePtr to the matching beam particle for any specified PID.
//Convenience overload for passing a single pid exists.
//If no PIDs are passed, all beam particles are fetched and the first one is returned.
//This function will throw if more than one or less than one particle is found in the search
auto beam_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs);

//Returns true iff the event contains at least one target particle of any of the supplied
//  PIDs otherwise, false.
//Convenience overload for passing a single pid exists.
bool has_target_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs);

//Returns a GenParticlePtr to the matching target particle for any specified PID.
//Convenience overload for passing a single pid exists.
//If no PIDs are passed, all target particles are fetched and the first one is returned.
//This function will throw if more than one or less than one particle is found in the search
auto target_part(HepMC3::GenEvent const &ev, std::array<int, N> const &PIDs);
```

### Searching for nuclear particles

The previously detailed functions all ignore particles with pid >= 1000000000 for all searches. If you need to grab these particles you can use the below function that will get all final-state particles from the event with pid >= 1000000000.

```c++
//Returns a vector of GenParticlePtr pointing to final-state particles with pid >= 1000000000
auto out_nuclear_parts(HepMC3::GenEvent const &ev)
```

## part

The `ps::part` namespace, defined in [ProSelect/env/part.h](ProSelect/env/part.h) contains functions for working with `HepMC3::GenParticlePtr`s and collections thereof.

### convenience

The functions below are provided to work with the `std::array<std::vector,N>` return values of `ps::event::all_out_part`.

```c++
//returns exactly one HepMC3::GenParticlePtr from the input array of vectors
//throws if there is not exactly one particle in parts.
auto one(std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> parts);
//returns a single std::vector<HepMC3::GenParticlePtr> containing all HepMC3::GenParticlePtr
// in parts. You can think of this as equivalent to flattening the 2D data structure of parts.
auto cat(std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> parts);
```

As these are a bit more abstract, we will provide some example usage below.

```c++
//throws if there is not exactly one proton
auto the_one_proton = ps::part::one(ps::event::all_out_part(evt,2212));

//throws if there is not exactly one charged pion
auto the_one_charged_pion = ps::part::one(ps::event::all_out_part(evt, ps::pids{211,-211}));

//a single std::vector<HepMC3::GenParticlePtr> of all final-state charged pions found in the event
auto all_the_charged_pions = ps::part::cat(ps::event::all_out_part(evt, ps::pids{211,-211}));
```

### projectors

The rest of `ps::part` is built around the concept of particle projectors. Projectors are objects that can be called on a HepMC3::GenParticlePtr and return some type. 

Some of these projectors, those that project onto a single real number, can be used to define sorting orders and cuts. ProSelecta defines a number of projectors based on natural projections of four-vectors:

```c++
ps::p3mod; //can be used to sort or cut 
ps::energy; //can be used to sort or cut 
ps::theta; //can be used to sort or cut 
ps::costheta; //can be used to sort or cut 
ps::momentum;
```

The projectors are named like the corresponding HepMC3::FourVector methods or properties for clarity and consistency. The `ps::momentum` projector is included for ease of performing 4-vector sums over collections of particles, but it cannot be used to cut or sort particles.

```c++
//Returns the accumulated result of applying vector over all particles in parts
//A convenience overload exists for passing a single vector instead of an array of
//  vectors.
auto sum(T const &projector,
         std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> parts);
```

### sorting

```c++
//sort the input particles in ascending order according to the projector
//throws if parts is an empty vector
auto sort_ascending(T const &projector,
                    std::vector<HepMC3::ConstGenParticlePtr> parts);

//Gets the particle from parts with the highest projected value
//throws if parts is an empty vector
auto highest(T const &projector,
             std::vector<HepMC3::ConstGenParticlePtr> parts);

//Gets the particle from parts with the lowest projected value
//throws if parts is an empty vector
auto lowest(T const &projector,
            std::vector<HepMC3::ConstGenParticlePtr> parts);
```

### cuts

ProSelect provides a very simple cut syntax built on the projector objects. `cuts` are created by using one of the below operators on a valid projector (*i.e.* not `ps::momentum`).

```c++
auto cut1 = ps::p3mod < 10 * ps::unit::GeV;
auto cut2 = ps::p3mod <= 10 * ps::unit::GeV;
auto cut3 = ps::p3mod > 10 * ps::unit::GeV;
auto cut4 = ps::p3mod >= 10 * ps::unit::GeV;
```

`cuts` objects can also be negated and logically and'd, they cannot be logically or'd as building correct cut graphs is outside the scope for this utility and for the simple cuts envisioned and extra line or two of user code is worth keeping the framework code considerably simpler.

```c++
auto cut4not = !cut4;
auto cut1and2 = cut1&&cut2;
```

Importantly, `cuts` can be applied to vectors of particles.


```c++
//returns the vector of particles passing the cuts, c.
//returning an empty vector is not an error unlike elsewhere in this interface
auto filter(cuts const &c, std::vector<HepMC3::ConstGenParticlePtr> parts) 
```


## vect

The `ps::vect` namespace, defined in [ProSelect/env/vect.h](ProSelect/env/vect.h) contains 3- and 4-vector helper functions that provide useful extensions to the HepMC::FourVector class methods. 3-vector functions set HepMC3::FourVector

```c++
//Gets the 3-direction (spatial unit vector) from the input 4-vector, v
HepMC3::FourVector direction(HepMC3::FourVector v);

//Calculates the dot product (2-norm) of the spatial components of a and b
double dot(HepMC3::FourVector const &a, HepMC3::FourVector const &b);

//Calculates the cross product (vector product) of the spatial components of a and b
HepMC3::FourVector cross(HepMC3::FourVector const &a,
                         HepMC3::FourVector const &b);

//Calculates the smallest angle between the spatial components of a and b
double angle(HepMC3::FourVector const &v, HepMC3::FourVector const &refv);

//Calculates the spatial component of v transverse to the  vector, dir
HepMC3::FourVector transverse(HepMC3::FourVector v, HepMC3::FourVector dir);

//Calculates the spatial vector v rotated by theta (deg) around the axis specified by axis
HepMC3::FourVector rotate(HepMC3::FourVector const &v, HepMC3::FourVector axis,
                          double theta);

//Calculates the beta vector from an input 4-momentum vector
HepMC3::FourVector boost_beta(HepMC3::FourVector const &fv);

//Boosts the 4-vector, fv, into the reference frame with relative velocity taken from 
//  the spatial components of boost
//N.B. Do not pass a 4-momentum vector as the boost_beta vector, use ps::vect::boost_beta
HepMC3::FourVector boost(HepMC3::FourVector const &fv,
                         HepMC3::FourVector const &boost_beta);
```

## HepMC3 Types

ProSelecta is built on HepMC3 and so the full set of HepMC3 types can be used in ProSelecta-enabled functions.

# Auxilliary Definitions

## System of Units

This specification place no constraints on the internal units used by a concrete event and particle implementation. However, at least the below constants must be defined in the context of the internal units to allow deterministic conversion to user-specified units.

* Energy/Momentum: `[M,G,k,]eV`
* Mass: `[M,G,k,]eV_c2`
* Angle: `rad, deg`

Units constants can be used with numeric literals for direct comparison to calculated properties from an event or particle, for example:

`part.momentum().p3mod() > 1*unit::GeV`

They can also be used to convert the units of a calculated property, for example:

`part.momentum().p3mod() / unit::GeV`

## PDG MC Codes

## Missing Datum

It is useful to define a magic number to flag missed values, this enables the writing of functions that return/expect optional values. `ps::kMissingDatum` is a contexpr inline variable template that should be used in place of missed values. A definition for `int`s and `double`s are defined for use in ProSelecta functions. For example, a function that gets the energy of incoming muon neutrinos might look like:

```c++
double e_nu_mu_GeV(HepMC3::GenEvent const &evt){
  if(!ps::event::has_beam_part(evt, ps::pdg::kNuMu)){
    return ps::kMissingDatum<double>;
  }
  return ps::event::beam_part(evt, ps::pdg::kNuMu)->momentum().e() / unit::GeV;
}
```

This paradigm facilitates downstream code building columnar analyses without needing consistent nullable type handling.

# Community Functions

Community functions are auxilliary helper functions that the ProSelecta environment exposes, but come with fewer guarantees of robustness. They are provided in case they facilitate analysis development and truth studies, but we do not recommend that they are used in data releases as they are not as robustly defined or tested, or guaranteed to exist in ProSelecta for ever, unlike the core set of ProSelecta functions. Communitity functions live in `ProSelecta/ext/` and should be `#include`d in scripts that want to use them.

```c++
#include "ext/scatter.h"
```

Documentation for community functions, where it exists, can be found [here]().

# Citation Helper

If you use ProSelecta in your publication, please cite Cling, HepMC3, and ProSelecta itself.

```tex
%%%% ProSelecta citation when we have one

%%%% cling citation
% Peer-Reviewed Publication
%
% 19th International Conference on Computing in High Energy and Nuclear Physics (CHEP)
% 21-25 May, 2012, New York, USA
%
@inproceedings{Cling,
  author = {Vassilev,V. and Canal,Ph. and Naumann,A. and Moneta,L. and Russo,P.},
  title = {{Cling} -- The New Interactive Interpreter for {ROOT} 6}},
  journal = {Journal of Physics: Conference Series},
  year = 2012,
  month = {dec},
  volume = {396},
  number = {5},
  pages = {052071},
  doi = {10.1088/1742-6596/396/5/052071},
  url = {https://iopscience.iop.org/article/10.1088/1742-6596/396/5/052071/pdf},
  publisher = {{IOP} Publishing}
}

%%%% HepMC3 citation
@article{Buckley_2021,
   title={The HepMC3 event record library for Monte Carlo event generators},
   volume={260},
   ISSN={0010-4655},
   url={http://dx.doi.org/10.1016/j.cpc.2020.107310},
   DOI={10.1016/j.cpc.2020.107310},
   journal={Computer Physics Communications},
   publisher={Elsevier BV},
   author={Buckley, Andy and Ilten, Philip and Konstantinov, Dmitri and Lönnblad, Leif and Monk, James and Pokorski, Witold and Przedzinski, Tomasz and Verbytskyi, Andrii},
   year={2021},
   month=mar, pages={107310} }

```