import pyProSelecta as pps
from sys import argv

pps.load_file(argv[1])

filter = pps.filter.MINERvA_PRL129_021803_SignalDefinition
proj = pps.project.MINERvA_PRL129_021803_Project_MuonE

# should error out
proj2 = pps.project.MINERvA_PRL129_021803_SignalDefinition
