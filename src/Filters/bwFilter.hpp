#ifndef BWFILTER_HPP
#define BWFILTER_HPP

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <vector>
#include <assert.h>

#include "CascadeFilters.hpp"

namespace openAFE {

	class bwFilter : public CascadeFilters < double, double, double, double > {
		
		uint32_t order;
		
		uint32_t cascadeNumber( uint32_t argOrder );
				
		public:
				
			bwFilter(double fs, unsigned short argOrder = 2, float f1 = 1000, bwType type = _bwlp, float f2 = 0);
			
			~bwFilter();
			
	};
};

#endif /* BWFILTER_HPP */
