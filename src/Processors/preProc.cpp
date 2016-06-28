#include "preProc.hpp"

using namespace openAFE;
	
            void PreProc::verifyParameters() {
				
				/* TODO : Follow Matlab AFE to update this function
				 * Nothing at : 17.02.2016
				 */
				
			}
						
			// Actual Processing
			void PreProc::process ( double* firstValue_l, std::size_t dim_l, double* firstValue_r, std::size_t dim_r ) {
			
					// 1- DC-removal filter
					if ( this->pp_bRemoveDC ) {
						std::thread leftThread1( &bwFilter::exec, this->dcFilter_l, firstValue_l, dim_l , firstValue_l );
						std::thread rightThread1( &bwFilter::exec, this->dcFilter_r, firstValue_r, dim_r , firstValue_r );
							
						leftThread1.join();                // pauses until left finishes
						rightThread1.join();               // pauses until right finishes
					}

					// 2- Pre-whitening
					if ( this->pp_bPreEmphasis ) {
						std::thread leftThread1( &GenericFilter<double,double, double, double>::exec, this->preEmphFilter_l, firstValue_l, dim_l , firstValue_l );
						std::thread rightThread1( &GenericFilter<double,double, double, double>::exec, this->preEmphFilter_r, firstValue_r, dim_r , firstValue_r );
							
						leftThread1.join();                // pauses until left finishes
						rightThread1.join();               // pauses until right finishes		
					}

					// 3- Automatic gain control	
					if ( this->pp_bNormalizeRMS ) {

						// Initialize the filter states if empty
						if ( !( agcFilter_l->isInitialized() ) ) {

							double intArg = this->pp_intTimeSecRMS * this->getFsIn();
							double sum_l = 0, sum_r = 0, s0_l, s0_r;
							uint32_t minVal_l, minVal_r;
							
							minVal_l = fmin ( dim_l, round( intArg ) );
							minVal_r = fmin ( dim_r, round( intArg ) );

							// Mean square of input over the time constant
							for ( unsigned int i = 0 ; i < minVal_l ; ++i ) {
								sum_l += pow( *(firstValue_l + i ) , 2);
								sum_r += pow( *(firstValue_r + i ) , 2);
							}
								
							// Initial filter states
							s0_l = exp ( -1 / intArg ) * ( sum_l / minVal_l );							
							s0_r = exp ( -1 / intArg ) * ( sum_r / minVal_r );
							
							this->agcFilter_l->reset( &s0_l, 1 );
							this->agcFilter_r->reset( &s0_r, 1 );
						}
						
						// Estimate normalization constants
						std::vector<double> tmp_l ( dim_l ), tmp_r ( dim_r );
						for ( unsigned int i = 0 ; i < dim_l ; ++i ) {
							tmp_l[ i ] = pow(*( firstValue_l + i ), 2);
							tmp_r[ i ] = pow(*( firstValue_r + i ), 2);
						}
						
						double normFactor;
						
						std::thread leftThread1( &GenericFilter<double,double, double, double>::exec, this->agcFilter_l, tmp_l.data(), dim_l , tmp_l.data() );
						std::thread rightThread1( &GenericFilter<double,double, double, double>::exec, this->agcFilter_r, tmp_r.data(), dim_r , tmp_r.data() );
							
						leftThread1.join();                // pauses until left finishes
						rightThread1.join();               // pauses until right finishes
						
						for ( unsigned int i = 0 ; i < dim_l ; ++i ) {
							tmp_l[ i ] = sqrt( tmp_l[ i ] ) + EPSILON;
							tmp_r[ i ] = sqrt( tmp_r[ i ] ) + EPSILON;
							normFactor = fmax ( tmp_l[ i ], tmp_r[ i ] );
							
							*(firstValue_l + i ) /= normFactor;
							*(firstValue_r + i ) /= normFactor;
						}
					}
					
					// 4- Level Scaling
					if ( this->pp_bLevelScaling ) {
						
						double current_dboffset = 100; // dbspl(1) = 100;
						double dbVar = pow( 10 , ( current_dboffset - this->pp_refSPLdB ) / 20 );

						for ( unsigned int i = 0 ; i < dim_l ; ++i ) {
							*(firstValue_l + i ) *= dbVar;
							*(firstValue_r + i ) *= dbVar;
						}
					}
					
					// 5- Middle Ear Filtering
					if ( this->pp_bUnityComp ) {
						
						switch ( this->pp_middleEarModel ) {
							case _jepsen:
								this->meFilterPeakdB = 55.9986;
								break;
							case _lopezpoveda:
								this->meFilterPeakdB = 66.2888;
								break;
							default:
					 			this->meFilterPeakdB = 0;
								break;
						}
					} else this->meFilterPeakdB = 0;
					
					if ( this->pp_bMiddleEarFiltering ) {
						//	TODO 
					}
													
					// Processed data is on PMZ					
			}
		
			/* PreProc */
			PreProc::PreProc (const std::string nameArg, std::shared_ptr<InputProc > upperProcPtr, bool pp_bRemoveDC,
																						  double pp_cutoffHzDC,
																						  bool pp_bPreEmphasis,
																						  double pp_coefPreEmphasis,
																						  bool pp_bNormalizeRMS,
																						  double pp_intTimeSecRMS,
																						  bool pp_bLevelScaling,
																						  double pp_refSPLdB,
																						  bool pp_bMiddleEarFiltering,
																						  middleEarModel pp_middleEarModel,
																						  bool pp_bUnityComp
					) : TDSProcessor<double> (nameArg, upperProcPtr->getFsOut(), upperProcPtr->getFsOut(), upperProcPtr->getBufferSize_s(), _inputProc) {

				this->pp_bRemoveDC = pp_bRemoveDC;
				this->pp_cutoffHzDC = pp_cutoffHzDC;
				this->pp_bPreEmphasis = pp_bPreEmphasis;
				this->pp_coefPreEmphasis =  pp_coefPreEmphasis;
				this->pp_bNormalizeRMS = pp_bNormalizeRMS;
				this->pp_intTimeSecRMS = pp_intTimeSecRMS;
				this->pp_bLevelScaling = pp_bLevelScaling;
				this->pp_refSPLdB = pp_refSPLdB;
				this->pp_bMiddleEarFiltering = pp_bMiddleEarFiltering;
				this->pp_middleEarModel = pp_middleEarModel;
				this->pp_bUnityComp = pp_bUnityComp;

				this->upperProcPtr = upperProcPtr;
				
				this->verifyParameters();
				this->prepareForProcessing ();
			}
				
			PreProc::~PreProc () {
				this->dcFilter_l.reset();
				this->dcFilter_r.reset();				
				
				this->preEmphFilter_l.reset();
				this->preEmphFilter_r.reset();
			
				this->agcFilter_l.reset();
				this->agcFilter_r.reset();

				this->midEarFilter_l.reset();
				this->midEarFilter_r.reset();
			}
			
			void PreProc::processChunk () {
	
					this->setNFR ( upperProcPtr->getNFR() ); /* for rosAFE */

					// Appending the chunk to process (the processing must be done on the PMZ)
					leftPMZ->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
					rightPMZ->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );
	
					std::shared_ptr<twoCTypeBlock<double> > l_PMZ = leftPMZ->getLastChunkAccesor();
					std::shared_ptr<twoCTypeBlock<double> > r_PMZ = rightPMZ->getLastChunkAccesor();
					
					// 0- Initialization
					std::size_t dim1_l = l_PMZ->array1.second;
					std::size_t dim2_l = l_PMZ->array2.second;
					std::size_t dim1_r = r_PMZ->array1.second;
					std::size_t dim2_r = r_PMZ->array2.second;
							
					double* firstValue1_l = l_PMZ->array1.first;
					double* firstValue2_l = l_PMZ->array2.first;
					double* firstValue1_r = r_PMZ->array1.first;
					double* firstValue2_r = r_PMZ->array2.first;				
					
					if ( ( dim1_l > 0 ) && ( dim1_r > 0 ) )
						process ( firstValue1_l, dim1_l, firstValue1_r, dim1_r );
					if ( ( dim2_l > 0 ) && ( dim2_r > 0 ) )	
						process ( firstValue2_l, dim2_l, firstValue2_r, dim2_r );
			}
			
			void PreProc::prepareForProcessing () {

				// Filter instantiation (if needed)	
				if ( this->pp_bRemoveDC ) {
					
					this->dcFilter_l.reset ( new bwFilter ( this->getFsIn(), 4 /* order */, this->pp_cutoffHzDC, (bwType)1 /* High */ ) );
					this->dcFilter_r.reset ( new bwFilter ( this->getFsIn(), 4 /* order */, this->pp_cutoffHzDC, (bwType)1 /* High */ ) );
					
				} else {
					// Deleting the filter objects
					this->dcFilter_l.reset();
					this->dcFilter_r.reset();
				}

				if ( this->pp_bPreEmphasis ) {
					
					std::vector<double> vectB (2,1);
					vectB[1] = -1 * fabs( this->pp_coefPreEmphasis );
					std::vector<double> vectA (1,1);
					
					this->preEmphFilter_l.reset ( new GenericFilter<double,double, double, double> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					this->preEmphFilter_r.reset ( new GenericFilter<double,double, double, double> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					
				} else {
					
					// Deleting the filter objects
					this->preEmphFilter_l.reset();
					this->preEmphFilter_r.reset();
				}
				
				if ( this->pp_bNormalizeRMS ) {

					std::vector<double> vectB (1,1);
					std::vector<double> vectA (2,1);
					
					vectA[1] = -1 * exp( -1 / ( this->pp_intTimeSecRMS * this->getFsIn() ) );
					vectB[0] = vectA[0] + vectA[1];

					this->agcFilter_l.reset ( new GenericFilter<double,double, double, double> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					this->agcFilter_r.reset ( new GenericFilter<double,double, double, double> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					
				} else {
					
					// Deleting the filter objects
					this->agcFilter_l.reset();
					this->agcFilter_r.reset();
				}

				if ( this->pp_bMiddleEarFiltering ) {

					 //this->midEarFilter_l.reset ( new GenericFilter<double,double, double, double> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					 //this->midEarFilter_r.reset ( new GenericFilter<double,double, double, double> ( vectB.data(), vectB.size(), vectA.data(), vectA.size() ) );
					
				} else {
					
					// Deleting the filter objects
					this->midEarFilter_l.reset();
					this->midEarFilter_r.reset();
				}			
			}

			/* Comapres informations and the current parameters of two processors */
			bool PreProc::operator==( PreProc& toCompare ) {
				if ( this->compareBase( toCompare ) )
					if ( ( this->get_pp_bRemoveDC() == toCompare.get_pp_bRemoveDC() ) and
					     ( this->get_pp_cutoffHzDC() == toCompare.get_pp_cutoffHzDC() ) and
					     ( this->get_pp_bPreEmphasis() == toCompare.get_pp_bPreEmphasis() ) and	
					     ( this->get_pp_bNormalizeRMS() == toCompare.get_pp_bNormalizeRMS() ) and	     
					     ( this->get_pp_intTimeSecRMS() == toCompare.get_pp_intTimeSecRMS() ) and
					     ( this->get_pp_bLevelScaling() == toCompare.get_pp_bLevelScaling() ) and	
					     ( this->get_pp_refSPLdB() == toCompare.get_pp_refSPLdB() ) and
					     ( this->get_pp_bMiddleEarFiltering() == toCompare.get_pp_bMiddleEarFiltering() ) and
					     ( this->get_pp_middleEarModel() == toCompare.get_pp_middleEarModel() ) and	
					     ( this->get_pp_bUnityComp() == toCompare.get_pp_bUnityComp() ) )					     		     			     
						return true;
				return false;
			}

			// getters
			bool PreProc::get_pp_bRemoveDC() {return this->pp_bRemoveDC;}
			double PreProc::get_pp_cutoffHzDC() {return this->pp_cutoffHzDC;}
			bool PreProc::get_pp_bPreEmphasis() {return this->pp_bPreEmphasis;}
			double PreProc::get_pp_coefPreEmphasis() {return this->pp_coefPreEmphasis;}
			bool PreProc::get_pp_bNormalizeRMS() {return this->pp_bNormalizeRMS;}
			double PreProc::get_pp_intTimeSecRMS() {return this->pp_intTimeSecRMS;}
			bool PreProc::get_pp_bLevelScaling() {return this->pp_bLevelScaling;}
			double PreProc::get_pp_refSPLdB() {return this->pp_refSPLdB;}
			bool PreProc::get_pp_bMiddleEarFiltering() {return this->pp_bMiddleEarFiltering;}
			middleEarModel PreProc::get_pp_middleEarModel() {return this->pp_middleEarModel;}
			bool PreProc::get_pp_bUnityComp() {return this->pp_bUnityComp;}

			// setters			
			void PreProc::set_pp_bRemoveDC(const bool arg) {this->pp_bRemoveDC=arg; this->prepareForProcessing ();}
			void PreProc::set_pp_cutoffHzDC(const double arg) {this->pp_cutoffHzDC=arg; this->prepareForProcessing ();}
			void PreProc::set_pp_bPreEmphasis(const bool arg) {this->pp_bPreEmphasis=arg; this->prepareForProcessing ();}
			void PreProc::set_pp_coefPreEmphasis(const double arg) {this->pp_coefPreEmphasis = arg; this->prepareForProcessing ();}
			void PreProc::set_pp_bNormalizeRMS(const bool arg) {this->pp_bNormalizeRMS=arg; this->prepareForProcessing ();}
			void PreProc::set_pp_intTimeSecRMS(const double arg) {this->pp_intTimeSecRMS=arg; this->prepareForProcessing ();}
			void PreProc::set_pp_bLevelScaling(const bool arg) {this->pp_bLevelScaling=arg; this->prepareForProcessing ();}
			void PreProc::set_pp_refSPLdB(const double arg) {this->pp_refSPLdB=arg; this->prepareForProcessing ();}
			void PreProc::set_pp_bMiddleEarFiltering(const bool arg) {this->pp_bMiddleEarFiltering=arg; this->prepareForProcessing ();}
			void PreProc::set_pp_middleEarModel(const middleEarModel arg) {this->pp_middleEarModel=arg; this->prepareForProcessing ();}
			void PreProc::set_pp_bUnityComp(const bool arg) {this->pp_bUnityComp=arg; this->prepareForProcessing ();}			

			std::string PreProc::get_upperProcName() {return this->upperProcPtr->getName();}

