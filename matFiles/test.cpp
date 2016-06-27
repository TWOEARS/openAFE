#include "matFiles.hpp"

int main(int argc, char **argv)
{
  std::vector <std::vector<double> > earSignals;
  double fsHz;
 
  int result;

  if (argc > 1) {
    result = matFiles::readMatFile(argv[1], earSignals, &fsHz);
	// std::cout << earSignals[1][0] << std::endl;    
  } else{
    result = 0;
    printf("Usage: matdgns <matfile>");
    printf(" where <matfile> is the name of the MAT-file");
    printf(" to be diagnosed\n");
  }
  
  return (result==0)?EXIT_SUCCESS:EXIT_FAILURE;

}
