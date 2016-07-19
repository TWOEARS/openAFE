#include "itdProc.hpp"

using namespace openAFE;
using namespace std;

			void ITD::prepareForProcessing() {
				this->zerosAccecor.reset( new twoCTypeBlock<double>() );
				
			}

			void ITD::processChannel( vector<shared_ptr<twoCTypeBlock<double> > >& firstValue_l, size_t ii, shared_ptr<twoCTypeBlock<double> > result ) {
				
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
												
				// Creating a chunk of zeros.
				this->zerosVector.resize( totalFrames, 0 );
				
				// Creating a accesor to it (The data on zerosVector is continious)
				this->zerosAccecor->array1.first = zerosVector.data(); this->zerosAccecor->array2.first = nullptr;
				this->zerosAccecor->array1.second = zerosVector.size(); this->zerosAccecor->array2.second = 0;
				
				// Appending this chunk to all channels of the PMZ.
				leftPMZ->appendNChunk( zerosAccecor );
				vector<shared_ptr<twoCTypeBlock<double> > > lastChunkOfPMZ = leftPMZ->getLastChunkAccesor();
				
				// Create a lag vector
				vector<double> lags(this->upperProcPtr->get_cc_lags_size());
				for ( size_t ii = 0 ; ii < lags.size() ; ++ii )
					lags[ii] = ii - floor(((double)lags.size())/2);
				
				// Loop over the time frame			
				for ( size_t ii = 0 ; ii < totalFrames ; ++ii ) {
					// Loop over the frequency channel
					for ( size_t jj = 0 ; jj < this->get_nChannel() ; ++jj ) {

						// Find the peak in the discretized crosscorrelation						
						std::size_t index;
						double max = *( lastChunk[jj][0]->getPtr(ii) );
						for ( std::size_t jjL = 0 ; jjL < lags.size() ; ++jjL ) {
							if ( *( lastChunk[jj][jjL]->getPtr(ii) ) < max ) {
								max = *( lastChunk[jj][jjL]->getPtr(ii) );
								index = ii;
							}							
						}
						
						// Lag of most salient peak
						double lagInt = lags[index];
			
					}
				}				
			}

			std::string ITD::get_upperProcName() {return this->upperProcPtr->getName();}
