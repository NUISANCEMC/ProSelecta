#include "ProSelecta/ProSelecta.h"

#include "HepMC3/Reader.h"
#include "HepMC3/ReaderFactory.h"

#include "HepMC3/GenEvent.h"

#include <functional>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> files_to_read;
std::string filename;

std::string annotate_symname;
std::string analysis_symname;
std::string sel_symname;
std::vector<std::string> projection_symnames;
std::vector<std::string> wgt_symnames;

std::vector<std::string> include_paths;

std::string ProSelecta_env_dir;

bool ProjectAll = false;

void SayUsage(char const *argv[]) {
  std::cout
      << "[USAGE]: " << argv[0] << "\n"
      << "\t-f <sourcefile.cxx>  : Source file to interpret. Can be passed "
         "more than once.\n"
      << "\t-i <file.hepmc>      : Input HepMC3 file\n"
      << "\t-I <path>            : Path to include in the interpreter's search "
         "path\n"
      << "  [One of]: \n"
      << "\t--Analysis <symname> : A symbol to pass each event after reading.\n"
      << "\t                       Can currently only be a cxx symbol."
      << "\t--Filter <symname>   : Symbol to use for filtering\n"
      << "  [Additional Hooks]: \n"
      << "\t--Annotate <symname> : A symbol to pass each event directly after\n"
      << "\t                       reading. Can be used to annotate with\n"
      << "\t                       input-specific metadata.\n"
      << "\t                       Can currently only be a cxx symbol."
      << "\t--Project <symname>  : Symbol to use for projection, can be passed "
         "more than once.\n"
      << "\t--Weight <symname>   : Symbol to use for weights, can be passed "
         "more than once.\n"
      << "\t--ProjectAll         : Whether to run projection functions on cut "
         "events."
      << std::endl;
}

void handleOpts(int argc, char const *argv[]) {
  int opt = 1;
  while (opt < argc) {
    if (std::string(argv[opt]) == "-?" || std::string(argv[opt]) == "--help") {
      SayUsage(argv);
      exit(0);
    } else if (std::string(argv[opt]) == "--ProjectAll") {
      ProjectAll = true;
    } else if ((opt + 1) < argc) {
      if (std::string(argv[opt]) == "-f") {
        files_to_read.push_back(argv[++opt]);
      } else if (std::string(argv[opt]) == "--Annotate") {
        annotate_symname = argv[++opt];
      } else if (std::string(argv[opt]) == "--Analysis") {
        analysis_symname = argv[++opt];
      } else if (std::string(argv[opt]) == "--Filter") {
        sel_symname = argv[++opt];
      } else if (std::string(argv[opt]) == "--Project") {
        while (((opt + 1) < argc) && (argv[opt + 1][0] != '-')) {
          projection_symnames.push_back(argv[++opt]);
        }
      } else if (std::string(argv[opt]) == "--Weight") {
        wgt_symnames.push_back(argv[++opt]);
      } else if (std::string(argv[opt]) == "-i") {
        filename = argv[++opt];
      } else if (std::string(argv[opt]) == "-I") {
        include_paths.push_back(argv[++opt]);
      } else if (std::string(argv[opt]) == "--env") {
        ProSelecta_env_dir = argv[++opt];
      }
    } else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }
}

using annotate_ftype = void (*)(HepMC3::GenEvent &);
using analysis_ftype = void (*)(HepMC3::GenEvent const &);

int main(int argc, char const *argv[]) {

  handleOpts(argc, argv);

  for (auto const &p : include_paths) {
    ProSelecta::Get().AddIncludePath(p);
  }

  char *ProSelecta_env = getenv("PROSELECTA_DIR");

  if (!ProSelecta_env && !ProSelecta_env_dir.length()) {
    std::cout << "[ERROR]: Cannot find ProSelecta environment headers. Either "
                 "define PROSELECTA_DIR in the calling environment or add "
                 "--env command line argument."
              << std::endl;
    return 1;
  }
  if (!ProSelecta_env_dir.length()) {
    ProSelecta_env_dir = ProSelecta_env;
    if (ProSelecta_env_dir.back() != '/') {
      ProSelecta_env_dir += '/';
    }
    ProSelecta_env_dir += "include/ProSelecta/env";
  }

  ProSelecta::Get().AddIncludePath(ProSelecta_env_dir);

  bool read_env = ProSelecta::Get().LoadText("#include \"env.h\"",
                                             ProSelecta::Interpreter::kCling);

  if (!read_env) {
    std::cout
        << "[ERROR]: Cling failed to interpret the processor environment, if "
           "you passed the right path to find these header files and this "
           "still occures then it is a bug in ProSelectaCPP itself."
        << std::endl;
    return 1;
  }

  ProSelecta::Get().LoadText("from pyHepMC3 import HepMC3 as hm",
                             ProSelecta::Interpreter::kPython);

  for (auto const &file_to_read : files_to_read) {
    if (!ProSelecta::Get().LoadFile(file_to_read.c_str())) {
      std::cout << "[ERROR]: Cling failed interpreting: " << argv[1]
                << std::endl;
      return 1;
    }
  }

  std::function<std::remove_pointer_t<annotate_ftype>> annotate_func{};
  if (annotate_symname.length()) {
    annotate_func = ProSelecta::Get().FindClingSym<annotate_ftype>(
        annotate_symname, "HepMC3::GenEvent &");

    if (!annotate_func) {
      std::cout << "[ERROR]: Cling didn't find a function named: "
                << annotate_symname << " in the input file." << std::endl;
      return 1;
    }
  }

  std::function<std::remove_pointer_t<analysis_ftype>> analysis_func{};
  if (analysis_symname.length()) {
    analysis_func =
        ProSelecta::Get().FindClingSym<analysis_ftype>(analysis_symname);

    if (!analysis_func) {
      std::cout << "[ERROR]: Cling didn't find a function named: "
                << analysis_symname << " in the input file." << std::endl;
      return 1;
    }
  }

  ProSelecta_ftypes::sel sel_func = nullptr;
  if (sel_symname.length()) {
    sel_func = ProSelecta::Get().GetFilterFunction(sel_symname);

    if (!sel_func) {
      std::cout << "[ERROR]: Cling didn't find a function named: "
                << sel_symname << " in the input file." << std::endl;
      return 1;
    }
  }

  std::vector<ProSelecta_ftypes::pro> proj_funcs;
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

  std::vector<ProSelecta_ftypes::wgt> wgt_funcs;
  std::vector<std::string> wgt_funcnames;
  for (auto &wgt_sym_name : wgt_symnames) {
    auto wgt_func = ProSelecta::Get().GetWeightFunction(wgt_sym_name);
    if (wgt_func) {
      wgt_funcs.push_back(wgt_func);
      wgt_funcnames.push_back(wgt_sym_name);
    } else {
      std::cout << "[WARN]: Cling didn't find a weight function named: "
                << wgt_sym_name << " in the input file. Skipping." << std::endl;
    }
  }

  std::shared_ptr<HepMC3::Reader> rdr = HepMC3::deduce_reader(filename);
  if (!rdr) {
    std::cout << "[ERROR]: Failed to determine input type for HepMC3 file: "
              << filename << std::endl;
    return 1;
  }

  size_t e_it = 0;

  if (sel_func) {
    std::cout << "# evtnum, pass";
    if (proj_funcs.size()) {
      std::cout << ", ";
    }
    for (size_t i = 0; i < proj_funcs.size(); ++i) {
      std::cout << proj_funcnames[i]
                << (((i + 1) == proj_funcs.size()) ? "" : ", ");
    }
    std::cout << std::endl;
  } // only print the header if we're running the filter

  while (!rdr->failed()) {
    HepMC3::GenEvent evt_in;
    rdr->read_event(evt_in);
    if (rdr->failed()) {
      break;
    }

    if (annotate_func) {
      annotate_func(evt_in);
    }

    if (analysis_func) {
      annotate_func(evt_in);
    }

    if (sel_func) {
      std::cout << e_it << ", ";
      bool filtered = sel_func(evt_in);
      if (filtered || ProjectAll) {
        std::cout << (filtered ? "pass, " : "cut, ");
        for (size_t i = 0; i < proj_funcs.size(); ++i) {
          std::cout << proj_funcs[i](evt_in)
                    << (((i + 1) == proj_funcs.size()) ? "" : ", ");
        }
        for (size_t i = 0; i < wgt_funcs.size(); ++i) {
          std::cout << wgt_funcs[i](evt_in)
                    << (((i + 1) == wgt_funcs.size()) ? "" : ", ");
        }
        std::cout << std::endl;
      } else {
        std::cout << "cut, ";
        for (size_t i = 0; i < proj_funcs.size(); ++i) {
          std::cout << " - " << (((i + 1) == proj_funcs.size()) ? "" : ", ");
        }
        for (size_t i = 0; i < wgt_funcs.size(); ++i) {
          std::cout << " - " << (((i + 1) == wgt_funcs.size()) ? "" : ", ");
        }
        std::cout << std::endl;
      }
    } // end filter section
    e_it++;
  }
}