import pyProSelecta as pps
from sys import argv
import os

pps.add_include_path(os.environ['ProSelecta_ROOT']+"/include/ProSelecta/env")
pps.add_include_path("/opt/HepMC3/git_master/include/")

if not pps.load_text("#include \"env.h\""):
  print("Failed to start the ProSelecta environment. See above errors")
  exit(1)

pps.load_file(argv[1])

filter = pps.filter.MINERvA_PRL129_021803_SignalDefinition
proj = pps.project.MINERvA_PRL129_021803_Project_MuonE
proj2 = pps.project.MINERvA_PRL129_021803_SignalDefinition
