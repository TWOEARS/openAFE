#include "ihcProc.hpp"

#include <numeric>
#include <chrono>

using namespace openAFE;
using namespace std;

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

			void IHCProc::processHalfWave ( size_t& dim1_l, size_t& dim2_l, double* firstValue1_l, double* firstValue2_l, size_t& dim1_r, size_t& dim2_r, double* firstValue1_r, double* firstValue2_r ) {
						
				// Halfwave rectification
				size_t ii;
				for ( ii = 0 ; ii < dim1_l ; ++ii ) {
					*( firstValue1_l + ii ) = fmax( *( firstValue1_l + ii ), 0 );
					*( firstValue1_r + ii ) = fmax( *( firstValue1_r + ii ), 0 );					
				}
				
				if ( dim2_l > 0 ) {
					 for ( ii = 0 ; ii < dim2_l ; ++ii ) {
						*( firstValue2_l + ii ) = fmax( *( firstValue2_l + ii ), 0 );
						*( firstValue2_r + ii ) = fmax( *( firstValue2_r + ii ), 0 );						
					}	
				}
			}	
			
			void IHCProc::processFullWave ( size_t& dim1_l, size_t& dim2_l, double* firstValue1_l, double* firstValue2_l, size_t& dim1_r, size_t& dim2_r, double* firstValue1_r, double* firstValue2_r ) {
						
				// Halfwave rectification
				size_t ii;
				for ( ii = 0 ; ii < dim1_l ; ++ii ) {
					*( firstValue1_l + ii ) = fabs( *( firstValue1_l + ii ) );
					*( firstValue1_r + ii ) = fabs( *( firstValue1_r + ii ) );					
				}
				
				if ( dim2_l > 0 ) {
					 for ( ii = 0 ; ii < dim2_l ; ++ii ) {
						*( firstValue2_l + ii ) = fabs( *( firstValue2_l + ii ) );
						*( firstValue2_r + ii ) = fabs( *( firstValue2_r + ii ) );						
					}	
				}
			}				

			void IHCProc::processSquare ( size_t& dim1_l, size_t& dim2_l, double* firstValue1_l, double* firstValue2_l, size_t& dim1_r, size_t& dim2_r, double* firstValue1_r, double* firstValue2_r ) {
						
				// Halfwave rectification
				size_t ii;
				for ( ii = 0 ; ii < dim1_l ; ++ii ) {
					*( firstValue1_l + ii ) = pow( fabs( *( firstValue1_l + ii ) ), 2 );
					*( firstValue1_r + ii ) = pow( fabs( *( firstValue1_r + ii ) ), 2 );					
				}
				
				if ( dim2_l > 0 ) {
					 for ( ii = 0 ; ii < dim2_l ; ++ii ) {
						*( firstValue2_l + ii ) = pow( fabs( *( firstValue2_l + ii ) ), 2 );
						*( firstValue2_r + ii ) = pow( fabs( *( firstValue2_r + ii ) ), 2 );				
					}	
				}
			}					
															
			void IHCProc::processDAU ( size_t& dim1_l, size_t& dim2_l, double* firstValue1_l, double* firstValue2_l, bwFilterPtr filter_l, size_t& dim1_r, size_t& dim2_r, double* firstValue1_r, double* firstValue2_r, bwFilterPtr filter_r ) {
						
				// Halfwave rectification
				this->processHalfWave ( dim1_l, dim2_l, firstValue1_l, firstValue2_l, dim1_r, dim2_r, firstValue1_r, firstValue2_r );
								
				// Filtering
				filter_l->exec( firstValue1_l, dim1_l, firstValue1_l );
				filter_r->exec( firstValue1_r, dim1_r, firstValue1_r );
				
				if ( dim2_l > 0 ) {
					filter_l->exec( firstValue2_l, dim2_l, firstValue2_l );
					filter_r->exec( firstValue2_r, dim2_r, firstValue2_r );
				}
			}
																
			IHCProc::IHCProc (const std::string nameArg, std::shared_ptr<GammatoneProc > upperProcPtr, ihcMethod method ) : TFSProcessor<double > (nameArg, upperProcPtr->getFsOut(), upperProcPtr->getFsOut(), upperProcPtr->getBufferSize_s(), upperProcPtr->get_nChannel(), _magnitude, _ihc) {
					
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
								
				vector<shared_ptr<twoCTypeBlock<double> > > l_PMZ = leftPMZ->getLastChunkAccesor();
				vector<shared_ptr<twoCTypeBlock<double> > > r_PMZ = rightPMZ->getLastChunkAccesor();
				
				vector<thread> threads;
				for ( size_t ii = 0 ; ii < this->get_nChannel() ; ++ii ) {
					  
					  size_t dim1_l = l_PMZ[ii]->array1.second;
					  size_t dim2_l = l_PMZ[ii]->array2.second;
							
					  double* firstValue1_l = l_PMZ[ii]->array1.first;
					  double* firstValue2_l = l_PMZ[ii]->array2.first;

					  size_t dim1_r = r_PMZ[ii]->array1.second;
					  size_t dim2_r = r_PMZ[ii]->array2.second;
							
					  double* firstValue1_r = r_PMZ[ii]->array1.first;
					  double* firstValue2_r = r_PMZ[ii]->array2.first;
					  				
					  switch ( this->method ) {
						  case _none:
							break;
						  case _halfwave:
						    threads.push_back(thread( &IHCProc::processHalfWave, this, ref(dim1_l), ref(dim2_l), firstValue1_l, firstValue2_l, ref(dim1_r), ref(dim2_r), firstValue1_r, firstValue2_r ) );
							break;
						  case _fullwave:
						    threads.push_back(thread( &IHCProc::processFullWave, this, ref(dim1_l), ref(dim2_l), firstValue1_l, firstValue2_l, ref(dim1_r), ref(dim2_r), firstValue1_r, firstValue2_r ) );					 
							break;
					      case _square:
						    threads.push_back(thread( &IHCProc::processSquare, this, ref(dim1_l), ref(dim2_l), firstValue1_l, firstValue2_l, ref(dim1_r), ref(dim2_r), firstValue1_r, firstValue2_r ) );					 							
							break;
						  case _joergensen:
						  case _breebart:
						  case _bernstein:						  
						  case _dau:
							threads.push_back(thread( &IHCProc::processDAU, this, ref(dim1_l), ref(dim2_l), firstValue1_l, firstValue2_l, ref(this->ihcFilter_l[ii]), ref(dim1_r), ref(dim2_r), firstValue1_r, firstValue2_r, ref(this->ihcFilter_r[ii]) ) );
							break;
						  default :
							break;
					  }
				   }

				   // Waiting to join the threads
				   for (auto& t : threads)
					t.join();	
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
