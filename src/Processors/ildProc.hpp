#ifndef ILDPROC_HPP
#define ILDPROC_HPP
	
#include "windowBasedProcs.hpp"

/* 
 * ILD Proc :
 * 
 * 
 * */
 
namespace openAFE {
	
	class ILDProc : public WindowBasedProcs {

		private:

			void processChannel( double* firstValue_l, double* firstValue_r, double *result );
			
		public:
		
			ILDProc (const std::string nameArg, std::shared_ptr<IHCProc > upperProcPtr, double wSizeSec = 0.02, double hSizeSec = 0.01, windowType wname = _hann );

			~ILDProc ();
			
			void processChunk ();

	}; /* class ILDProc */
}; /* namespace openAFE */

#endif /* ILDPROC_HPP */
