#include "bwFilter.hpp"

#include <iostream>


		uint32_t openAFE::bwFilter::cascadeNumber( uint32_t argOrder ) {
			return ( std::floor( argOrder / 2 ) + ( argOrder % 2 ) );
		}

		openAFE::bwFilter::bwFilter(double fs, uint32_t argOrder, double f1, openAFE::bwType type, double f2) : CascadeFilters < double, double, double, double > ( cascadeNumber( argOrder ) ) {
			
			std::vector<double> vectDcof(3, 0);
			std::vector<double> vectCcof(3, 0);

			this->order = argOrder;
			
			if ( argOrder > 1 ) {
				// The coeffs of a second order filter
				bwCoef(2, fs, f1, vectDcof, vectCcof, type, f2);
				
				for ( std::size_t ii = 0 ; ii < std::floor( argOrder / 2 ) ; ++ii)
					this->setFilter( vectCcof.data(), vectCcof.size(), vectDcof.data(), vectDcof.size() );
			}
			
			if ( argOrder % 2 == 1 ) {

				// Than we get the coeffs of a first order filter
				std::fill(vectDcof.begin(), vectDcof.end(), 0);
				std::fill(vectCcof.begin(), vectCcof.end(), 0);

				bwCoef(1, fs, f1, vectDcof, vectCcof, type, f2);
				
				this->setFilter( vectDcof.data(), vectDcof.size(), vectCcof.data(), vectCcof.size() );
			}
		}
		
		openAFE::bwFilter::~bwFilter() {
			
		}
