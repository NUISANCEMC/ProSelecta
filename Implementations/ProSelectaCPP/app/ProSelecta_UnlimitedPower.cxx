#include "ProSelecta/ProSelecta.h"

#include "HepMC3/Reader.h"
#include "HepMC3/ReaderFactory.h"

#include "HepMC3/GenEvent.h"

#include <iostream>
#include <string>
#include <vector>

std::string file_to_read;
std::string filename;

std::string analysis_symname;
std::string filter_symname;
std::vector<std::string> projection_symnames;
std::vector<std::string> weights_symnames;

std::vector<std::string> include_paths;

void SayUsage(char const *argv[]) {
  std::cout
      << "[USAGE]: " << argv[0] << "\n"
      << "\t-f <sourcefile.cxx> : File to interpret\n"
      << "\t--Filter <symname>  : Symbol to use for filtering\n"
      << "\t--Project <symname> : Symbol to use for projection, can be passed "
         "more than once\n"
      << "\t--Weight <symname>  : Symbol to use for weights, can be passed "
         "more than once\n"
      << "\t-i <file.hepmc>     : Input HepMC3 file\n"
      << "\t-I <path>           : Path to include in the interpreter's search "
         "path\n"
      << std::endl;
}

void handleOpts(int argc, char const *argv[]) {
  int opt = 1;
  while (opt < argc) {
    if (std::string(argv[opt]) == "-?" || std::string(argv[opt]) == "--help") {
      SayUsage(argv);
      exit(0);
    } else if ((opt + 1) < argc) {
      if (std::string(argv[opt]) == "-f") {
        file_to_read = argv[++opt];
      } else if (std::string(argv[opt]) == "--Analysis") {
      analysis_symname = argv[++opt];
      } else if (std::string(argv[opt]) == "-i") {
        filename = argv[++opt];
      } else if (std::string(argv[opt]) == "-I") {
        include_paths.push_back(argv[++opt]);
      }
    } else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }

}

int main(int argc, char const *argv[]) {

  handleOpts(argc, argv);

  for (auto const &p : include_paths) {
    ProSelecta::Get().AddIncludePath(p);
  }

  ProSelecta::Get().LoadText("#include \"ProSelecta/Units.h\"");
  ProSelecta::Get().LoadText("#include \"ProSelecta/PDGCodes.h\"");
  ProSelecta::Get().LoadText("#include \"ProSelecta/Selectors.h\"");
  ProSelecta::Get().LoadText("#include \"ProSelecta/Projectors.h\"");

  if (!ProSelecta::Get().LoadFile(file_to_read.c_str())) {
    std::cout << "[ERROR]: Cling failed interpreting: " << argv[1] << std::endl;
    return 1;
  }

  auto analysis_func = ProSelecta::Get().GetAnalysisFunction(analysis_symname);
  if (!analysis_func) {
    std::cout << "[ERROR]: Cling didn't find an analysis function named: "
              << filter_symname << " in the input file. Did you extern \"C\"?"
              << std::endl;
    return 1;
  }
  analysis_func(filename);
}