#!/usr/bin/env python3
import yaml

import pyProSelecta as pps

import sys, os

import subprocess

if len(sys.argv) < 3:
  raise RuntimeError("Excepted 2 arguments: <input_manifest.yml> <outprojectname>")

with open(sys.argv[1]) as manifest:
  data = yaml.safe_load(manifest)

snippet_files = []
project_funcs = []
select_funcs = []

for analysis in data:
  snippet = analysis["snippet"]
  print(f"snippet file: {snippet}")
  if not pps.load_file(snippet):
    raise RuntimeError(f"Failed to parse snippet file {snippet}. See above cling output for errors")

  print(f"  checking select functions")
  funcs = analysis["functions"]

  for sf in funcs["select"]:
    print(f"    fetching symbol for {sf}")
    if pps.select.get(sf):
      select_funcs.append(sf)
    else:
      raise RuntimeError(f"Failed to read selection function: {sf}")


  print(f"  checking project functions")
  for sf in funcs["project"]:
    print(f"    fetching symbol for {sf}")
    if pps.project.get(sf):
      project_funcs.append(sf)
    else:
      raise RuntimeError(f"Failed to read projection function: {sf}")

  snippet_files.append(snippet)

outproj = sys.argv[2]

with open(f"{outproj}.h",'w') as outputh:
  outputh.write('#include "ProSelecta/env.h"\n')

  for sf in select_funcs:
    outputh.write(f'int {sf}(HepMC3::GenEvent const&);\n')

  for sf in project_funcs:
    outputh.write(f'double {sf}(HepMC3::GenEvent const&);\n')

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
  outputimpl.write(f'#include "{outproj}.h"\n')
  for sn in snippet_files:
    with open(sn,'r') as snf:
      outputimpl.write(f"//START -- imported from {sn}\n")
      for line in snf:
        outputimpl.write(line)
      outputimpl.write(f"//END -- imported from {sn}\n")

ccmd = ["g++", "-std=c++17", "-O3", f"-I{hepmc3inc}", f"-L{hepmc3lib}", f"-I{ProSelecta_ROOT}/include", "-shared", "-fPIC", "-o", f"{outproj}.so", f"{outproj}.cxx"]

cproc = subprocess.run(ccmd, capture_output=True)
if cproc.returncode != 0:
  print(cproc.stdout.decode("utf-8"))
  print(cproc.stderr.decode("utf-8"))
  print("Failed to compile project. Compiler command:\n\t"," ".join(ccmd))
  raise RuntimeError()