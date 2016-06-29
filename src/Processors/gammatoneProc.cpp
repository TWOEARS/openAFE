#include "gammatoneProc.hpp"

using namespace openAFE;
			      			
            std::size_t GammatoneProc::verifyParameters( filterBankType fb_type, double fb_lowFreqHz, double fb_highFreqHz, double fb_nERBs,
									 uint32_t fb_nChannels, double* fb_cfHz, std::size_t fb_cfHz_length, uint32_t fb_nGamma, double fb_bwERBs ) {
				
				this->fb_type = fb_type;
				this->fb_lowFreqHz = fb_lowFreqHz;
				this->fb_highFreqHz = fb_highFreqHz;
				this->fb_nERBs = fb_nERBs;
				this->fb_nChannels = fb_nChannels;		
				this->fb_nGamma = fb_nGamma;
				this->fb_bwERBs = fb_bwERBs;

				/* Solve the conflicts between center frequencies, number of channels, and
				 * distance between channels.
				 */

				if ( !( fb_cfHz ) and ( fb_cfHz_length > 0 ) ) {
                // Highest priority case: a vector of channels center 
                //   frequencies is provided
					this->cfHz.resize( fb_cfHz_length );
					for ( std::size_t ii = 0 ; ii < fb_cfHz_length ; ++ii )
						cfHz[ii]= *( fb_cfHz + ii );
             
					this->fb_lowFreqHz = *( cfHz.begin() );
					this->fb_highFreqHz = *( cfHz.end() - 1 );
					this->fb_nChannels = fb_cfHz_length;
					this->fb_nERBs = 0;

				} else 
					if ( this->fb_nChannels > 0 ) {
						/* Medium priority: frequency range and number of channels
						 *  are provided.
						 */ 
					   
						// Build a vector of center ERB frequencies
						std::vector<double> ERBS = linspace( freq2erb(this->fb_lowFreqHz),
															freq2erb(this->fb_highFreqHz),
															this->fb_nChannels ); 
						this->cfHz.resize( ERBS.size() );
						erb2freq( ERBS.data(), ERBS.size(), cfHz.data() );    // Convert to Hz
									
						this->fb_nERBs = ( *(ERBS.end()-1) - *(ERBS.begin()) ) / this->fb_nChannels ;
					}
					else {
						/* Lowest (default) priority: frequency range and distance 
						 *   between channels is provided (or taken by default).
						 */
						
						// Build vector of center ERB frequencies
						std::vector<double> ERBS;
						for ( double tmp = freq2erb( this->fb_lowFreqHz ) ; tmp < freq2erb( this->fb_highFreqHz ) ; tmp += this->fb_nERBs )
							ERBS.push_back( tmp );
		
						this->cfHz.resize( ERBS.size() );
						erb2freq( ERBS.data(), ERBS.size(), this->cfHz.data() );    // Convert to Hz

						this->fb_nChannels = this->cfHz.size();														
					}
				return this->fb_nChannels;
			}
			
			void GammatoneProc::populateFilters( filterPtrVector& filters ) {
				
				const uint32_t fs = this->getFsIn();
				filters.clear();
				filters.resize(cfHz.size());
								
				for ( unsigned int ii = 0 ; ii < this->cfHz.size() ; ++ii ) {
					gammatoneFilterPtr thisFilter( new GammatoneFilter( this->cfHz[ii], fs, this->fb_nGamma, this->fb_bwERBs ) );
					filters[ii] = thisFilter ;
				}
			}

			void GammatoneProc::processChannel ( gammatoneFilterPtr oneFilter, std::shared_ptr<twoCTypeBlock<double> > oneChannel ) {
				// 0- Initialization
				std::size_t dim1 = oneChannel->array1.second;
				std::size_t dim2 = oneChannel->array2.second;
							
				double* firstValue1 = oneChannel->array1.first;
				double* firstValue2 = oneChannel->array2.first;
													
				std::vector< std::complex<double > > tmpComplex;
				if ( dim1 > 0 ) {
					tmpComplex.resize(dim1, 0);
					oneFilter->exec( firstValue1, dim1, tmpComplex.data() );							
					for ( std::size_t ii = 0 ; ii < dim1 ; ++ii )
						*( firstValue1 + ii ) = tmpComplex[ii].real() * 2;
				}
				if ( dim2 > 0 )	{
					tmpComplex.resize(dim2, 0);
					oneFilter->exec( firstValue2, dim2, tmpComplex.data() );
					for ( std::size_t ii = 0 ; ii < dim2 ; ++ii )
						*( firstValue2 + ii ) = tmpComplex[ii].real() * 2;	
				}

			}

			void GammatoneProc::processLR ( filterPtrVector& filters, std::vector<std::shared_ptr<twoCTypeBlock<double> > > PMZ ) {
				std::vector<std::thread> threads;
				  for ( std::size_t ii = 0 ; ii < this->cfHz.size() ; ++ii )
					threads.push_back(std::thread( &GammatoneProc::processChannel, this, filters[ii], PMZ[ii] ));

				  // Waiting to join the threads
				  for (auto& t : threads)
					t.join();
			}
														
			GammatoneProc::GammatoneProc (const std::string nameArg, std::shared_ptr<PreProc > upperProcPtr, filterBankType fb_type,
																											 double fb_lowFreqHz,
																											 double fb_highFreqHz,
																											 double fb_nERBs,
																											 uint32_t fb_nChannels,		
																											 double* fb_cfHz,		
																											 std::size_t fb_cfHz_length,		
																											 uint32_t fb_nGamma,
																											 double fb_bwERBs
				) : TFSProcessor<double > (nameArg, upperProcPtr->getFsOut(), upperProcPtr->getFsOut(), upperProcPtr->getBufferSize_s(), verifyParameters( fb_type, fb_lowFreqHz, fb_highFreqHz, fb_nERBs, fb_nChannels,		
					fb_cfHz, fb_cfHz_length, fb_nGamma, fb_bwERBs), _magnitude, _gammatone) {
																											 
				this->verifyParameters( fb_type, fb_lowFreqHz, fb_highFreqHz, fb_nERBs, fb_nChannels, fb_cfHz, fb_cfHz_length, fb_nGamma, fb_bwERBs);

				this->upperProcPtr = upperProcPtr;				
				this->prepareForProcessing ();
			}
				
			GammatoneProc::~GammatoneProc () {
				this->leftFilters.clear();
				this->rightFilters.clear();
			}
			
			void GammatoneProc::processChunk ( ) {
				this->setNFR ( this->upperProcPtr->getNFR() );
				
				// Appending the chunk to process (the processing must be done on the PMZ)
				leftPMZ->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
				rightPMZ->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );
				
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > l_PMZ = leftPMZ->getLastChunkAccesor();
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > r_PMZ = rightPMZ->getLastChunkAccesor();
				
				std::thread leftThread( &GammatoneProc::processLR, this, std::ref(this->leftFilters), l_PMZ );
				std::thread rightThread( &GammatoneProc::processLR, this, std::ref(this->rightFilters), r_PMZ );
							
				leftThread.join();                // pauses until left finishes
				rightThread.join();               // pauses until right finishes
			}
			
			void GammatoneProc::prepareForProcessing () {

				this->populateFilters( leftFilters );
				this->populateFilters( rightFilters );
			}
			
			/* Comapres informations and the current parameters of two processors */
			bool GammatoneProc::operator==( GammatoneProc& toCompare ) {
				if ( this->compareBase( toCompare ) )
					if ( ( this->get_fb_type() == toCompare.get_fb_type() ) and
					     ( this->get_fb_lowFreqHz() == toCompare.get_fb_lowFreqHz() ) and
					     ( this->get_fb_highFreqHz() == toCompare.get_fb_highFreqHz() ) and	
					     ( this->get_fb_nERBs() == toCompare.get_fb_nERBs() ) and	     
					     ( this->get_fb_nChannels() == toCompare.get_fb_nChannels() ) and
					     ( this->get_fb_nGamma() == toCompare.get_fb_nGamma() ) and	
					     ( this->get_fb_bwERBs() == toCompare.get_fb_bwERBs() ) )
						return true;
				return false;
			}

			// getters
			const filterBankType GammatoneProc::get_fb_type() {return this->fb_type;}
			const double GammatoneProc::get_fb_lowFreqHz() {return this->fb_lowFreqHz;}
			const double GammatoneProc::get_fb_highFreqHz() {return this->fb_highFreqHz;}
			const double GammatoneProc::get_fb_nERBs() {return this->fb_nERBs;}
			const uint32_t GammatoneProc::get_fb_nChannels() {return this->fb_nChannels;}
			const uint32_t GammatoneProc::get_fb_nGamma() {return this->fb_nGamma;}
			const double GammatoneProc::get_fb_bwERBs() {return this->fb_bwERBs;}
			const double* GammatoneProc::get_fb_cfHz() {return this->cfHz.data();}

			// setters
			void GammatoneProc::set_fb_nGamma(const uint32_t arg) {this->fb_nGamma = arg; this->prepareForProcessing ();}
			void GammatoneProc::set_fb_bwERBs(const double arg) {this->fb_bwERBs = arg; this->prepareForProcessing ();}

			std::string GammatoneProc::get_upperProcName() {return this->upperProcPtr->getName();}
