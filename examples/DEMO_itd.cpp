#include <memory>
#include <iostream>

#include "../src/Processors/inputProc.hpp"
#include "../src/Processors/preProc.hpp"
#include "../src/Processors/gammatoneProc.hpp"
#include "../src/Processors/ihcProc.hpp"
#include "../src/Processors/crossCorrelation.hpp"
#include "../src/Processors/itdProc.hpp"

#include "matFiles.hpp"

using namespace openAFE;

int main(int argc, char **argv) {

  std::vector <std::vector<double> > earSignals;
  double fsHz;
  
  std::string dataPath = "../../examples/Test_signals/AFE_earSignals_16kHz.mat";  
  std::string outputName = "itd_out.mat"; 
  
  std::string errorMessage = "The correct usage is : ";
	  
  int result = matFiles::readMatFile(dataPath.c_str(), earSignals, &fsHz);

  if ( result == 0 ) {
	  std::shared_ptr <InputProc > inputP; 
	  inputP.reset( new InputProc("input", fsHz, 10, false ) );

	  std::shared_ptr <PreProc > ppP;
	  ppP.reset( new PreProc("preProc", inputP ) );

	  std::shared_ptr <GammatoneProc > gtP;
	  gtP.reset( new GammatoneProc("gammatoneProc", ppP ) );

	  std::shared_ptr <IHCProc > ihcP;
	  ihcP.reset( new IHCProc("innerHairCell", gtP ) );

	  std::shared_ptr <CrossCorrelation > xcorrP;
	  xcorrP.reset( new CrossCorrelation("xcorrP", ihcP ) );

	  std::shared_ptr <ITD > itdP;
	  itdP.reset( new ITD("itdP", xcorrP ) );
	  	  	  												  
	  inputP->processChunk ( earSignals[0].data(), earSignals[0].size(), earSignals[1].data(), earSignals[1].size() );
	  inputP->releaseChunk(); 
		  
	  ppP->processChunk ();
	  ppP->releaseChunk();

	  gtP->processChunk ();
	  gtP->releaseChunk();

	  ihcP->processChunk ();
	  ihcP->releaseChunk();
	  
	  xcorrP->processChunk ();
	  xcorrP->releaseChunk();

	  itdP->processChunk ();
	  itdP->releaseChunk();
	  
	  std::vector<std::shared_ptr<twoCTypeBlock<double> > > lOut = itdP->getLeftWholeBufferAccessor();
  
	  matFiles::writeTFSMatFile(outputName.c_str(), lOut, lOut, fsHz);
  } else throw new std::string("Unable to read the inFilePath " + errorMessage);
  																			    
  return 0;  
}

