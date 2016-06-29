#include "ildProc.hpp"

using namespace openAFE;

			uint32_t ILDProc::calcFsOut( double ild_hSizeSec ) {
				return 1 / ild_hSizeSec;
			}

			void ILDProc::prepareForProcessing() {
            
				// Compute internal parameters
				this->ild_wSize = 2 * round( this->ild_wSizeSec * this->getFsIn() / 2 );
				this->ild_hSize = round( this->ild_hSizeSec * this->getFsIn() );		
		
				switch ( this->ild_wname ) {
					case _hamming: 
						this->win = hamming( this->ild_wSize );
						break;
					case _hanning: 
						this->win = hanning( this->ild_wSize );
						break;
					case _hann: 
						this->win = hann( this->ild_wSize );
						break;
					case _blackman: 
						this->win = blackman( this->ild_wSize );
						break;
					case _triang: 
						this->win = triang( this->ild_wSize );
						break;
					case _sqrt_win: 
						this->win = sqrt_win( this->ild_wSize );
						break;
					default:
						this->win.resize( this->ild_wSize, 0 );
						break;
				}
				
				this->zerosAccecor.reset( new twoCTypeBlock<double>() );
			}

			void ILDProc::processChannel( double* firstValue_l, double* firstValue_r, std::shared_ptr<twoCTypeBlock<double> > PMZ, size_t ii ) {
				
				multiplication( this->win.data(), firstValue_l, this->ild_wSize, firstValue_l );
				multiplication( this->win.data(), firstValue_r, this->ild_wSize, firstValue_r );
							
				double mSq_l = meanSquare( firstValue_l, this->ild_wSize );
				double mSq_r = meanSquare( firstValue_r, this->ild_wSize );
							
				if ( PMZ->array1.second >= ii )
					*(PMZ->array1.first + ii ) = ild( mSq_r, mSq_l );
				else
					*(PMZ->array2.first + ( ii - PMZ->array1.second ) ) = ild( mSq_r, mSq_l );
			}
					
			ILDProc::ILDProc (const std::string nameArg, std::shared_ptr<IHCProc > upperProcPtr, double ild_wSizeSec, double ild_hSizeSec, ildWindowType ild_wname  )
			: TFSProcessor<double > (nameArg, upperProcPtr->getFsOut(), this->calcFsOut( ild_hSizeSec ), upperProcPtr->getBufferSize_s(), upperProcPtr->get_ihc_nChannels(), "magnitude", _ratemap) {
					
				this->fb_nChannels = upperProcPtr->get_ihc_nChannels();
				
				this->upperProcPtr = upperProcPtr;
				this->ild_wSizeSec = ild_wSizeSec;
				this->ild_hSizeSec = ild_hSizeSec;
				this->ild_wname = ild_wname;
								
				this->buffer_l.reset( new TimeFrequencySignal<double>( this->getFsIn(), this->getBufferSize_s(), this->fb_nChannels, "inner buffer", "magnitude", _left) );
				this->buffer_r.reset( new TimeFrequencySignal<double>( this->getFsIn(), this->getBufferSize_s(), this->fb_nChannels, "inner buffer", "magnitude", _right) );
								
				this->prepareForProcessing();
			}

			ILDProc::~ILDProc () {
				
				this->win.clear();
				this->buffer_l.reset();
				this->buffer_r.reset();
			}
			
			void ILDProc::processChunk () {				
				this->setNFR ( this->upperProcPtr->getNFR() );
							
				// Append provided input to the buffer
				this->buffer_l->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
				this->buffer_r->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );
				
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > l_innerBuffer = buffer_l->getLastChunkAccesor();
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > r_innerBuffer = buffer_r->getLastChunkAccesor();
				
				// Quick control of dimensionality
				assert( l_innerBuffer.size() == r_innerBuffer.size() );
				
				uint32_t nSamples = l_innerBuffer[0]->getSize();
				
				for ( std::size_t ii = 0 ; ii < l_innerBuffer.size() ; ++ii ) {
					assert ( nSamples == r_innerBuffer[ii]->getSize() );
					assert ( l_innerBuffer[ii]->getSize() == nSamples );
				}

				std::size_t nSamples_1 = l_innerBuffer[0]->array1.second;
				std::size_t nSamples_2 = l_innerBuffer[0]->array2.second;

				// How many frames are in the buffered input?
				std::size_t nFrames_1 = 0, nFrames_2 = 0;

				if ( nSamples_1 > this->ild_wSize ) 
					nFrames_1 = floor( ( nSamples_1 - ( this->ild_wSize - this->ild_hSize ) ) / this->ild_hSize );
				if ( nSamples_2 > this->ild_wSize ) 
					nFrames_2 = floor( ( nSamples_2 - ( this->ild_wSize - this->ild_hSize ) ) / this->ild_hSize );

				// Creating a chunk of zeros.
				this->zerosVector.resize( nFrames_1 + nFrames_2, 0 );

				this->zerosAccecor->array1.first = zerosVector.data(); this->zerosAccecor->array2.first = nullptr;
				this->zerosAccecor->array1.second = zerosVector.size(); this->zerosAccecor->array2.second = 0;
				

				// Appending this chunk to all channels of the PMZ.
				leftPMZ->appendChunk( zerosAccecor );
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > PMZ = leftPMZ->getLastChunkAccesor();
				
				// Processing on PMZ
            	if ( nFrames_1 > 0 ) {
					// Loop on the time frame				
					for ( size_t ii = 0 ; ii < nFrames_1 ; ++ii ) {
						// Get start and end indexes for the current frame
						uint32_t n_start = ii * this->ild_hSize;
						// uint32_t n_end = ii * this->ild_hSize + this->ild_wSize - 1;
					
						// Loop on the channel
						for ( size_t jj = 0 ; jj < this->fb_nChannels ; ++jj ) {
							processChannel( l_innerBuffer[jj]->array1.first + n_start, r_innerBuffer[jj]->array1.first + n_start, PMZ[jj], ii );
						}
						
			/*			std::vector<std::thread> threads;
						for ( size_t jj = 0 ; ii < this->fb_nChannels ; ++jj )
							threads.push_back(std::thread( &ILDProc::processChannel, this, l_innerBuffer[jj]->array1.first + n_start, r_innerBuffer[jj]->array1.first + n_start, PMZ[jj], ii ));

						// Waiting to join the threads
						for (auto& t : threads)
							t.join();						
			*/			
						
						
					}
				}
				
			}
			
			/* Comapres informations and the current parameters of two processors */
			bool ILDProc::operator==( ILDProc& toCompare ) {
				if ( this->compareBase( toCompare ) )
					if ( ( this->get_ild_wSizeSec() == toCompare.get_ild_wSizeSec() ) and 
						 ( this->get_ild_hSizeSec() == toCompare.get_ild_hSizeSec() ) and 					
						 ( this->get_ild_wname() == toCompare.get_ild_wname() ) )				     		     			     
						return true;
				return false;
			}

			// getters
			const double ILDProc::get_ild_wSizeSec() {return this->ild_wSizeSec;}
			const double ILDProc::get_ild_hSizeSec() {return this->ild_hSizeSec;}
			const ildWindowType ILDProc::get_ild_wname() {return this->ild_wname;}
			
			const uint32_t ILDProc::get_ild_nChannels() {return this->fb_nChannels;}

			// setters			
			void ILDProc::set_ild_wSizeSec(const double arg) {this->ild_wSizeSec=arg; this->prepareForProcessing ();}
			void ILDProc::set_ild_hSizeSec(const double arg) {this->ild_hSizeSec=arg; this->prepareForProcessing ();}
			void ILDProc::set_ild_wname(const ildWindowType arg) {this->ild_wname=arg; this->prepareForProcessing ();}

			std::string ILDProc::get_upperProcName() {return this->upperProcPtr->getName();}
