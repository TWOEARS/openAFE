#include "ildProc.hpp"

using namespace openAFE;

			void ILDProc::processChannel( double* firstValue_l, double* firstValue_r, double *result ) {

				multiplication( this->win.data(), firstValue_l, this->wSize, firstValue_l );
				multiplication( this->win.data(), firstValue_r, this->wSize, firstValue_r );
							
				double mSq_l = meanSquare( firstValue_l, this->wSize );
				double mSq_r = meanSquare( firstValue_r, this->wSize );
				
				*(result) = ild( mSq_r, mSq_l );
			}
					
			ILDProc::ILDProc (const std::string nameArg, std::shared_ptr<IHCProc > upperProcPtr, double wSizeSec, double hSizeSec, windowType wname  )
			: WindowBasedProcs (nameArg, upperProcPtr, _ild, wSizeSec, hSizeSec, wname, _magnitude ) {

			}

			ILDProc::~ILDProc () {
				
			}
			
			void ILDProc::processChunk () {				
				this->setNFR ( this->upperProcPtr->getNFR() );
							
				// Append provided input to the buffer
				this->buffer_l->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
				this->buffer_r->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );
				
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > l_innerBuffer = buffer_l->getWholeBufferAccesor();
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > r_innerBuffer = buffer_r->getWholeBufferAccesor();
				
				// Quick control of dimensionality
				assert( l_innerBuffer.size() == r_innerBuffer.size() );

				std::size_t totalFrames = floor( ( l_innerBuffer[0]->getSize() - ( this->wSize - this->hSize ) ) / this->hSize );
				
				// Creating a chunk of zeros.
				this->zerosVector.resize( totalFrames, 0 );
				
				// Creating a accesor to it (The data on zerosVector is continious)
				this->zerosAccecor->array1.first = zerosVector.data(); this->zerosAccecor->array2.first = nullptr;
				this->zerosAccecor->array1.second = zerosVector.size(); this->zerosAccecor->array2.second = 0;

				// Appending this chunk to all channels of the PMZ.
				leftPMZ->appendChunk( zerosAccecor );
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > lastChunkOfPMZ = leftPMZ->getLastChunkAccesor();

				std::size_t nSamples_1 = l_innerBuffer[0]->array1.second;
				std::size_t nSamples_2 = l_innerBuffer[0]->array2.second;

				std::size_t nFrames_1 = 0, nFrames_2 = 0, reste1, reste2;

				if ( nSamples_1 > this->wSize ) {
					reste1 = ( nSamples_1 - ( this->wSize - this->hSize ) ) / this->hSize;
					nFrames_1 = floor( reste1 );
					
					reste1 -= nFrames_1;
				}
				if ( nSamples_2 > this->wSize ) 
					nFrames_2 = floor( ( nSamples_2 - ( this->wSize - this->hSize ) ) / this->hSize );
													
				for ( std::size_t ii = 0 ; ii < totalFrames : ++ii ) {
					
					if ( ii < nFrames_1 ) {
						uint32_t n_start = ii * this->hSize;
					
						// Loop on the channels : Got better run-time results when not creating threads.
						for ( std::size_t jj = 0 ; jj < this->fb_nChannels ; ++jj )
							processChannel( l_innerBuffer[jj]->array1.first + n_start, r_innerBuffer[jj]->array1.first + n_start, lastChunkOfPMZ[jj]->getPtr(ii) );
							
					} else if ( ( ii == nFrames_1 ) && ( reste1 > 0 ) ) { /* and ii < totalFrames */
						
						std::vector<double> tmp( this->hSize, 0 ); /* The data between array1 and array2 */
						
						for ( std::size_t jj = 0 ; jj < this->hSize ; ++jj ) {
							// tmp[jj] = 
						}
						
						
						
					} else if ( ii > nFrames_1 ) { /* and ii < totalFrames */
					
					}
				}
			}
