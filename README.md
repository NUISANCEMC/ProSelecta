# ProSelecta

This document corresponds to version 1 of the NUISANCE ProSelecta definition.

ProSelecta is a bit like an Analysis Descriptor Language (ADL), except that it doesn't attempt to define a new Domain Specific Language. Instead defines a list of utility functions (_the environment_) that an event processing framework might implement to facilitate the declaration of a measurement's key steps.

ProSelecta is also a header-only implementation of these utility functions in C++ and using [HepMC3](https://doi.org/10.1016/j.cpc.2020.107310) as the event format. To avoid needless code duplication, some of the HepMC3 event model is included as part of ProSelecta. While this limits the purity and generality of ProSelecta, we think that it is the right choice practically.

ProSelecta is also also a library providing an interface for writing dynamic event processors that JITs C++ source files written against the ProSelecta environment and exposes functions defined within the JIT'd source as readily callable for event processing.

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
projection = pps.project.get("Enu_GeV") #get a python handle to the C++ projection function

for event in hepmc3_file: #don't ask how we got this file
  if selection(event):
    enu = projection(event)
```

This example is somewhat contrived and not particularly efficient, but hopefully it highlights how a robust event processing environment, coupled to a JIT runtime can enable a range of dynamic, automated workflows.

## Attribution

ProSelecta is built on [HepMC3](https://gitlab.cern.ch/hepmc/HepMC3) and [cling](https://github.com/root-project/cling) technologies. If you use ProSelecta, please cite both. Find a bibtex entry [here](#citation-helper).

Designed for the [NUISANCE](https://github.com/NUISANCEMC/nuisance) framework.

## Table of Contents

* [The Environment](#the-proselecta-environment)
  * [Quick Start](#quick-start)
  * [ProSelecta Snippets](#proselecta-snippets)
  * [event](#event)
  * [vect](#vect)
  * [part](#part)
  * [HepMC3 Types](#hepmc3-types)
* [Auxilliary Definitions](#auxilliary-defintions)
  * [Units](#units)
  * [PDG MC Codes](#pdg-mc-codes)
  * [Missing Datum](#missing-datum)
* [Community Functions](#community-functions)
* [The Interpreter](#the-interpreter)
  * [ProSelecta Function Types](#proselecta-function-types)
* [Python Bindings](#python-bindings)
* [Compiling Snippets](#compiling-snippets)
* [FAQs and Common Issues](#faqs-and-common-issues)
* [Appendices](#appendices)
  * [Citation Helper](#citation-helper)

# The ProSelecta Environment

This section provides high-level documentation for the ProSelecta Environment.

## Quick Start

Below are some examples, in increasing complexity, to showcase the expected use cases of ProSelecta.

1) Check for and fetch an incoming neutrino of any flavor. 

```c++
if (!ps::event::has_beam_part(evt, ps::pdg::kNeutralLeptons)) {
  return false;
}
HepMC3::ConstGenParticlePtr nu =
    ps::event::beam_part(evt, ps::pdg::kNeutralLeptons);
```

All single particle fetching functions throw if no particle can be found. 
If a ProSelecta function returns a `nullptr`, it should be reported as a bug.

2) Check that the event has at least one incoming muon neutrino and one outgoing muon:
```c++
if (!ps::event::has_beam_part(evt, 14) || !ps::event::has_out_part(evt, 13)) {
  return false;
}
```

3) Check the final state topology exactly matches: 1mu1p1pi:
```c++
if (!ps::event::out_part_topology_matches(evt, ps::pids(13, 2212, 211),
                                          {1, 1, 1})) {
  return false;
}
```

4) Get all outgoing protons and sort them by 3-momentum magnitude:
```c++
std::vector<HepMC3::ConstGenParticlePtr> protons =
    ps::event::all_out_part(evt, 2212);
auto protons_sorted = ps::part::sort_ascending(ps::p3mod, protons);
```

Multi-particle search functions can return empty vectors. Accessing an
empty vector is undefined behavior. Look before you leap.

5) Get the highest momentum outgoing proton and negative pion:
```c++
auto [hmproton, hmpim] = ps::event::hm_out_part(evt, ps::pids(2212, -211));
```

6) Get the transverse component of the vector sum of the final state muon and all protons:
```c++
auto sum_pt =
    ps::part::sum(ps::momentum, ps::event::all_out_part(
                                    evt, ps::pids(13, 2212), ps::squeeze))
        .pt() /
    ps::unit::GeV_c;
```

7) Get all protons with more than 0.05 GeV/c but less than 2 GeV/c of 3momentum:
```c++
auto p3mod_cut =
    (ps::p3mod > 0.05 * ps::unit::GeV) && (ps::p3mod < 2 * ps::unit::GeV);
auto passing_protons =
    ps::part::filter(p3mod_cut, ps::event::all_out_part(evt, 2212));
```

8) Get the invariant mass of all final state protons and pions with more than 250 MeV/c of 3momentum:
```c++
auto invmass_protons_and_pions =
    ps::part::sum(ps::momentum,
                  ps::part::filter(
                      ps::p3mod > 250 * ps::unit::MeV,
                      ps::event::all_out_part(
                          evt, ps::pids(2212, 211, -211, 111), ps::squeeze)))
        .m();
```

## ProSelecta Snippets

*Snippets* are what we call C++ source files that contain one or more functions defined against the ProSelect environment. These snippets will often be used to define the selection and projection operations required to turn a stream of simulated HEP events into the building blocks of a measurement prediction. We do provide some strong recommendations for writing clear and robust snippet code.

For the majority of use cases, using cling to JIT snippet source code and execute functions via cling is the most flexible and the preferred method of using ProSelecta-enabled code. However, we do envision the need to compile snippets into shared libraries for some use cases. See [Compiling Snippets](#compiling-snippets) for tips and tools supporting this workflow.

### A Note on Namespaces

ProSelecta makes quite heavy use of namespacing to separate parts of the environment. While this is useful for many reasons, it can lead to what looks like a lot of 'qualifying' boiler plate. You may be tempted to use a `using namespace` declaration to import the namespaces into scope, and while we do recommend doing so, we have some words of caution and one strict requirement.

**Requirement:**
* Never expose any namespace to the global scope in a snippet file, multiple snippet files are likely to be parsed sequentially, and your using directive may change the behavior of someone elses code.

**Recommendations:**
* Don't `using namespace` the `ps::event`, `ps::part`, or `ps::vect` namespaces, it makes the snippet code clearer if the module qualifier is left in place.
* Add `using namespace` declarations for `ps`, `ps::unit`, and `ps::pdg` at the function scope, where required.

In the examples in the rest of this section, the `ps` namespace qualifier is often omitted.

### A Note on Function Names

## `ps::event`

The `ps::event` namespace, defined in [ProSelecta/event.h](env/ProSelecta/event.h) contains functions for getting particles and lists of particles from an event. For many of the functions described below there are convenience overrides for specifying either a list of particle identifiers to search for, or a single one. For example, both of the below are valid checks for whether final-state muons exist in an event. The first only checked for final-state muons, while the second checks for either final-state muons or anti-muons.

```c++
bool has_fs_mu = event::has_out_part(evt, pdg::kMuon);
bool has_fs_mu_or_amu = event::has_out_part(evt, pids(pdg::kMuon, pdg::kAMuon));
```

**N.B.** While it might be tempting to pass a initializer list of pids rather than using the `ps::pids` function, the C++17 template argument deduction cannot pick the right type in this instance and so we need to use the `ps::pids` as a type hint to avoid compiler errors. `ps::pids` is a `constexpr` function returning a fully qualified `std::array` type, and so should not affect runtime performance.

For functions that fetch or count particles, passing a list of pids may result in a `std::array` of responses being returned. This can be very powerful, but can also trip you up if you aren't aware. We can modify the above example to count the number of final-state muons and anti-muons, rather than just check for the existance of either:

```c++
auto num_fs_mu = event::num_out_part(evt, pdg::kMuon);
auto num_fs_mu_or_amu = event::num_out_part(evt, pids(pdg::kMuon, pdg::kAMuon));
```

The type of `num_fs_mu_or_amu` is `std::array<int,2>` where `num_fs_mu_or_amu[0]` holds the number of final-state muons and `num_fs_mu_or_amu[1]` holds the number of final-state anti-muons. This is useful thanks to [C++17's structured bindings](https://en.cppreference.com/w/cpp/language/structured_binding), which allows the following syntactic sugar, and gives us something remarkably similar to python's tuple unpacking:

```c++
auto [num_fs_mu, num_fs_amu] = event::num_out_part(evt, pids(pdg::kMuon, pdg::kAMuon));
```

Finally on this note, sometimes you want to search for particles from a list of multiple pids but then not keep the results separated like above. The `ps::squeeze` optional argument can be used to return a single object from many of the ProSelecta functions. What the returned object is depends on the function called, for the example above, we can count the total number of muons and anti-muons in one call by `squeeze`-ing the result:

```c++
int num_fs_mu_and_amu = event::num_out_part(evt, pids(pdg::kMuon, pdg::kAMuon), ps::squeeze);
``` 

### Searching for final-state particles

#### API Documentation

```c++
// Returns true iff the event contains at least one final-state particle of any
// of the supplied PIDs otherwise, false.
// - Convenience overload for passing a single pid exists.
bool ps::event::has_out_part(HepMC3::GenEvent const &ev,
                             std::array<int, N> const &PIDs);

// Returns true iff the event contains exactly the number of specified
// final-state particles for each PID otherwise, false. Additional, unspecified
// PIDs are not checked.
// - Convenience overload for passing a single pid and associated count exists.
bool ps::event::has_exact_out_part(HepMC3::GenEvent const &ev,
                                   std::array<int, N> const &PIDs,
                                   std::array<int, N> const &counts);

// Returns true iff the event contains exactly the number of specified
// final-state particles for each PID and no additional final state particles
// otherwise, false.
bool ps::event::out_part_topology_matches(HepMC3::GenEvent const &ev,
                                          std::array<int, N> const &PIDs,
                                          std::array<int, N> const &counts);

// Returns true iff the event contains at least the number of specified
// final-state particles for each PID otherwise, false. Additional, unspecified
// PIDs are not checked.
bool ps::event::has_at_least_out_part(HepMC3::GenEvent const &ev,
                                      std::array<int, N> const &PIDs,
                                      std::array<int, N> const &counts);

// Returns an array of the number of final-state particles for each specified
// PID.
// - Passing ps::squeeze as the last parameter will return the total number
//   of final-state particles of all specified PIDs
// - Convenience overload for passing a single pid exists and returns an int
//   rather than an std::array<int,1>.
// - If no PIDs are passed, all final-state particles are counted.
auto ps::event::num_out_part(HepMC3::GenEvent const &ev,
                             std::array<int, N> const &PIDs);

// Returns the number of final-state particles with pid not specified in PIDs.
int ps::event::num_out_part_except(HepMC3::GenEvent const &ev,
                                   std::array<int, N> const &PIDs);

// Returns an array of vectors of GenParticlePtr to final-state particles for
// each specified PID.
// - Passing ps::squeeze as the last parameter will return all matching
//   particles in a single std::vector
// - Convenience overload for passing a single pid exists and returns an
//   std::vector rather than an std::array<std::vector,1>.
// - If no PIDs are passed, all final-state particles are returned in a single
//   std::vector.
auto ps::event::all_out_part(HepMC3::GenEvent const &ev,
                             std::array<int, N> const &PIDs);

// Returns as single std::vector of GenParticlePtr to final-state particles with
// pid not specified in PIDs.
auto ps::event::all_out_part_except(HepMC3::GenEvent const &ev,
                                    std::array<int, N> const &PIDs);

// Returns an array of GenParticlePtr to the highest momentum final-state
// particles for each specified PID.
// - Passing ps::squeeze as the last parameter will return only the highest
//   momentum particle found over all specified PIDs.
// - Convenience overload for passing a single pid exists and returns a
//   GenParticlePtr rather than an std::array<GenParticlePtr,1>.
auto ps::event::hm_out_part(HepMC3::GenEvent const &ev,
                            std::array<int, N> const &PIDs);
```

#### Example Usage

Below are some concrete usage examples for each different version of the function families presented in the previous section. Hopefully it is clear that the framework is quite flexible. That does with some cost, and that cost is that the functions rely quite heavily on template metaprogramming, which cause result in impenetrable compiler errors for seemingly innocuous issues. Hopefully the below examples clearly show how to use these functions correctly, but if you do hit compiler errors that are difficult to parse, first check the [Appendix](#common-compiler-errors) and then reach out on [slack: nuisance-xsec#proselecta](nuisance-xsec.slack.com).

```c++
using namespace ps;
using namespace ps::pdg;

auto has_final_state_muon = event::has_out_part(ev, kMuon);
auto has_final_state_muon_or_antimuon =
    event::has_out_part(ev, pids(kMuon, kAMuon));
auto has_final_state_charged_leptons =
    event::has_out_part(ev, kChargedLeptons);

auto has_2_final_state_protons = event::has_exact_out_part(ev, kProton, 2);
auto has_2_final_state_protons_and_1_muon =
    event::has_exact_out_part(ev, pids(kProton, kMuon), {2, 1});

auto final_state_only_1mu2p3pip = event::out_part_topology_matches(
    ev, pids(kMuon, kProton, kPiPlus), {1, 2, 3});

auto final_state_more_than_2_protons =
    event::has_at_least_out_part(ev, kProton, 2);
auto final_state_more_than_2_protons_and_neutrons =
    event::has_at_least_out_part(ev, pids(kProton, kNeutron), {2, 2});

auto num_final_state_protons = event::num_out_part(ev, kProton);
auto [num_fs_protons, num_fs_neutrons] =
    event::num_out_part(ev, pids(kProton, kNeutron));
auto num_fs_protons_and_neutrons] = 
    event::num_out_part(ev, pids(kProton,kNeutron), ps::squeeze);

auto num_exotic = event::num_out_part_except(
    ev, pids(kMuon, kProton, kNeutron, kPiPlus,
             kPiZero, kPiMinus, kGamma));

auto all_protons = event::all_out_part(ev, kProton);
auto [all_protons, all_pims] =
    event::all_out_part(ev, pids(kProton, kPiMinus));
auto all_protons_and_pims =
    event::all_out_part(ev, pids(kProton, kPiMinus), ps::squeeze);

auto all_not_protons = event::all_out_part_except(ev, kProton);
auto all_exotic = event::all_out_part_except(
    ev, pids(kMuon, kProton, kNeutron, kPiPlus,
             kPiZero, kPiMinus, kGamma));

auto hm_proton = event::hm_out_part(ev, kProton);
auto [hm_proton, hm_pi0] =
    event::hm_out_part(ev, pids(kProton, kPiZero));
auto hm_proton_or_pi0 =
    event::hm_out_part(ev, pids(kProton, kPiZero), ps::squeeze);
```


### Searching for beam and target particles

Since the [NuHepMC](https://arxiv.org/pdf/2310.13211) standard considers events with anything other than a single beam and a single target particle invalid, the below functions that fetch particles will throw if they find no matching particles **or** if they find more than one matching particle.

```c++
// Returns true iff the event contains at least one beam particle of any of the
// supplied PIDs otherwise, false.
// - Convenience overload for passing a single pid exists.
bool ps::event::has_beam_part(HepMC3::GenEvent const &ev,
                              std::array<int, N> const &PIDs);

// Returns a GenParticlePtr to the matching beam particle for any specified PID.
// - Convenience overload for passing a single pid exists.
// - If no PIDs are passed, all beam particles are fetched and the first one is
//   returned.
// - This function will throw if more than one or less than one particle is
//   found in the search
auto ps::event::beam_part(HepMC3::GenEvent const &ev,
                          std::array<int, N> const &PIDs);

// Returns true iff the event contains at least one target particle of any of
// the supplied PIDs otherwise, false.
// - Convenience overload for passing a single pid exists.
bool ps::event::has_target_part(HepMC3::GenEvent const &ev,
                                std::array<int, N> const &PIDs);

// Returns a GenParticlePtr to the matching target particle for any specified
// PID.
// - Convenience overload for passing a single pid exists. If no PIDs are
//   passed, all target particles are fetched and the first one is returned.
// - This function will throw if more than one or less than one particle is
//   found in the search
auto ps::event::target_part(HepMC3::GenEvent const &ev,
                            std::array<int, N> const &PIDs);
```

### Searching for nuclear particles

The previously detailed functions all ignore particles with pid >= 1000000000 for all searches. If you need to grab these particles you can use the below function that will get all final-state particles from the event with pid >= 1000000000.

```c++
// Returns a vector of GenParticlePtr pointing to final-state particles with 
// pid >= 1000000000
auto ps::event::out_nuclear_parts(HepMC3::GenEvent const &ev)
```

### misc

```c++
// Returns the signal_process_id for the event, throws if the attribute does 
// not exist
int ps::event::signal_process_id(HepMC3::GenEvent const &ev);
```

## part

The `ps::part` namespace, defined in [ProSelecta/part.h](env/ProSelecta/part.h) contains functions for working with `HepMC3::GenParticlePtr`s and collections thereof.

### projectors

`ps::part` is built around the concept of particle projectors. Projectors are objects that can be called on a HepMC3::GenParticlePtr and return some calculated object, usually a number representing the value of a physical property. Those that project onto a single real number, can be used to define particle sorting orders and particle cuts. ProSelecta defines a number of projectors based on natural projections of four-vectors:

```c++
ps::p3mod; //can be used to sort or cut 
ps::energy; //can be used to sort or cut 
ps::kinetic_energy; //can be used to sort or cut 
ps::theta; //can be used to sort or cut 
ps::costheta; //can be used to sort or cut 
ps::momentum;
```

The projectors are named like the corresponding HepMC3::FourVector methods or properties for clarity and consistency. The `ps::momentum` projector is included for ease of performing 4-vector sums over collections of particles, but it cannot be used to cut or sort particles.

The two angular projectors default to taking the angle with respect to the z-axis, but can be passed a reference vector to use instead. Commonly MCEGs will fire the beam particle along the z-axis, but not always, and this assumption should generally be avoided in general code, instead a new projector instances can be made from the original instance and a `HepMC3::FourVector`, like so:

```c++
auto beamdir = beam_part(ev)->momentum();
auto angle_wrt_beam = theta(beamdir);
auto cosangle_wrt_beam = costheta(beamdir);
```

Be careful to pass a vector and not a particle, or the returned object will be the projection of that particle, rather than another projector object using the profferred momentum direction as the reference vector.

Any of these projectors can be use with `part::sum`, which generally just passes it's arguments to `std::accumulate`.

```c++
// Returns the accumulated result of applying vector over all particles in parts
//  - A convenience overload exists for passing a single vector instead of an
//    array of vectors.
//  - Passing ps::squeeze as the last parameter will return only the highest
//    momentum particle found over all specified PIDs.
auto ps::part::sum(
    T const &projector,
    std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> parts);
```

#### Example Usage

```c++
using namespace ps;
using namespace ps::pdg;

auto sum_T_p = part::sum(kinetic_energy, event::all_out_part(ev, kProton));
auto [sum_T_p_alt, sum_T_pi0] =
    part::sum(kinetic_energy, event::all_out_part(ev, pids(kProton, kPiZero)));
auto sum_p_p_and_pi0 = part::sum(
    p3mod, event::all_out_part(ev, pids(kProton, kPiZero), ps::squeeze));
// equivalent to
auto sum_p_p_and_pi0_alt = part::sum(
    p3mod, event::all_out_part(ev, pids(kProton, kPiZero)), ps::squeeze);
```

### sorting

```c++
// Sort each vector of input particles in ascending order according to the projector
// - A convenience overload exists for passing a single vector instead of an
//   array of vectors.
// - Passing ps::squeeze as the last parameter will sort all input particles
//   together according to the result of projector and will return a single 
//   vector of particles
auto ps::part::sort_ascending(T const &projector,
    std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> parts);

// Gets the particle from each vector in parts with the highest projected value
// - Throws if any vector in parts is empty
// - A convenience overload exists for passing a single vector instead of an
//   array of vectors.
// - Passing ps::squeeze as the last parameter will sort all input particles
//   together according to the result of projector and will return a single 
//   particle
auto ps::part::highest(T const &projector,
    std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> parts);

// Gets the particle from each vector in parts with the lowest projected value
// - Throws if any vector in parts is empty
// - Passing ps::squeeze as the last parameter will sort all input particles
//   together according to the result of projector and will return a single 
//   particle
auto ps::part::lowest(T const &projector,
    std::array<std::vector<HepMC3::ConstGenParticlePtr>, N> parts);
```

#### Example Usage

```c++
using namespace ps;
using namespace ps::pdg;

auto protons_orderby_p3mod =
    part::sort_ascending(p3mod, event::all_out_part(ev, kProton));
auto [protons_orderby_p3mod_alt, pims_orderby_p3mod] = part::sort_ascending(
    p3mod, event::all_out_part(ev, pids(kProton, kPiMinus)));
auto protons_and_pims_orderby_p3mod = part::sort_ascending(
    p3mod, event::all_out_part(ev, pids(kProton, kPiMinus), ps::squeeze));
// equivalent to
auto protons_and_pims_orderby_p3mod_alt = part::sort_ascending(
    p3mod, event::all_out_part(ev, pids(kProton, kPiMinus)), ps::squeeze);

auto hm_proton = part::highest(p3mod, event::all_out_part(ev, kProton));
auto [hm_proton_alt, hm_pim] =
    part::highest(p3mod, event::all_out_part(ev, pids(kProton, kPiMinus)));
auto hm_proton_or_pim = part::highest(
    p3mod, event::all_out_part(ev, pids(kProton, kPiMinus), ps::squeeze));
// equivalent to
auto hm_proton_or_pim_alt = part::highest(
    p3mod, event::all_out_part(ev, pids(kProton, kPiMinus)), ps::squeeze);

auto pnu = event::beam_part(ev, kNeutralLeptons)->momentum();
auto angle_with_pnu = ps::theta(pnu);

auto most_forward_proton =
    part::lowest(angle_with_pnu, event::all_out_part(ev, kProton));
auto [most_forward_proton_alt, most_forward_pim] = part::lowest(
    angle_with_pnu, event::all_out_part(ev, pids(kProton, kPiMinus)));
auto most_forward_proton_or_pim =
    part::lowest(angle_with_pnu,
                 event::all_out_part(ev, pids(kProton, kPiMinus), ps::squeeze));
// equivalent to
auto most_forward_proton_or_pim_alt =
    part::lowest(angle_with_pnu,
                 event::all_out_part(ev, pids(kProton, kPiMinus)), ps::squeeze);
```

### cuts

ProSelecta provides a very simple cut syntax built with the projector objects. `ps::cuts` are created by using one of the below operators on a valid projector (*i.e.* not `ps::momentum`). See [System of Units](#system-of-units) for discussion on the unit constants used below.

```c++
auto cut1 = p3mod < 10 * unit::GeV;
auto cut2 = p3mod <= 10 * unit::GeV;
auto cut3 = p3mod > 10 * unit::GeV;
auto cut4 = p3mod >= 10 * unit::GeV;
```

`ps::cuts` objects can also be negated and logically and'd. They cannot be logically or'd as building correct cut execution graphs is outside the scope of this utility and ,for the simple cuts envisioned, an extra line or two of user code is worth keeping the framework code considerably simpler.

```c++
auto cut4not = !cut4;
auto cut1and2 = cut1&&cut2;
```

Importantly, `ps::cuts` can be applied to vectors of particles.

```c++
// Returns the vector of particles passing the cuts, c.
// - A convenience overload exists for passing a single vector instead of an
//   array of vectors.
// - Passing ps::squeeze as the last parameter will sort all input particles
//   together according to the result of projector and will return a single 
//   particle
auto ps::part::filter(ps::cuts const &c, std::vector<HepMC3::ConstGenParticlePtr> parts) 
```

#### Example Usage

```c++
using namespace ps;
using namespace ps::unit;
using namespace ps::pdg;

auto protons_in_range = part::filter(p3mod > 0.1 * GeV && p3mod < 1.5 * GeV,
                                     event::all_out_part(ev, kProton));
auto [protons_in_range, pim_in_range] =
    part::filter(p3mod > 0.1 * GeV && p3mod < 1.5 * GeV,
                 event::all_out_part(ev, pids(kProton, kPiMinus)));
auto protons_and_pim_in_range =
    part::filter(p3mod > 0.1 * GeV && p3mod < 1.5 * GeV,
                 event::all_out_part(ev, pids(kProton, kPiMinus), ps::squeeze));
// equivalent to
auto protons_and_pim_in_range_alt =
    part::filter(p3mod > 0.1 * GeV && p3mod < 1.5 * GeV,
                 event::all_out_part(ev, pids(kProton, kPiMinus)), ps::squeeze);
```

## `ps::vect`

The `ps::vect` namespace, defined in [ProSelecta/vect.h](env/ProSelecta/vect.h) contains 3- and 4-vector helper functions that provide useful extensions to the `HepMC::FourVector` class methods. 3-vector functions set `HepMC3::FourVector::m_4v` to zero. This module contains no templates and is generally a lot simpler than the previous modules.

```c++
// Gets the 3-direction (spatial unit vector) from the input 4-vector, v
HepMC3::FourVector direction(HepMC3::FourVector v);

// Calculates the dot product (2-norm) of the spatial components of a and b
double dot(HepMC3::FourVector const &a, HepMC3::FourVector const &b);

// Calculates the cross product (vector product) of the spatial components of a
// and b
HepMC3::FourVector cross(HepMC3::FourVector const &a,
                         HepMC3::FourVector const &b);

// Calculates the smallest angle between the spatial components of a and b
double angle(HepMC3::FourVector const &v, HepMC3::FourVector const &refv);

// Calculates the spatial component of v transverse to the  vector, dir
HepMC3::FourVector transverse(HepMC3::FourVector v, HepMC3::FourVector dir);

// Calculates the spatial vector v rotated by theta (deg) around the axis
// specified by axis
HepMC3::FourVector rotate(HepMC3::FourVector const &v, HepMC3::FourVector axis,
                          double theta);

// Calculates the beta vector from an input 4-momentum vector
HepMC3::FourVector boost_beta(HepMC3::FourVector const &fv);

// Boosts the 4-vector, fv, into the reference frame with relative velocity
// taken from the spatial components of boost
// - N.B. Do not pass a 4-momentum vector as the boost_beta vector, use
//   ps::vect::boost_beta
HepMC3::FourVector boost(HepMC3::FourVector const &fv,
                         HepMC3::FourVector const &boost_beta);
```

## HepMC3 Types

ProSelecta is built on HepMC3 and so the full set of HepMC3 types can be used in ProSelecta-enabled functions. It is worth noting that the main ProSelecta environment deals with 'real' or 'observable' particles, though with status, 11, 4, or 1. For many MC studies or probing generator predictions it can be important to examine the internal details of the event graph, such as studying hadrons produced in a primary process scattering through some medium on their way to the detector. You can use the full HepMC3 API to process the event graph in ProSelecta snippets and extract any available information. However, when using ProSelecta to describe the selection and projection operations for comparing to real measurements, we only expect those selections and projections to be codified in terms of observable particles, hence the focus of the functions defined.

# Auxilliary Definitions

## System of Units

The follow simple system of units is defined in [ProSelecta/unit.h](env/ProSelecta/unit.h) under the `ps::unit` namespace. These enable projected event properties to be converted to a specified unit and absolute cut values to be defined.

* Energy: `ps::unit::[M,G,k,]eV`
* Momentum: `ps::unit::[M,G,k,]eV_c`
* Mass: `ps::unit::[M,G,k,]eV_c2`
* Spacetime interval: `ps::unit::[M,G,k,]eV2`
* Angle: `ps::unit::rad, ps::unit::deg`

User-defined literal operators are also defined under the `ps` namespace which allows a slightly simpler more concise syntax for literals with units. You must have a `using namespace ps;` declaration at the relevant scope to use these literal operators.

```c++
using namespace ps;

auto five_MeV = 5_MeV;
auto one_eighty_deg = 180_deg;
```

Units should be used with numeric literals for direct comparison to calculated properties from an event or particle, for example:

```c++
part.momentum().p3mod() > 1_GeV
```

The constants can also be used to convert the units of a calculated property, for example:

```c++
part.momentum().p3mod() / ps::unit::GeV
```

**N.B** that when expressing an event in some unit, you always *divide* by the constant, and when specifying a literal in some unit, you *multiply* by the constant (or use a user-defined literal operator).

## PDG MC Codes

A number of pid constants are defined in [ProSelecta/pdg.h](env/ProSelecta/pdg.h) according to the [PDG MC particle numbering scheme](https://pdg.lbl.gov/2020/reviews/rpp2020-rev-monte-carlo-numbering.pdf). Groups of related particles are provides as `std::arrays` for  convenient use with particle search functions in the `ps::event` namespace. Some examples are given below:

```c++
ps::pdg::kNuE = 12;
ps::pdg::kANuMu = -14;
ps::pdg::kElectron = 11;
ps::pdg::kProton = 2212;
ps::pdg::kChargedLeptons = {kElectron, kAElectron, kMuon, kAMuon};
ps::pdg::kNeutralLeptons_matter = {kNuE, kNuMu};
```

## Missing Datum

It is useful to define a magic number to flag missed values, this enables the writing of functions that return/expect optional values. `ps::kMissingDatum` is a contexpr inline variable template that should be used in place of missed or uncalculabe values. A definition for `int`s and `double`s are defined for use in ProSelecta functions. For example, a function that gets the energy of incoming muon neutrinos might look like:

```c++
double e_nu_mu_GeV(HepMC3::GenEvent const &evt){
  using namespace ps;
  using namespace ps::pdg;
  using namespace ps::unit;

  if(!event::has_beam_part(evt, kNuMu)){
    return kMissingDatum<double>;
  }
  return event::beam_part(evt, kNuMu)->momentum().e() / GeV;
}
```

This paradigm facilitates downstream columnar analyses without needing consistent nullable type handling.

# Community Functions

Community functions are auxilliary helper functions that the ProSelecta environment exposes, but come with fewer guarantees of robustness. They are provided in case they facilitate analysis development and truth studies, but we do not recommend that they are used in data releases as they are not as robustly defined or tested, or guaranteed to exist in ProSelecta for ever, unlike the core set of ProSelecta functions. Communitity functions live in `ProSelecta/ext/` and should be `#include`d in scripts that want to use them.

```c++
#include "ext/event_proj.h"
```

Documentation for community functions, where it exists, can be found in [CommunityFunctions.md](CommunityFunctions.md).

# The Interpreter

ProSelecta provides an interpreter based on `cling` which can be used to JIT C++ source containing functions written against the ProSelecta environment and then provide type-checked access to those functions. This enables the writing of fully dynamic event processing frameworks that can apply selection and projection operators defined in externally-provided source files on input event vectors.

The facilities provided by the ProSelecta Interpreter are quite minimal. The Proselecta environment is included before any proffered source is forwarded to the interpreter so that environment boiler plate in user scripts can be kept to a minimum.

Snippets are passed to the interpreter via the `ps::ProSelecta::load_file` method. Since ROOT manages a global cling instance, and there is no real need for thread-safety in interactions with the interpreter, the `ps::ProSelecta` interface is exposed as a singleton instance. To pass a file at location `path/to/file.cxx` to the interpreter, you would use: `ps::ProSelecta::Get.load_file("path/to/file.cxx")`. This function returns a boolean indicating whether the interpreter successfully JIT'd the file. During JITing, the error stream from cling is printed to stderr, so users should expect to see real compiler errors if their snippets are not compileable. 

Re-`load_file`ing the same file at the same path will generally trigger cling to unload the previous symbols and allow them to be replaced with the new versions. This sometimes fails and sometimes the best way to load a corrected snippet is to restart the process using ProSelecta.

## ProSelecta Function Types

We limit the signatures of functions that can be retrieved from the interpreter via the ProSelecta interface. This allows us to do type-checking and significantly reduce the scope for hard-to-debug errors from calling JIT'd symbols incorrectly. The only valid function types that can be retrieved are defined in [src/ProSelecta/FuncTypes.h](src/ProSelecta/FuncTypes.h) and examples are given below:

```c++
int my_selection_func(HepMC3::GenEvent const&);
std::vector<int> my_selections_func(HepMC3::GenEvent const&);

double my_projections_func(HepMC3::GenEvent const&);
std::vector<double> my_projections_func(HepMC3::GenEvent const&);
```

We expect that the vast majority of users will only use functions that return a single projection or selection operator, but we envision use cases where intermediate calculations are computationally expensive and it is significantly more efficient to return vectors of selection and projection results. The `HepMC3::GenEvent -> double/std::vector<double>` signatures are also used for 'weighting' functions, but the type-checking is identical.

**N.B.** A common problem that may result in confusing errors for new users it to forget the `const &` part of the `HepMC3::GenEvent const&` argument. As CV qualifiers and the reference is part of the type, which is part of the function signature, if the functions defined in the snippet exclude these, then they will not be accesible from the interpreter, even if `ProSelecta::load_file` successfully JIT'd the code.

To retrieve callable handles to the above functions from ProSelecta after passing the relevant source to the interpreter, you would use the following code:

```c++
  auto selfunc = ps::ProSelecta::Get().GetSelectFunction("my_selection_func");
  auto selsfunc = ps::ProSelecta::Get().GetSelectsFunction("my_selections_func");

  auto projfunc = ps::ProSelecta::Get().GetProjectionFunction("my_projections_func");
  auto projsfunc = ps::ProSelecta::Get().GetProjectionsFunction("my_projections_func");

```

## An Example Event Processor

A complete example of a program that applys a selection and projection on an input event vector is included below.

```c++

#include "ProSelecta/ProSelecta.h"
#include "HepMC3/deduce_reader.h"

#include <iostream>

//arguments <inputsrc.cpp> <sel_func_name> <proj_func_name> <inputevts.hepmc>
int main(int argc, char const *argv[]){

  ps::ProSelecta::Get().load_file(argv[1]);
  auto selfunc = ps::ProSelecta::Get().GetSelectFunction(argv[2]);
  auto projfunc = ps::ProSelecta::Get().GetProjectionFunction(argv[3]);

  auto rdr = HepMC3::deduce_reader(argv[4]);
  HepMC3::GenEvent evt;

  int i = 0;
  while(!rdr->failed()){
    rdr->read_event(evt);
    if(rdr->failed()){
      break;
    }

    if(selfunc(evt)){ //if the event passes the selection
      std::cout << i << ", " << projfunc(evt) << std::endl; // print the projection
    }
    i++;
  }
}
```


# Python Bindings

Python bindings are provided for both the ProSelecta environment functions and for writing scripts that make use of the ProSelecta interpreter.

## The Environment

The python interface is purposefully defined to work very similarly to the C++ interface. There are some differences in the implementation, many times the C++ interface uses compile-time-sized `std::arrays` to pass lists of pids to particle selection functions, this can't be replicated in python. However, to the user, it doesn't matter, the python interface takes lists instead of arrays and converts them to `std::vectors` which work with the templated C++ code seamlessly. As a result, swapping between the python interface and the C++ interface *should* be trivial. 

It is worth noting that event-level loops in python will be comparitively slower, however, for prototyping it can often be a lot simpler to tinker with a python implementation and then convert it to C++ for larger runs of the analysis. The homogeneity of the C++ and python interfaces hopefully simplifies that workflow.

The C++ examples shown in [Quick Start](#quick-start) are reproduced below in python.

1) Check for and fetch an incoming neutrino of any flavor. 
```python
from pyProSelecta import event, part, unit, pdg, p3mod, momentum

if not event.has_beam_part(evt, pdg.kNeutralLeptons):
  return False

nu = event.beam_part(evt, pdg.kNeutralLeptons)
```

2) Check that the event has at least one incoming muon neutrino and one outgoing muon:
```python
if not event.has_beam_part(evt, 14) or not event.has_out_part(evt, 13):
  return False
```

3) Check the final state topology exactly matches: 1mu1p1pi:
```python
if not event.out_part_topology_matches(evt, [13, 2212, 211], [1, 1, 1]):
  return False
```

4) Get all outgoing protons and sort them by 3-momentum magnitude:
```python
protons = event.all_out_part(evt, 2212)
protons_sorted = part.sort_ascending(p3mod, protons)
```

5) Get the highest momentum outgoing proton and negative pion:
```python
hmproton, hmpim = event.hm_out_part(evt, [2212, -211])
```

6) Get the transverse component of the vector sum of the final state muon and all protons:
```python
sum_pt = part.sum(momentum, event.all_out_part(
           evt, [13, 2212], squeeze=True)).pt() / unit.GeV_c
```

7) Get all protons with more than 0.05 GeV/c but less than 2 GeV/c of 3momentum:
```python
p3mod_cut = (p3mod > 0.05 * unit.GeV) && (p3mod < 2 * unit.GeV)
passing_protons =
    part.filter(p3mod_cut, event.all_out_part(evt, 2212));
```

8) Get the invariant mass of all final state protons and pions with more than 250 MeV/c of 3momentum:
```python
nvmass_protons_and_pions =
    part.sum(momentum,
                  part.filter(
                      p3mod > 250 * unit.MeV,
                      event.all_out_part(
                          evt, [2212, 211, -211, 111], squeeze=True)))
        .m();

```

for more details, using `help` on the sub modules should provide useful documentation: 

```python
import pyProSelecta as pps

help(pps.event)
```

## The Interpreter

Similarly, the python interface to the interpreter is very similar to the C++ one, as below:

```python
import sys
from pyHepMC3 import HepMC3 as hm
import pyProSelecta as pps

pps.load_file(sys.argv[1])

selfunc = pps.select.get(sys.argv[2])
projfunc = pps.project.get(sys.argv[3])

rdr = hm3.deduce_reader(sys.argv[4])
evt = hm3.GenEvent()

while not rdr.failed():
  rdr.read_event(evt);
  if rdr.failed():
    break

  if selfunc(evt): #if the event passes the selection
    print(f"{i}, ", projfunc(evt)) # print the projection
  i += 1
```

# Compiling Snippets

If you need to compile one or more snippets into a shared library, you can use the [`ProSelectaBuild.py`](app/ProSelectaBuild.py) script to do so. It takes a YAML manifest of snippet files and selection and projection operators to expose and produces a shared object and a C++ header file for use in other code. An example manifest can be found at [examples/example_build_manifest.yml](examples/example_build_manifest.yml), and is reproduced below:

```yaml
- snippet: example_MINERvA_PRL.129.021803.cxx
  functions:
    select:
      - MINERvA_PRL129_021803_SignalDefinition
    project:
      - MINERvA_PRL129_021803_Project_MuonE
      - MINERvA_PRL129_021803_Project_SumTp
      - MINERvA_PRL129_021803_Project_q0QE
```

We have to manually specify functions that we would like to be exposed so that a correct header file can be generated. `pyProSelecta` is used to check that the functions exist in the snippet file and have the correct type and an error will be reported if problems are found. Not every function in the snippet file needs to be exposed - in fact, functions that do not have one of the allowed signatures cannot be exposed by `ProSelectaBuild.py`.

The generated header file only depends on HepMC3, all dependence on the ProSelecta environment is fully encapsulated in the compiled library. Running `ProSelectaBuild.py example_build_manifest.yml myproj`, which references the example snippet, [examples/example_MINERvA_PRL.129.021803.cxx](examples/example_MINERvA_PRL.129.021803.cxx), produces in the following generated header file.

```c++
#include "HepMC3/GenEvent.h"

//START -- Prototypes for example_MINERvA_PRL.129.021803.cxx

//START -- select functions
int MINERvA_PRL129_021803_SignalDefinition(HepMC3::GenEvent const&);
//END   -- select functions

//START -- project functions
double MINERvA_PRL129_021803_Project_MuonE(HepMC3::GenEvent const&);
double MINERvA_PRL129_021803_Project_SumTp(HepMC3::GenEvent const&);
double MINERvA_PRL129_021803_Project_q0QE(HepMC3::GenEvent const&);
//END   -- project functions

//END   -- Prototypes for example_MINERvA_PRL.129.021803.cxx
```

# FAQs and Common Issues

## Interpreter

### Function Not Defined When It Definitely Is

### Common Compiler Errors

## python/Juptyer

### Kernel Crashing In Jupyter During ProSelecta Function Execution

# Appendices

## Citation Helper

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