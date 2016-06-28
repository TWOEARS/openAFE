#ifndef ILDPROC_HPP
#define ILDPROC_HPP

#include <assert.h>
	
#include "TFSProcessor.hpp"
#include "ihcProc.hpp"

#include "../tools/window.hpp"

/* 
 * ILD Proc :
 * 
 * 
 * */
 
namespace openAFE {

	enum ildWindowType {
        _hamming,
        _hanning,
        _hann,
        _blackman,
        _triang,
        _sqrt_win
	};
	
	class ILDProc : public TFSProcessor<double > {

		private:
		
			ildWindowType ild_wname;       		// Window shape descriptor (see window.m)
			double ild_wSizeSec;    			  	// Window duration in seconds
			double ild_hSizeSec;    			  	// Step size between windows in seconds

			uint64_t ild_wSize;														// Window duration in samples
			uint64_t ild_hSize;														// Step size between windows in samples
			std::vector<double> win;         										// Window vector
			std::shared_ptr <TimeFrequencySignal<double> > buffer_l, buffer_r;    	// Buffered input signals

			uint32_t fb_nChannels;
			
			std::shared_ptr<IHCProc > upperProcPtr;

			std::vector<double> zerosVector;
			std::shared_ptr<twoCTypeBlock<double> > zerosAccecor;
			
			// Output sampling frequency;
			uint32_t calcFsOut( double ild_hSizeSec );

			void prepareForProcessing();

			void processChannel( double* firstValue_l, double* firstValue_r, std::shared_ptr<twoCTypeBlock<double> > PMZ, size_t ii );
			
		public:
		
			ILDProc (const std::string nameArg, std::shared_ptr<IHCProc > upperProcPtr, double ild_wSizeSec = 0.02, double ild_hSizeSec = 0.01, ildWindowType ild_wname = _hann );

			~ILDProc ();
			
			void processChunk ();
			
			/* Comapres informations and the current parameters of two processors */
			bool operator==( ILDProc& toCompare );

			// getters
			const double get_ild_wSizeSec();
			const double get_ild_hSizeSec();
			const ildWindowType get_ild_wname();
			
			const uint32_t get_ild_nChannels();

			// setters			
			void set_ild_wSizeSec(const double arg);
			void set_ild_hSizeSec(const double arg);
			void set_ild_wname(const ildWindowType arg);

			std::string get_upperProcName();
			
	}; /* class ILDProc */
}; /* namespace openAFE */

#endif /* ILDPROC_HPP */
