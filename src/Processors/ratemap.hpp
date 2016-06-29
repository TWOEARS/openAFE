#ifndef RATEMAP_HPP
#define RATEMAP_HPP
	
#include "windowBasedProcs.hpp"

#include "../Filters/leakyIntegratorFilter.hpp"

/* 
 * RATEMAP Proc :
 * 
 * 
 * */
 
namespace openAFE {

	class Ratemap : public WindowBasedProcs {

		private:

			typedef std::shared_ptr< leakyIntegratorFilter > leakyIntegratorFilterPtr;
            typedef std::vector <leakyIntegratorFilterPtr > filterPtrVector;
            
			filterPtrVector rmFilter_l, rmFilter_r;			

			void prepareFilters();

			void processChannel( double* firstValue_l, double* firstValue_r, std::shared_ptr<twoCTypeBlock<double> > PMZ, std::size_t ii );
			
		public:
		
			Ratemap (const std::string nameArg, std::shared_ptr<IHCProc > upperProcPtr, double ild_wSizeSec = 0.02, double ild_hSizeSec = 0.01, ildWindowType ild_wname = _hann );

			~Ratemap ();
			
			void processChunk ();
			
	}; /* class Ratemap */
}; /* namespace openAFE */

#endif /* RATEMAP_HPP */
