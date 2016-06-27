/* Compile : g++ -o run test.cpp -lopenAFE -std=c++11 -lpthread */


#include <memory>
#include <iostream>

#include "openafe/Processors/inputProc.hpp"
#include "openafe/Processors/preProc.hpp"

#define CHUNK_SIZE 100

using namespace openAFE;

int main(int argc, char **argv) {

  std::vector<double> left(CHUNK_SIZE), right(CHUNK_SIZE);
  for ( std::size_t ii = 0 ; ii < CHUNK_SIZE ; ++ii ) {
	left[ii] = ii;
	right[ii] = ii + CHUNK_SIZE;
  }

  std::shared_ptr <InputProc > inputSignal;
  inputSignal.reset( new InputProc("input's Name", 44100, 10, false) );

  std::shared_ptr <PreProc > ppSignal;
  ppSignal.reset( new PreProc("input's Name", inputSignal, true, // pp_bRemoveDC
														   20, // pp_cutoffHzDC
														   false, // pp_bPreEmphasis
														   0.97, // pp_coefPreEmphasis
														   false, // pp_bNormalizeRMS
														   0.5, // pp_intTimeSecRMS
														   false, // pp_bLevelScaling
														   100, // pp_refSPLdB
														   false, // pp_bMiddleEarFiltering
														   _jepsen, // pp_middleEarModel
														   true ) ); // pp_bUnityComp

  inputSignal->processChunk (left.data(), left.size(), right.data(), right.size() );
  inputSignal->releaseChunk();

  ppSignal->processChunk ();
  ppSignal->releaseChunk();


  ppSignal
  std::cout << "left : ";
  for ( std::size_t ii = 0 ; ii < CHUNK_SIZE ; ++ii )
	std::cout << left[ii] << " ";
  std::cout << std::endl;

  std::cout << "right : ";
  for ( std::size_t ii = 0 ; ii < CHUNK_SIZE ; ++ii )
	std::cout << right[ii] << " ";
  std::cout << std::endl;

  return 0;  
}
