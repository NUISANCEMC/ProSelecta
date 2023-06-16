#include "ProSelecta/ProSelecta.h"

#include <iostream>

int main(int argc, char const *argv[]) {

  ProSelecta::Get().LoadText(R"(
print("hello from python")
def test():
  print("Ran test")
)",
                             ProSelecta::Interpreter::kPython);

  std::cout << "test is defined: "
            << ProSelecta::Get().PythonFunctionIsDefined("test") << std::endl;
  std::cout << "test1 is defined: "
            << ProSelecta::Get().PythonFunctionIsDefined("test1") << std::endl;

  ProSelecta::Get().LoadText(R"(
def test2():
  print("Ran test2")
)",
                             ProSelecta::Interpreter::kPython);

  std::cout << "test2 is defined: "
            << ProSelecta::Get().PythonFunctionIsDefined("test2") << std::endl;
}