#include "ProSelecta/ProSelecta.h"

#include "HepMC3/Reader.h"
#include "HepMC3/ReaderFactory.h"

#include "HepMC3/GenEvent.h"

#include <iostream>
#include <string>
#include <vector>

std::string file_to_read;
std::string filename;

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
      } else if (std::string(argv[opt]) == "--Filter") {
        filter_symname = argv[++opt];
      } else if (std::string(argv[opt]) == "--Project") {
        projection_symnames.push_back(argv[++opt]);
      } else if (std::string(argv[opt]) == "--Weight") {
        weights_symnames.push_back(argv[++opt]);
      } else if (std::string(argv[opt]) == "-i") {
        filename = argv[++opt];
      } else if (std::string(argv[opt]) == "-i") {
        include_paths.push_back(argv[++opt]);
      }
    } else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }

  if (!filter_symname.length()) {
    std::cout << "[ERROR]: No filter function specified." << std::endl;
    abort();
  }
}

int main(int argc, char const *argv[]) {

  handleOpts(argc, argv);

  for (auto const &p : include_paths) {
    ProSelecta::Get().AddIncludePath(p);
  }

  ProSelecta::Get().LoadText("#include \"ProSelecta/Selectors.h\"");
  ProSelecta::Get().LoadText("#include \"ProSelecta/Units.h\"");

  if (!ProSelecta::Get().LoadFile(file_to_read.c_str())) {
    std::cout << "[ERROR]: Cling failed interpreting: " << argv[1] << std::endl;
    return 1;
  }

  auto filter_func = ProSelecta::Get().GetFilterFunction(filter_symname);
  if (!filter_func) {
    std::cout << "[ERROR]: Cling didn't find a filter function named: "
              << filter_symname << " in the input file. Did you extern \"C\"?"
              << std::endl;
    return 1;
  }

  std::vector<ProSelecta_ftypes::project> proj_funcs;
  std::vector<std::string> proj_funcnames;
  for (auto &proj_sym_name : projection_symnames) {
    auto proj_func = ProSelecta::Get().GetProjectionFunction(proj_sym_name);
    if (proj_func) {
      proj_funcs.push_back(proj_func);
      proj_funcnames.push_back(proj_sym_name);
    } else {
      std::cout << "[WARN]: Cling didn't find a projection function named: "
                << proj_sym_name << " in the input file. Skipping."
                << std::endl;
    }
  }

  std::shared_ptr<HepMC3::Reader> rdr = HepMC3::deduce_reader(filename);
  if (!rdr) {
    std::cout << "[ERROR]: Failed to determine input type for HepMC3 file: "
              << filename << std::endl;
    return 1;
  }

  size_t e_it = 0;

  std::cout << "# evtnum, pass";
  if (proj_funcs.size()) {
    std::cout << ", ";
  }
  for (size_t i = 0; i < proj_funcs.size(); ++i) {
    std::cout << proj_funcnames[i]
              << (((i + 1) == proj_funcs.size()) ? "" : ", ");
  }
  std::cout << std::endl;

  while (!rdr->failed()) {
    HepMC3::GenEvent evt_in;
    rdr->read_event(evt_in);
    if (rdr->failed()) {
      break;
    }

    std::cout << e_it << ", ";
    bool filtered = filter_func(evt_in);
    if (filtered) {
      std::cout << "pass, ";
      for (size_t i = 0; i < proj_funcs.size(); ++i) {
        std::cout << proj_funcs[i](evt_in)
                  << (((i + 1) == proj_funcs.size()) ? "" : ", ");
      }
      std::cout << std::endl;
    } else {
      std::cout << "cut, ";
      for (size_t i = 0; i < proj_funcs.size(); ++i) {
        std::cout << " - " << (((i + 1) == proj_funcs.size()) ? "" : ", ");
      }
      std::cout << std::endl;
    }
    e_it++;
  }
}