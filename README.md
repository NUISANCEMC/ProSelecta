# ProSelecta

This document corresponds to version 0.1 of the NUISANCE ProSelecta definition.

ProSelecta is a bit like an Analysis Descriptor Language (ADL), except that it doesn't attempt to define a new Domain Specific Language and instead defines a list of utility functions that an event procesing environment might implement to facilitate the declaration of a measurements key steps.

ProSelecta is an abstract specification for a minimal set of utilities to support describing the _event_ selection and projection steps of constructing a prediction of a neutrino-scattering measurement in a declarative way. 

An implementation of the ProSelecta environment will have to choose a specific language, event format, and <>

Designed for the [NUISANCE](https://github.com/NUISANCEMC/nuisance) framework.

## Table of Contents

# First Off, Some Physics

## The Neutrino Interaction

## The Event Object

## System of Units

This specification place no constraints on the internal units used by a concrete event and particle implementation. However, at least the below constants must be defined in the context of the internal units to allow deterministic conversion to user-specified units.

* Energy/Momentum
  - MeV
  - GeV
* Mass
  - MeV_c2
  - GeV_c2
* Angle
  - rad
  - deg

# The ProSelecta Environment

## Notation

We need to define a notational convention that can be used to describe function inputs and outputs in a language agnostic way. We adopt something similar to C++ for familiarity.

### Types

The ProSelecta type system is defined below:

* `bool`
* `real`
* `PID`: An integer identifier that specifies particle species. See [PDG 2023](https://pdg.lbl.gov/2023/mcdata/mc_particle_id_contents.html).
* `3vec`: A 3-vector 
* `particle`
* `event`
* `list<T>`: A generic container of a single, specified type, _e.g._ `list<particle>`.

### Function Declarations

All ProSelecta functions must be pure functions. Pure functions process inputs to outputs in a deterministic way and cannot have side effects or static state. This implies that a ProSelecta function will always return the same output for the same inputs. In languages that have the concept of `const`ness, ProSelecta function arguments should always be considered `const`.

Functions declarations look like:

```MyFunction(event) -> list<particle>```

which describes a function, `MyFunction`, which takes an `event` as input and returns a `list` of `particle`s.

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

```
Proj_q0(particle, particle) -> real
Proj_q3(particle, particle) -> real
Proj_Q2Lep(particle, particle) -> real

Proj_Theta(particle, particle) -> real

Proj_W(list<particles>) -> real
Proj_Pt(list<particles>) -> real
Proj_Pmiss(list<particles>) -> real
```

## Reference Implementations

This package provides the ProSelectaCPP reference implementation that can be used to process HepMC3 events and then two sets of additional language bindings. Each comes with additional documentation and concrete examples of writing selection and projection operators in each environment.

* [ProSelectaCPP](Implementations/ProSelectaCPP/README.md)
* [pyProSelecta](Implementations/pyProSelecta/README.md)
* [ProSolecta](Implementations/ProSolecta/README.md)