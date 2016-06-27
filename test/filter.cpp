/* Compile : g++ -o run test.cpp -lopenAFE -std=c++11 */

#include <iostream>

#include "openafe/Filters/CascadeFilters.hpp"

int main() {

  std::size_t cascadeOrder = 5;

  std::shared_ptr <openAFE::CascadeFilters<double, double, double, double> > filter;
  filter.reset(new openAFE::CascadeFilters<double, double, double, double>( cascadeOrder ) );
 
  std::vector<double> a(5,1);
  std::vector<double> b(2,1);

  std::vector<double> chunk(10,1);
  std::vector<double> chunk2(10,1);
      
  for ( std::size_t ii = 0 ; ii < cascadeOrder ; ++ii )
	filter->setFilter ( b.data(), b.size(), a.data(), a.size() );
	
  filter->exec( chunk.data(), chunk.size(), chunk.data() );
  
  for ( std::size_t ii = 0 ; ii < chunk.size() ; ++ii )
	std::cout << chunk[ii] << " ";
  std::cout << std::endl;

  filter->exec( chunk2.data(), chunk2.size(), chunk2.data() );

  for ( std::size_t ii = 0 ; ii < chunk2.size() ; ++ii )
	std::cout << chunk2[ii] << " ";
  std::cout << std::endl;
    
  return 0;
}
