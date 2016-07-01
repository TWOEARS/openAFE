#ifndef FFTENGINE_HPP
#define FFTENGINE_HPP

#include <fftw3.h>
#include <vector>
#include <complex>

namespace openAFE {

	class FFTEngine {
		
		private:
			std::size_t N;
			fftw_plan fft, ifft;

			fftw_complex *in, *out, *res;
			
			resize( std::size_t N );
 
		public:
        
			FFTEngine( std::size_t N );
			~FFTEngine( );
			
			std::complex<double> *fft( *std::complex<double>, std::size_t N );
			std::complex<double> *ifft( *std::complex<double>, std::size_t N );

	}; // class FFTEngine
}; // namespace openAFE

#endif // FFTENGINE_HPP
