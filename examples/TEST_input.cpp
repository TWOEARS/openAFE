// Compile : g++ -o run test.cpp -lopenAFE -std=c++11


#include <memory>
#include <iostream>

#include "../src/Processors/inputProc.hpp"

using namespace openAFE;

int main() {
 
  std::shared_ptr <InputProc > inputSignal;
  
  inputSignal.reset( new InputProc("input's Name", 44100, 10, false) );
  
  return 0;  
}
