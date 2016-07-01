#ifndef CROSSCORRELATION_HPP
#define CROSSCORRELATION_HPP
	
#include "windowBasedLAGProcs.hpp"
#include "../tools/mathTools.hpp"

/// CROSSCORRELATIONPROC Cross-correlation processor.
///   The cross-correlation between the right and left ears is computed from
///   their respective Inner Hair-Cell representations, in the Fourier 
///   domain for given time frames. This is normalized by the
///   auto-correlation sequence at lag zero, and then evaluated for time lags
///   in a given range, resulting in a three-dimensional function of time
///   frame, frequency channel and lag time.
 
namespace openAFE {

	class CrossCorrelation : public WindowBasedLAGProcs {

		private:

            double maxDelaySec; // Maximum delay in cross-correlation computation (s)
            std::vector<double > lags; // Vector of lags at which cross-correlation is computed

			std::size_t getNLags( double maxDelaySec, double fs );

			void prepareForProcessing();

			void processChannel( double* firstValue_l, double* firstValue_r, double *result );
			
		public:
		
			CrossCorrelation (const std::string nameArg, std::shared_ptr<IHCProc > upperProcPtr, double wSizeSec = 0.02, double hSizeSec = 0.01, double decaySec = 0.008, double maxDelaySec = 0.0011, windowType wname = _hann );

			~CrossCorrelation ();
			
			void processChunk ();
/*			
			// getters
			const double get_cc_maxDelaySec();
			const std::vector<double >& get_cc_lags();
  
			// setters			
			void set_cc_maxDelaySec(const double arg);
			void set_cc_lags(const std::vector<double >& arg);
*/
			
	}; /* class CrossCorrelation */
}; /* namespace openAFE */

#endif /* CROSSCORRELATION_HPP */
