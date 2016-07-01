#include "openafe/tools/window.hpp"
#include "openafe/tools/window.hpp"
#include "../tools/window.hpp"


#include "iostream"


int main () {


  std::vector<double> vectorWin = openAFE::hann( 320 );
  
  for ( std::size_t ii = 0 ; ii < vectorWin.size() ; ++ii )
	std::cout << vectorWin[ii] << " ";
  std::cout << std::endl;

  return 0;
}
