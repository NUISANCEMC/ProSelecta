#include "ProSelecta/ProSelecta.h"

#include "test_event_builder.h"

using namespace ps;

int main(){

  auto evt1 = BuildEvent({{"14 4 1", "1000060120 11 0"},
                          {"14 3 1", "1000060110 1 0", "2112 21 0"},
                          {"13 1 0.7", "2212 1 0.15"}},
                         {2, 1});

  
  ProSelecta::Get().load_text(R"(
int myfunc(HepMC3::GenEvent const &ev){
  ps::event::hm_out_part(ev, ps::pdg::kElectron);
  return 0;
}
)");

  auto sfunc = ProSelecta::Get().get_select_func("myfunc");

  sfunc(evt1);
}