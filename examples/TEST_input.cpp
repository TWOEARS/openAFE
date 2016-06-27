// Compile : g++ -o run test.cpp -lopenAFE -std=c++11


#include <memory>
#include <iostream>
#include <exception>

#include "../src/Processors/inputProc.hpp"

#define CHUNK_SIZE 100

using namespace openAFE;

int main(int argc, char **argv) {
	
  std::string name;
  uint32_t fs;
  double bufferSize_s;
  bool doNormalize;
  uint32_t normalizeValue;

  std::vector<double> left(CHUNK_SIZE), right(CHUNK_SIZE);
  for ( std::size_t ii = 0 ; ii < CHUNK_SIZE ; ++ii ) {
	left[ii] = ii;
	right[ii] = ii + CHUNK_SIZE;
  }
   
  switch ( argc ) {
	  case 1:
		name = "input";
		fs = 44100;
		bufferSize_s = 10;
		doNormalize = false;
		normalizeValue = MAXCODABLEVALUE;
		break;
	  case 2:
		name = argv[1];
		fs = 44100;
		bufferSize_s = 10;
		doNormalize = false;
		normalizeValue = MAXCODABLEVALUE;
		break;
	  case 3:
		name = argv[1];
		fs = atoi(argv[2]);
		bufferSize_s = 10;
		doNormalize = false;
		normalizeValue = MAXCODABLEVALUE;
		break;
	  case 4:
		name = argv[1];
		fs = atoi(argv[2]);
		bufferSize_s = atoi(argv[3]);
		doNormalize = false;
		normalizeValue = MAXCODABLEVALUE;
		break;
	  case 5:
		name = argv[1];
		fs = atoi(argv[2]);
		bufferSize_s = atoi(argv[3]);
		doNormalize = *(argv[4]) != '0';
		normalizeValue = MAXCODABLEVALUE;
		break;
	  case 6:
		name = argv[1];
		fs = atoi(argv[2]);
		bufferSize_s = atoi(argv[3]);
		doNormalize = *(argv[4]) != '0';
		normalizeValue = atoi(argv[5]);
		break;		
	  default:
		throw new std::string("The correct usage is ./TEST_input name fs bufferSize_s doNormalize normalizeValue");
		break;			 
  }

std::cout << name << std::endl;

  std::shared_ptr <InputProc > inputSignal;
  
  inputSignal.reset( new InputProc(name, fs, bufferSize_s, doNormalize, normalizeValue ) );

  inputSignal->processChunk ( left.data(), left.size(), right.data(), right.size() );
  inputSignal->releaseChunk();
  
  std::shared_ptr<twoCTypeBlock<double> > lOut = inputSignal->getLeftWholeBufferAccessor();
  std::shared_ptr<twoCTypeBlock<double> > rOut = inputSignal->getRightWholeBufferAccessor();
  
  std::cout << "left : ";
  for ( std::size_t ii = 0 ; ii < lOut->array1.second ; ++ii )
	std::cout << *(lOut->array1.first + ii ) << " ";
  for ( std::size_t ii = 0 ; ii < lOut->array2.second ; ++ii )
	std::cout << *(lOut->array2.first + ii ) << " ";
  std::cout << std::endl;	

  std::cout << "right : ";
  for ( std::size_t ii = 0 ; ii < rOut->array1.second ; ++ii )
	std::cout << *(rOut->array1.first + ii ) << " ";
  for ( std::size_t ii = 0 ; ii < rOut->array2.second ; ++ii )
	std::cout << *(rOut->array2.first + ii ) << " ";
  std::cout << std::endl;	

    
  return 0;
}
