#include "crossCorrelation.hpp"

using namespace openAFE;
using namespace std;

			size_t CrossCorrelation::getNLags( double maxDelaySec, double fs ) {
				this->maxLag = ceil( maxDelaySec * fs );
				size_t lagN = maxLag * 2 + 1;
				
				this->lags.resize( lagN );
				
				this->lags = linspace<double>(-this->maxLag, this->maxLag, lagN);
				for ( size_t i = 0 ; i < lagN ; ++i )
					this->lags[i] /= fs;
					
				return lagN;
			}

			void CrossCorrelation::prepareForProcessing() {
				this->getNLags( this->maxDelaySec, this->getFsIn() );

			}

			vector<double> CrossCorrelation::processChannel( double* firstValue_l, double* firstValue_r ) {

				// Extract frame for left and right input
				multiplication( this->win.data(), firstValue_l, this->wSize, firstValue_l );
				multiplication( this->win.data(), firstValue_r, this->wSize, firstValue_r );
							
				// Compute the N-points for the Fourier domain
				size_t N = pow (2,nextpow2(2 * this->wSize - 1));

				// Compute the frames in the Fourier domain										
				vector<complex<double> > leftFFT = fft( firstValue_l, this->wSize, N );
				vector<complex<double> > rightFFT = fft( firstValue_r, this->wSize, N );
												
				// Compute cross-power spectrum
				_conj( rightFFT );
				multiplication( leftFFT.data(), rightFFT.data(), leftFFT.size(), leftFFT.data() );

				// Back to time domain
				vector<double> c = ifft( leftFFT.data(), this->wSize, N );
								
                // Adjust to requested maximum lag and move negative
                // lags upfront
				vector<double> cFinal( this->lags.size() );

				if ( this->maxLag > this->wSize ) {
					// Then pad with zeros
					// TODO :
				} else {
					// Else keep lags lower than requested max
					std::size_t jj = 0;
					for( std::size_t ii = N - this->maxLag ; ii < N ; ++ii, ++jj ) {
						cFinal[jj] = c[ii];
					}
					for( std::size_t ii = 0 ; ii <= this->maxLag   ; ++ii, ++jj ) {
						cFinal[jj] = c[ii];
					}
				}
	
				double powL = sumPow( firstValue_l, this->wSize, 2 );
				double powR = sumPow( firstValue_r, this->wSize, 2 );
				
				double div = sqrt( powL * powR + EPSILON );
				for ( std::size_t ii = 0 ; ii < this->lags.size() ; ++ii )
					cFinal[ii] /= div;
							
				return cFinal;
				
			}
			
			CrossCorrelation::CrossCorrelation (const string nameArg, shared_ptr<IHCProc > upperProcPtr, double wSizeSec, double hSizeSec, double maxDelaySec, windowType wname )
			: WindowBasedLAGProcs (nameArg, upperProcPtr, _crosscorrelation, this->getNLags( maxDelaySec, upperProcPtr->getFsOut() ), wSizeSec, hSizeSec, wname ) {
				this->maxDelaySec = maxDelaySec;
				
				this->prepareForProcessing();
			}

			CrossCorrelation::~CrossCorrelation () {
				
			}
			
			void CrossCorrelation::processChunk () {	

				this->setNFR ( this->upperProcPtr->getNFR() );

				// Append provided input to the buffer
				this->buffer_l->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
				this->buffer_r->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );

				/* The buffer should be linearized for windowing. */
				this->buffer_l->linearizeBuffer();
				this->buffer_r->linearizeBuffer();

				vector<shared_ptr<twoCTypeBlock<double> > > l_innerBuffer = buffer_l->getWholeBufferAccesor();
				vector<shared_ptr<twoCTypeBlock<double> > > r_innerBuffer = buffer_r->getWholeBufferAccesor();

				// Quick control of dimensionality
				assert( l_innerBuffer.size() == r_innerBuffer.size() );

				size_t totalFrames = floor( ( this->buffer_l->getSize() - ( this->wSize - this->hSize ) ) / this->hSize );

				// Creating a chunk of zeros.
				this->zerosVector.resize( totalFrames, 0 );

				// Creating a accesor to it (The data on zerosVector is continious)
				this->zerosAccecor->array1.first = zerosVector.data(); this->zerosAccecor->array2.first = nullptr;
				this->zerosAccecor->array1.second = zerosVector.size(); this->zerosAccecor->array2.second = 0;

				// Appending this chunk to all channels of the PMZ.
				leftPMZ->appendChunk( zerosAccecor );
				vector<vector<shared_ptr<twoCTypeBlock<double> > > > lastChunkOfPMZ = leftPMZ->getLastChunkAccesor();

				std::size_t ii;
				uint32_t n_start;
				for ( ii = 0 ; ii < totalFrames ; ++ii ) {

					n_start = ii * this->hSize;
					
					 for ( std::size_t jj = 0 ; jj < this->get_nChannel() ; ++jj ) {
							
						vector<double> chunk = this->processChannel( l_innerBuffer[jj]->array1.first + n_start, r_innerBuffer[jj]->array1.first + n_start );
						
						for ( std::size_t jjL = 0 ; jjL < this->lags.size() ; ++jjL )
							*( lastChunkOfPMZ[jj][jjL]->getPtr(ii) ) = chunk[jjL];
					}
				}
				
				this->buffer_l->pop_chunk( ii * this->hSize );
				this->buffer_r->pop_chunk( ii * this->hSize );

			}
			
			// getters
			const double CrossCorrelation::get_cc_maxDelaySec() {return this->maxDelaySec;}
			const double *CrossCorrelation::get_cc_lags() {return this->lags.data();}
			const size_t CrossCorrelation::get_cc_lags_size() {return this->lags.size();}
  
			// setters			
			void CrossCorrelation::set_cc_maxDelaySec(const double arg) {this->maxDelaySec=arg; this->prepareForProcessing ();}
