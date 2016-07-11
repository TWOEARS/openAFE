#include "ihcProc.hpp"

using namespace openAFE;

			void IHCProc::populateFilters( filterPtrVector& filters ) {

				 filters.resize( this->get_nChannel() );

                 switch ( this->method ) {

                     case _joergensen:
                         // First order butterworth filter @ 150Hz
						 for ( size_t ii = 0 ; ii < this->get_nChannel() ; ++ii )
							filters[ii].reset(new bwFilter( this->getFsIn(), 1, 150, _bwlp ) );
                         break;

                     case _dau:
                         // Second order butterworth filter @ 1000Hz
						 for ( size_t ii = 0 ; ii < this->get_nChannel() ; ++ii )
							filters[ii].reset(new bwFilter( this->getFsIn(), 2, 1000, _bwlp ) );						 
                         break;

                     case _breebart:
                         // First order butterworth filter @ 2000Hz cascaded 5 times
						 for ( size_t ii = 0 ; ii < this->get_nChannel() ; ++ii ) {
							 // TODO : implement THIS
						 }
                         break;

                     case _bernstein:
                         // Second order butterworth filter @ 425Hz
						 for ( size_t ii = 0 ; ii < this->get_nChannel() ; ++ii ) {
							filters[ii].reset(new bwFilter( this->getFsIn(), 2, 425, _bwlp ) );
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
				size_t dim1 = oneChannel->array1.second;
				size_t dim2 = oneChannel->array2.second;
							
				double* firstValue1 = oneChannel->array1.first;
				double* firstValue2 = oneChannel->array2.first;
			
                 switch ( this->method ) {

                     case _joergensen:
						// TODO : implement THIS
						break;
					 case _dau:
					 
						// Halfwave rectification
						if ( dim1 > 0 )
							for ( size_t ii = 0 ; ii < dim1 ; ++ii )
								*( firstValue1 + ii ) = fmax( *( firstValue1 + ii ), 0 );
						if ( dim2 > 0 )
							for ( size_t ii = 0 ; ii < dim2 ; ++ii )
								*( firstValue2 + ii ) = fmax( *( firstValue2 + ii ), 0 );
								
						// Filtering
						if ( dim1 > 0 )
						    filter->exec( firstValue1, dim1, firstValue1 );
						if ( dim2 > 0 )
						    filter->exec( firstValue2, dim2, firstValue2 );

					 	break;
					 	
					 case _breebart:
						// TODO : implement THIS (filters are not initialized)

					 	break;
					 	
					 case _bernstein:
						// TODO : implement THIS

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
				  for ( size_t ii = 0 ; ii < this->get_nChannel() ; ++ii ) {
					  if ( ( ( this->method == _joergensen ) or ( this->method == _dau ) or ( this->method == _breebart ) or ( this->method == _bernstein ) ) and ( filters.size() > 0 ) )
						threads.push_back(std::thread( &IHCProc::processFilteringChannel, this, filters[ii], PMZ[ii] ));
					  else threads.push_back(std::thread( &IHCProc::processChannel, this, PMZ[ii] ));
				   }

				  // Waiting to join the threads
				  for (auto& t : threads)
					t.join();
			}
																
			IHCProc::IHCProc (const std::string nameArg, std::shared_ptr<GammatoneProc > upperProcPtr, ihcMethod method ) : TFSProcessor<double > (nameArg, upperProcPtr->getFsOut(), upperProcPtr->getFsOut(), upperProcPtr->getBufferSize_s(), upperProcPtr->get_fb_nChannels(), _magnitude, _ihc) {
					
				this->upperProcPtr = upperProcPtr;
				this->method = method;
				this->prepareForProcessing();																							 
			}
			
			IHCProc::~IHCProc () {
				
				this->ihcFilter_l.clear();
				this->ihcFilter_r.clear();
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
				
				std::thread leftThread( &IHCProc::processLR, this, std::ref(this->ihcFilter_l), l_PMZ );
				std::thread rightThread( &IHCProc::processLR, this, std::ref(this->ihcFilter_r), r_PMZ );
							
				leftThread.join();                // pauses until left finishes
				rightThread.join();               // pauses until right finishes			
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
			
			// setters
			void IHCProc::set_ihc_method(const ihcMethod arg) {this->method=arg; this->prepareForProcessing ();}

			std::string IHCProc::get_upperProcName() {return this->upperProcPtr->getName();}
