# ProSelecta

This document corresponds to version 1 of the NUISANCE ProSelecta definition.

ProSelecta is a bit like an Analysis Descriptor Language (ADL), except that it doesn't attempt to define a new Domain Specific Language and instead defines a list of utility functions that an event procesing environment might implement to facilitate the declaration of a measurement's key steps.

ProSelecta is an specification for a minimal set of utilities to support describing the _event_ selection and projection steps of constructing a prediction of a neutrino-scattering measurement in a declarative way.

ProSelecta is an implementation of this specification in C++ and using HepMC3 as the event format. To avoid needless code duplication, some of the HepMC3 event model is included as part of ProSelecta. While this limits the purity and generality of ProSelecta, we think that it is the right choice practicaly.

ProSelecta is built on [HepMC3](https://gitlab.cern.ch/hepmc/HepMC3) and [cling](https://github.com/root-project/cling) technologies. If you use ProSelecta, please cite all three. Find a bibtex entry [here](#citation-helper).

Designed for the [NUISANCE](https://github.com/NUISANCEMC/nuisance) framework.

## Table of Contents

* Quick Start
* Types
* The Environment
  * event
  * part
  * vect
* Auxilliary Definitions
  * unit
  * pid
  * missing_datum
* Examples
* Community Functions

# Quick Start

Check for and fetch an incoming neutrino of any flavor
```c++
auto nu = ps::event::beam_part(evt, ps::pdg::kNeutralLeptons);
```

Check that event has at least one incoming muon neutrino and one outgoing muon:
```c++
bool has_incoming_numu = ps::event::has_beam_part(evt, 14);
bool has_final_state_proton = ps::event::has_out_part(evt, 13);
```

Check the final state topology exactly matches: 1mu1p1pi:
```c++
bool is_1mu1p1pi = ps::event::out_part_topology_matches(evt, ps::pids{13,2212,211}, {1,1,1});
```

Get all outgoing protons, ordered by momentum
```c++
auto protons = ps::event::all_out_part(evt, 2212);
auto protons_sorted = ps::part::sort_ascending(ps::p3mod, protons);
```

Get the highest momentum outgoing proton and negative pion
```c++
auto [protons, pims] = ps::event::all_out_part(evt, ps::pids{2212,-211});
auto hmproton = ps::part::highest(ps::p3mod, protons);
auto hmpim = ps::part::highest(ps::p3mod, pims);
```

Get the transverse component of the vector sum of the final state muon and all protons
```c++
auto sum_pt = ps::event::sum(ps::momentum, ps::event::all_out_part(evt, ps::pids{13, 2212})).pt();
```

# Auxilliary Definitions

## HepMC3 Type System

ProSelecta is built on HepMC3

## System of Units

This specification place no constraints on the internal units used by a concrete event and particle implementation. However, at least the below constants must be defined in the context of the internal units to allow deterministic conversion to user-specified units.

* Energy/Momentum: `[M,G,k,]eV`
* Mass: `[M,G,k,]eV_c2`
* Angle: `rad, deg`

Units constants can be used with numeric literals for direct comparison to calculated properties from an event or particle, for example:

`part.momentum().p3mod() > 1*units::GeV`

They can also be used to convert the units of a calculated property, for example:

`part.momentum().p3mod() / units::GeV`

# Community Functions

# The ProSelecta Environment

## Notation

We need to define a notational convention that can be used to describe function inputs and outputs in a language agnostic way. We adopt something similar to C++ for familiarity.

### Types

The ProSelecta type system is defined below:

* `bool`
* `real`
* `PID`: An integer identifier that specifies particle species. See [PDG 2023](https://pdg.lbl.gov/2023/mcdata/mc_particle_id_contents.html).
* `4vec`: A 4-vector 
* `particle`
* `event`
* `list<T>`: A generic container of a single, specified type, _e.g._ `list<particle>`.
* `tuple<T1,T2,...>`: A fixed-length container of multiple, possible heterogenous, specified types, _e.g._ `tuple<particle,4vec>`.

### Function Declarations

All ProSelecta functions must be pure functions. Pure functions process inputs to outputs in a deterministic way and cannot have side effects or static state. This implies that a ProSelecta function will always return the same output for the same inputs. In languages that have the concept of `const`ness, ProSelecta function arguments should always be considered `const`.

Functions declarations look like:

```qual::MyFunction(event) -> list<particle>```

which describes a function, `MyFunction`, in namespace or module or with prefix `qual`, which takes an `event` as input and returns a `list` of `particle`s. Depending on the language the actual function invocation may look like: `qual::MyFunction`, `qual.MyFunction`, or `qual_MyFunction`, see documentation for the concrete implementation for explicit details.

## Selections

```
GetBeam(event, PID) -> particle
GetBeamAny(event, list<PID>) -> particle

GetBeams(event, PID) -> list<particle>
GetBeamsAny(event, list<PID>) -> list<particles>

GetTarget(event) -> particle

GetOutPartFirst(event, PID) -> particle
GetOutPartFirstAny(event, list<PID>) -> particle

GetOutPartHM(event, PID) -> particle
GetOutPartHMAny(event, list<PID>) -> particle

GetOutParts(event, PID) -> list<particles>
GetOutPartsAny(event, list<PID>) -> list<particles>

GetOutPartsExcept(event, PID) -> list<particles>
GetOutPartsExceptAny(event, list<PID>) -> list<particles>

FilterByMomentum(list<particles>, real, real) -> list<particles>
out_nuclear_parts(event) -> list<particles>
```

## Hard-Scatter Channels

Unfortunately, existing measurements require event selection based on the true interaction channel that was simulated. As a result, ProSelecta provides the below functions for use by historic measurement implementations. There is almost no reason for new measurements to be made using these functions.

The details of how these functions are implemented are not specified beyond the intent designated by the name of each function. Given model and simulation implementation differences it is practically impossible to uniquely define be descriptive about the details of such functions in a consistent, useful way.

```
IsCC(event) -> bool
IsCOH(event) -> bool
Is1p1h(event) -> bool
Is2p2h(event) -> bool
IsSPP(event) -> bool
IsRES(event) -> bool
IsDIS(event) -> bool
```

## Projections

### Event-level Projections

```
event::q0(event) -> real
event::q3(event) -> real
event::Q2Lep(event) -> real
event::CosThetaLep(event) -> real
event::ThetaLep(event) -> real
```

### Particle-level Projections

The above event-level projections use some of the Particle [Selections](#selections) with default PID lists to grab the most likely particles that a user would like to use to calculate various projections. If you need more control you can use the `parts` versions of these functions.

Additionally other useful projections are defined that we do not feel confident pre-deciding which particles a user may want included.

```
parts::q0(particle, particle) -> real
parts::q3(particle, particle) -> real
parts::Q2(particle, particle) -> real

parts::CosTheta(particle, particle) -> real
parts::Theta(particle, particle) -> real

parts::W(list<particles>) -> real
parts::EPmiss(list<particles>) -> 4vec
```

## Reference Implementation

This package provides the [ProSelectaCPP](README.md) reference implementation that can be used to process HepMC3 events in the ProSelecta environment with interpreted C++ or python processing functions.

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