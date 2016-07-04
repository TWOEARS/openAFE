#include <fftEngine.hpp>

using namespace openAFE;
using namespace std;

			FFTEngine::resize( std::size_t N ) {
				fftw_destroy_plan( this->fft );
				fftw_destroy_plan( this->ifft );
				
				 this->fft = fftw_plan_dft_1d(N, this->in, this->out, FFTW_FORWARD, FFTW_ESTIMATE);
				 this->ifft = fftw_plan_dft_1d(N, this->out, this->res, FFTW_BACKWARD, FFTW_ESTIMATE);				
				
			}
        
			FFTEngine::FFTEngine( std::size_t N ) {
				 this->N = N;
				 this->fft = fftw_plan_dft_1d(N, this->in, this->out, FFTW_FORWARD, FFTW_ESTIMATE);
				 this->ifft = fftw_plan_dft_1d(N, this->out, this->res, FFTW_BACKWARD, FFTW_ESTIMATE);
			}
			
			FFTEngine::~FFTEngine( );
			
			std::complex<double> FFTEngine::fft( std::complex<double> *inVect, std::size_t N ) {
				
				if ( N != this->N )
				this->resize( N );
				
				std::complex<double> outVect( N );
				
				this->in = reinterpret_cast<fftw_complex*>( inVec );
				this->out = reinterpret_cast<fftw_complex*>( outVec.data() );

				fftw_execute( this->fft );
				
				return outVect;
			}
			
			std::complex<double> FFTEngine::ifft( std::complex<double> *inVect, std::size_t N ) {
				
				if ( N != this->N )
				this->resize( N );

				std::complex<double> resultVect( N );

				this->out = reinterpret_cast<fftw_complex*>( inVect );
				this->res = reinterpret_cast<fftw_complex*>(resultVec.data());
				
				fftw_execute( this->ifft );
				
				return resultVect;							
			}
