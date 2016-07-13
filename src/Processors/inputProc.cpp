#include "inputProc.hpp"

using namespace openAFE;

			void InputProc::process ( double* firstValue, std::size_t dim ) {
				for ( unsigned int i = 0 ; i < dim ; ++i )
					*( firstValue + i ) = *( firstValue + i ) / this->in_normalizeValue;
			}

			InputProc::InputProc ( const std::string nameArg, const uint32_t fs, const double bufferSize_s, bool in_doNormalize, uint64_t in_normalizeValue ) : TDSProcessor<double> (nameArg, fs, fs, bufferSize_s, _inputProc) {
				if ( in_normalizeValue != 0 ) {
					this->in_doNormalize = in_doNormalize;
					this->in_normalizeValue = in_normalizeValue;
				}
			}
				
			InputProc::~InputProc () {	}
			
			/* This function does the asked calculations. 
			 * The inputs are called "privte memory zone". The asked calculations are
			 * done here and the results are stocked in that private memory zone.
			 * However, the results are not publiched yet on the output vectors.
			 */
			void InputProc::processChunk (double* inChunkLeft, std::size_t leftDim, double* inChunkRight, std::size_t rightDim ) {
				
				// Appending the chunk to process (the processing must be done on the PMZ)
				leftPMZ->appendTChunk( inChunkLeft, leftDim );
				rightPMZ->appendTChunk( inChunkRight, rightDim );
				
				std::shared_ptr<twoCTypeBlock<double> > l_PMZ = leftPMZ->getLastChunkAccesor();
				std::shared_ptr<twoCTypeBlock<double> > r_PMZ = rightPMZ->getLastChunkAccesor();

				if ( this-> in_doNormalize ) {
					// 0- Initialization
					unsigned long dim1_l = l_PMZ->array1.second;
					unsigned long dim2_l = l_PMZ->array2.second;
					unsigned long dim1_r = r_PMZ->array1.second;
					unsigned long dim2_r = r_PMZ->array2.second;
								
					double* firstValue1_l = l_PMZ->array1.first;
					double* firstValue2_l = l_PMZ->array2.first;
					double* firstValue1_r = r_PMZ->array1.first;
					double* firstValue2_r = r_PMZ->array2.first;				
						
					if ( ( dim1_l > 0 ) && ( dim1_r > 0 ) ) {
						std::thread leftThread( &InputProc::process, this, firstValue1_l, dim1_l );
						std::thread rightThread( &InputProc::process, this, firstValue1_r, dim1_r );
						
						leftThread.join();                // pauses until left finishes
						rightThread.join();               // pauses until right finishes
					}
					if ( ( dim2_l > 0 ) && ( dim2_r > 0 ) )	{
						std::thread leftThread( &InputProc::process, this, firstValue2_l, dim2_l );
						std::thread rightThread( &InputProc::process, this, firstValue2_r, dim2_r );
						
						leftThread.join();                // pauses until left finishes
						rightThread.join();               // pauses until right finishes
					}
				}
	
			}
			
			void InputProc::processChunk () { }
						
			void InputProc::prepareForProcessing () { }

			/* Comapres informations and the current parameters of two processors */
			bool InputProc::operator==( InputProc& toCompare ) {
				if ( this->compareBase( toCompare ) ) {
					if  ( ( this->get_in_doNormalize() == toCompare.get_in_doNormalize() ) and
					    ( this->get_in_normalizeValue() == toCompare.get_in_normalizeValue() ) )			
					return true;
				} return false;
			}
			
			bool InputProc::get_in_doNormalize() {return this->in_doNormalize;}
			uint64_t InputProc::get_in_normalizeValue() {return this->in_normalizeValue;}

			// setters			
			void InputProc::set_in_doNormalize(const bool arg) {this->in_doNormalize=arg;}
			void InputProc::set_in_normalizeValue(const uint64_t arg) { if ( arg > 0 ) this->in_normalizeValue=arg;}			
	
			std::string InputProc::get_upperProcName()	{return "BASS";}
