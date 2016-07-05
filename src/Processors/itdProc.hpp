#ifndef ITDPROC_HPP
#define ITDPROC_HPP

#include <memory>

#include "TFSProcessor.hpp"	
#include "crossCorrelation.hpp"

#include "../tools/mathTools.hpp"

/// ITDPROC Interaural Time Difference processor.
///    This processor estimates the time difference between the left and the
///    right ear signals for individual frequency channels and time frames by
///    locating the time lag that corresponds to the most prominent peak in
///    the normalized cross-correlation function. This estimation is further 
///    refined by a parabolic interpolation stage [1].
/// 
///    Reference:
///    [1] May, T., van de Par, S., and Kohlrausch, A. (2011), "A probabilistic 
///        model for robust localization based on a binaural auditory front-end,�� 
///        IEEE Transactions on Audio, Speech, and Language Processing 19(1), 
///        pp. 1?13.
 
namespace openAFE {

	class ITD : public TFSProcessor<double> {

		private:

            double frameFsHz;   // Sampling frequency of the signal before framing, used for expressing ITDs in seconds

			void prepareForProcessing();

			void processChannel( double* firstValue_l, double* firstValue_r, double* result );
			
			std::shared_ptr<CrossCorrelation > upperProcPtr;

			/* Will be used to append to PMZ */
			std::vector<double> zerosVector;
			std::shared_ptr<twoCTypeBlock<double> > zerosAccecor;
						
		public:
		
			ITD (const std::string nameArg, std::shared_ptr<CrossCorrelation > upperProcPtr );

			~ITD ();
			
			void processChunk ();

			std::string get_upperProcName();
			
	}; /* class ITD */
}; /* namespace openAFE */

#endif /* ITDPROC_HPP */
