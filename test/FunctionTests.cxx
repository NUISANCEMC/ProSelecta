#include "ProSelecta/env/env.h"

#include "test_event_builder.h"

#include <cassert>

int main(){

  //Test the particle builder
  {
    auto part = BuildPart("2212 11 1 20 .938 45");
    assert(part->pid() == 2212);
    assert(part->status() == 11);
    assert(std::fabs(part->momentum().length() - 1) < 1E-8);
    assert(std::fabs((part->momentum().phi() * (180.0/M_PI)) - 45) < 1E-8);
    assert(std::fabs((part->momentum().theta() * (180.0/M_PI)) - 20) < 1E-8);
  }

}