#include "ihcProc.hpp"

using namespace openAFE;

			void IHCProc::populateFilters( filterPtrVector& filters ) {

                 switch ( this->method ) {

                     case _joergensen:
						 ihcFilter_l.resize( fb_nChannels ); ihcFilter_r.resize( fb_nChannels );
                         // First order butterworth filter @ 150Hz
						 for ( size_t ii = 0 ; ii < fb_nChannels ; ++ii ) {

						 }
						 
                         break;

                     case _dau:
						 ihcFilter_l.resize( fb_nChannels ); ihcFilter_r.resize( fb_nChannels );
                         // First order butterworth filter @ 150Hz
						 for ( size_t ii = 0 ; ii < fb_nChannels ; ++ii ) {
							 
						 }
                         break;

                     case _breebart:
                         // First order butterworth filter @ 2000Hz cascaded 5
                         // times
						 ihcFilter_l.resize( fb_nChannels ); ihcFilter_r.resize( fb_nChannels );
                         // First order butterworth filter @ 150Hz
						 for ( size_t ii = 0 ; ii < fb_nChannels ; ++ii ) {
							 
						 }
                         break;

                     case _bernstein:
                         // Second order butterworth filter @ 425Hz
						 ihcFilter_l.resize( fb_nChannels ); ihcFilter_r.resize( fb_nChannels );
						 for ( size_t ii = 0 ; ii < fb_nChannels ; ++ii ) {
							 
						 }
                         break;

                     default:
                         this->ihcFilter_l.clear();
                         this->ihcFilter_r.clear();
                         break;

				}	

			}

			void IHCProc::processFilteringChannel ( bwFilterPtr filter, std::shared_ptr<twoCTypeBlock<double> > oneChannel ) {
				// 0- Initialization
			/*	size_t dim1 = oneChannel->array1.second;
				size_t dim2 = oneChannel->array2.second;
							
				double* firstValue1 = oneChannel->array1.first;
				double* firstValue2 = oneChannel->array2.first;
			*/
                 switch ( this->method ) {

                     case _joergensen:
						
						break;
					 case _dau:

					 	break;
					 	
					 case _breebart:
					 
					 	break;
					 	
					 case _bernstein:

					 	break;
					 	
					 default:
					 
						break;
				}					
			}
			
			void IHCProc::processChannel ( std::shared_ptr<twoCTypeBlock<double> > oneChannel ) {
				// 0- Initialization
				size_t dim1 = oneChannel->array1.second;
				size_t dim2 = oneChannel->array2.second;
							
				double* firstValue1 = oneChannel->array1.first;
				double* firstValue2 = oneChannel->array2.first;

                 switch ( this->method ) {

                     case _none:
						// Nothing
						break;
					 case _halfwave:

						if ( dim1 > 0 )
							for ( size_t ii = 0 ; ii < dim1 ; ++ii )
								*( firstValue1 + ii ) = fmax( *( firstValue1 + ii ), 0 );
						if ( dim2 > 0 )
							for ( size_t ii = 0 ; ii < dim2 ; ++ii )
								*( firstValue2 + ii ) = fmax( *( firstValue2 + ii ), 0 );
					 	break;
					 	
					 case _fullwave:
					 
						if ( dim1 > 0 )
							for ( size_t ii = 0 ; ii < dim1 ; ++ii )
								*( firstValue1 + ii ) = fabs( *( firstValue1 + ii ) );
						if ( dim2 > 0 )	
							for ( size_t ii = 0 ; ii < dim2 ; ++ii )
								*( firstValue2 + ii ) = fabs( *( firstValue2 + ii ) );	 
					 	break;
					 	
					 case _square:

						if ( dim1 > 0 )
							for ( size_t ii = 0 ; ii < dim1 ; ++ii )
								*( firstValue1 + ii ) = pow( fabs( *( firstValue1 + ii ) ), 2 );
						if ( dim2 > 0 )	
							for ( size_t ii = 0 ; ii < dim2 ; ++ii )
								*( firstValue2 + ii ) = pow( fabs( *( firstValue2 + ii ) ), 2 );						 
					 	break;
					 	
					 default:
						break;
				}					
			}
			
			void IHCProc::processLR ( filterPtrVector& filters, std::vector<std::shared_ptr<twoCTypeBlock<double> > > PMZ ) {
				std::vector<std::thread> threads;
				  for ( size_t ii = 0 ; ii < this->fb_nChannels ; ++ii ) {
					  if ( ( ( this->method == _joergensen ) or ( this->method == _dau ) or ( this->method == _breebart ) or ( this->method == _bernstein ) ) and ( filters.size() > 0 ) )
						threads.push_back(std::thread( &IHCProc::processFilteringChannel, this, filters[ii], PMZ[ii] ));
					  else threads.push_back(std::thread( &IHCProc::processChannel, this, PMZ[ii] ));
				   }

				  // Waiting to join the threads
				  for (auto& t : threads)
					t.join();
			}
																
			IHCProc::IHCProc (const std::string nameArg, std::shared_ptr<GammatoneProc > upperProcPtr, ihcMethod method ) : TFSProcessor<double > (nameArg, upperProcPtr->getFsOut(), upperProcPtr->getFsOut(), upperProcPtr->getBufferSize_s(), upperProcPtr->get_fb_nChannels(), "magnitude", _ihc) {
					
				this->fb_nChannels = upperProcPtr->get_fb_nChannels();
				this->upperProcPtr = upperProcPtr;
				this->method = method;
				this->prepareForProcessing();																							 
			}
			
			IHCProc::~IHCProc () {
				
				ihcFilter_l.clear();
				ihcFilter_r.clear();
			}
			
			void IHCProc::prepareForProcessing() {
				this->populateFilters( this->ihcFilter_l );
				this->populateFilters( this->ihcFilter_r );
			}
			
			void IHCProc::processChunk ( ) {
				this->setNFR ( this->upperProcPtr->getNFR() );

				// Appending the chunk to process (the processing must be done on the PMZ)
				leftPMZ->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
				rightPMZ->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > l_PMZ = leftPMZ->getLastChunkAccesor();
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > r_PMZ = rightPMZ->getLastChunkAccesor();
				
				this->processLR( ihcFilter_l, l_PMZ );
				this->processLR( ihcFilter_r, r_PMZ );				
			}
			
			/* Comapres informations and the current parameters of two processors */
			bool IHCProc::operator==( IHCProc& toCompare ) {
				if ( this->compareBase( toCompare ) )
					if ( this->get_ihc_method() == toCompare.get_ihc_method() )				     		     			     
						return true;
				return false;
			}

			// getters
			const ihcMethod IHCProc::get_ihc_method() {return this->method;}  
			const uint32_t IHCProc::get_ihc_nChannels() {return this->fb_nChannels;}

			// setters			
			void IHCProc::set_ihc_method(const ihcMethod arg) {this->method=arg; this->prepareForProcessing ();}

			std::string IHCProc::get_upperProcName() {return this->upperProcPtr->getName();}
