#!/usr/bin/env python3
import yaml

import pyProSelecta as pps

import sys, os

import subprocess

if len(sys.argv) < 3:
  raise RuntimeError("Excepted 2 arguments: <input_manifest.yml> <outprojectname>")

with open(sys.argv[1]) as manifest:
  data = yaml.safe_load(manifest)

snippet_files = {}

for analysis in data:
  snippet = analysis["snippet"]
  print(f"snippet file: {snippet}")
  if not pps.load_file(snippet):
    raise RuntimeError(f"Failed to parse snippet file {snippet}. See above cling output for errors")

  print(f"  checking select functions")
  funcs = analysis["functions"]

  snippet_files[snippet] = { "select": [], "project": [] }

  for sf in funcs["select"]:
    print(f"    fetching symbol for {sf}")
    if pps.select.get(sf):
      snippet_files[snippet]["select"].append(sf)
    else:
      raise RuntimeError(f"Failed to read selection function: {sf}")

  print(f"  checking project functions")
  for sf in funcs["project"]:
    print(f"    fetching symbol for {sf}")
    if pps.project.get(sf):
      snippet_files[snippet]["project"].append(sf)
    else:
      raise RuntimeError(f"Failed to read projection function: {sf}")

outproj = sys.argv[2]

with open(f"{outproj}.h",'w') as outputh:
  outputh.write('#include "HepMC3/GenEvent.h"\n\n')

  for sn in snippet_files.keys():
    outputh.write(f'//START -- Prototypes for {sn}\n\n')
    outputh.write(f'//START -- select functions\n')
    for sf in snippet_files[sn]["select"]:
      outputh.write(f'int {sf}(HepMC3::GenEvent const&);\n')

    outputh.write(f'//END   -- select functions\n\n')
    outputh.write(f'//START -- project functions\n')
    for sf in snippet_files[sn]["project"]:
      outputh.write(f'double {sf}(HepMC3::GenEvent const&);\n')
    outputh.write(f'//END   -- project functions\n\n')
    outputh.write(f'//END   -- Prototypes for {sn}\n\n')

cproc = subprocess.run(["HepMC3-config", "--includedir"], capture_output=True)
if cproc.returncode != 0:
  print(cproc.stdout.decode("utf-8"))
  print(cproc.stderr.decode("utf-8"))
hepmc3inc = cproc.stdout.decode("utf-8").strip()

cproc = subprocess.run(["HepMC3-config", "--libdir"], capture_output=True)
if cproc.returncode != 0:
  print(cproc.stdout.decode("utf-8"))
  print(cproc.stderr.decode("utf-8"))
hepmc3lib = cproc.stdout.decode("utf-8").strip()

ProSelecta_ROOT = os.environ["ProSelecta_ROOT"]

with open(f"{outproj}.cxx",'w') as outputimpl:
  outputimpl.write(f'#include "{outproj}.h"\n\n')
  outputimpl.write('#include "ProSelecta/env.h"\n\n')

  for sn in snippet_files.keys():
    with open(sn,'r') as snf:
      outputimpl.write(f"//START -- imported from {sn}\n")
      for line in snf:
        outputimpl.write(line)
      outputimpl.write(f"\n//END -- imported from {sn}\n\n")

ccmd = ["g++", "-std=c++17", "-O3", f"-I{hepmc3inc}", f"-L{hepmc3lib}", f"-I{ProSelecta_ROOT}/include", "-shared", "-fPIC", "-o", f"{outproj}.so", f"{outproj}.cxx"]

cproc = subprocess.run(ccmd, capture_output=True)
if cproc.returncode != 0:
  print(cproc.stdout.decode("utf-8"))
  print(cproc.stderr.decode("utf-8"))
  print("Failed to compile project. Compiler command:\n\t"," ".join(ccmd))
  raise RuntimeError()