#include "bwFilter.hpp"

using namespace openAFE;

		uint32_t bwFilter::cascadeNumber( uint32_t argOrder ) {
			return std::floor( argOrder / 2 ) + ( argOrder % 2 ) );
		}

		bwFilter::bwFilter(double fs, uint32_t argOrder = 2, double f1 = 1000, bwType type = _bwlp, float f2 = 0) : CascadeFilters < double, double, double, double > ( cascadeNumber( argOrder ) {
			
			std::vector<double> vectDcof(3, 0);
			std::vector<double> vectCcof(3, 0);

			// The coeffs of a second order filter
			bwCoef(2, fs, f1, vectDcof, vectCcof, type, f2);
			
			for ( std::size_t ii = 0 ; ii < std::floor( argOrder / 2 ) ; ++ii)
				this->setFilter( vectDcof.data(), vectDcof.size(), vectCcof.data(), vectCcof.size() );
			
			// numFilter-1 can be biquiad or 1st order
			apf::BiQuad< T > *filter = new apf::BiQuad< T >;
			
			if ( argOrder % 2 == 1 ) {
				// Than we get the coeffs of a first order filter
				std::fill(vectDcof.begin(), vectDcof.end(), 0);
				std::fill(vectCcof.begin(), vectCcof.end(), 0);

				bwCoef(1, fs, f1, vectDcof, vectCcof, type, f2);
				
				this->setFilter( vectDcof.data(), vectDcof.size(), vectCcof.data(), vectCcof.size() );
			}
		}
		
		bwFilter::~bwFilter() {
			
		}
		
	};
};

#endif /* BWFILTER_HPP */
