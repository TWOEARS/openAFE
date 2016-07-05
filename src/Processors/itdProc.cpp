#include "itdProc.hpp"

using namespace openAFE;
using namespace std;

			void ITD::prepareForProcessing() {
				this->zerosAccecor.reset( new twoCTypeBlock<double>() );
				
			}

			void processChannel( double* firstValue_l, double* firstValue_r, double* result ) {
				
			}
			
			ITD::ITD ( const std::string nameArg, std::shared_ptr<CrossCorrelation > upperProcPtr )
			: TFSProcessor<double> ( nameArg, upperProcPtr->getFsOut(), upperProcPtr->getFsOut(), upperProcPtr->getBufferSize_s(), upperProcPtr->get_nChannels(),
				_magnitude, _itd) {
					
				this->upperProcPtr = upperProcPtr;
					
				this->frameFsHz = 16000; // TODO :  upperProcPtr->upperProcPtr->upperProcPtr->upperProcPtr->getFsIn();
					
				this->prepareForProcessing();			
			}
			
			ITD::~ITD () {
				
			}
			
			void ITD::processChunk () {
				
				this->setNFR ( this->upperProcPtr->getNFR() );

				vector<vector<shared_ptr<twoCTypeBlock<double> > > > lastChunk = this->upperProcPtr->getLeftLastChunkAccessor();
				
				size_t totalFrames = lastChunk[0][0]->getSize();
				
				std::cout << totalFrames << std::endl;				
								
				// Creating a chunk of zeros.
				this->zerosVector.resize( totalFrames, 0 );
				
				// Creating a accesor to it (The data on zerosVector is continious)
				this->zerosAccecor->array1.first = zerosVector.data(); this->zerosAccecor->array2.first = nullptr;
				this->zerosAccecor->array1.second = zerosVector.size(); this->zerosAccecor->array2.second = 0;
				
				// Appending this chunk to all channels of the PMZ.
				leftPMZ->appendChunk( zerosAccecor );
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > lastChunkOfPMZ = leftPMZ->getLastChunkAccesor();
				
				// Create a lag vector
				vector<double> lags(this->upperProcPtr->get_cc_lags_size());
				for ( size_t ii = 0 ; ii < lags.size() ; ++ii )
					lags[ii] = ii - floor(((double)lags.size())/2);
							
				for ( ii = 0 ; ii < totalFrames ; ++ii ) {
					for ( std::size_t jj = 0 ; jj < this->fb_nChannels ; ++jj ) {
						
						// processChannel( l_innerBuffer[jj]->array1.first + n_start, r_innerBuffer[jj]->array1.first + n_start, lastChunkOfPMZ[jj]->getPtr(ii) );
					}
				}				
			}

			std::string ITD::get_upperProcName() {return this->upperProcPtr->getName();}
