/* Compile : g++ -o run test.cpp -lopenAFE -std=c++11 */

#include <iostream>

#include "openafe/Signals/TimeFrequencySignal.hpp"
#include "openafe/Filters/GenericFilter.hpp"

int main() {
  
 
  std::shared_ptr <openAFE::TimeFrequencySignal<double> > signal;
  signal.reset(new openAFE::TimeFrequencySignal<double>(10,10,1));

  std::shared_ptr <openAFE::GenericFilter<double, double, double, double> > filter;
  filter.reset(new openAFE::GenericFilter<double, double, double, double> );
 
  std::cout << "signal's name : " << signal->getName() << std::endl;
  std::cout << "isInit ? : " << filter->isInitialized () << std::endl;
 
  return 0;
}
