#include "crossCorrelation.hpp"

using namespace openAFE;

			std::size_t CrossCorrelation::getNLags( double maxDelaySec, double fs ) {
				std::size_t maxLags = ceil( maxDelaySec * fs );
				std::size_t lagN = maxLags *2 + 1;
				
				this->lags.resize( lagN );
				
				this->lags = linspace<double>(-maxLags, maxLags, lagN);
				for ( size_t i = 0 ; i < lagN ; ++i )
					this->lags[i] /= fs;
					
				return lagN;
			}

			void CrossCorrelation::prepareForProcessing() {
				this->getNLags( this->maxDelaySec, this->getFsIn() );
			}

			void CrossCorrelation::processChannel( double* firstValue_l, double* firstValue_r, double *result ) {
				
				
			}
			
			CrossCorrelation::CrossCorrelation (const std::string nameArg, std::shared_ptr<IHCProc > upperProcPtr, double wSizeSec, double hSizeSec, double decaySec, double maxDelaySec, windowType wname )
			: WindowBasedLAGProcs (nameArg, upperProcPtr, _crosscorrelation, this->getNLags( maxDelaySec, upperProcPtr->getFsOut() ), wSizeSec, hSizeSec, wname ) {
				
				this->maxDelaySec = maxDelaySec;
				
				this->prepareForProcessing();
			}

			CrossCorrelation::~CrossCorrelation () {
				
			}
			
			void CrossCorrelation::processChunk () {	

				this->setNFR ( this->upperProcPtr->getNFR() );
				
				
				
			}
/*			
			// getters
			const double CrossCorrelation::get_cc_decaySec() {return this->decaySec;}
			const scalingType CrossCorrelation::get_cc_lags() {return this->scailing;}
  
			// setters			
			void CrossCorrelation::set_cc_lags(const double arg) {this->decaySec=arg; this->prepareForProcessing ();}
			void CrossCorrelation::set_cc_maxDelaySec(const scalingType arg) {this->scailing=arg; this->prepareForProcessing ();}
*/
