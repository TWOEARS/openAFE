#include <fftEngine.hpp>

using namespace openAFE;

			FFTEngine::resize( std::size_t N ) {
				
			}
        
			FFTEngine::FFTEngine( std::size_t N );
			FFTEngine::~FFTEngine( );
			
			std::complex<double> *FFTEngine::fft( *std::complex<double>, std::size_t N ) {
				
				p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
			}
			
			std::complex<double> *FFTEngine::ifft( *std::complex<double>, std::size_t N ) {
				
				
			}

