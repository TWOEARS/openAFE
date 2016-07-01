#include "ratemap.hpp"

using namespace openAFE;

			void Ratemap::populateFilters( filterPtrVector& filterVec, std::size_t numberOfChannels, double fs ) {
							
				filterVec.resize(numberOfChannels);

				for ( std::size_t ii = 0 ; ii < numberOfChannels ; ++ii )
					filterVec[ii].reset( new leakyIntegratorFilter( fs, this->decaySec ) );
			}
			
			void Ratemap::prepareForProcessing() {
				
				this->populateFilters( rmFilter_l, this->get_nChannels(), this->getFsIn() );
				this->populateFilters( rmFilter_r, this->get_nChannels(), this->getFsIn() );				
			}

			void Ratemap::processChannel( double* firstValue_l, double* firstValue_r, double *resultLeft, double *resultRight ) {

				switch( this->scailing) {
					
					case _magnitude:
						multiplication( this->win.data(), firstValue_l, this->wSize, firstValue_l );
						multiplication( this->win.data(), firstValue_r, this->wSize, firstValue_r );
								
						*(resultLeft) = mean( firstValue_l, this->wSize );
						*(resultRight) = mean( firstValue_r, this->wSize );
						
						break;
						
					case _power:
						multiplication( this->win.data(), firstValue_l, this->wSize, firstValue_l );
						multiplication( this->win.data(), firstValue_r, this->wSize, firstValue_r );
							
						*resultLeft = meanSquare( firstValue_l, this->wSize );
						*resultRight = meanSquare( firstValue_r, this->wSize );
						
						break;
						
					default:
						break;
				}
				
				std::cout << "resultLeft : " << *resultLeft << " resultRight : " << *resultRight << std::endl;
				sleep(3);
			}
			
			Ratemap::Ratemap (const std::string nameArg, std::shared_ptr<IHCProc > upperProcPtr, double wSizeSec, double hSizeSec, scalingType scailingArg, double decaySec, windowType wname  )
			: WindowBasedProcs (nameArg, upperProcPtr, _ratemap, wSizeSec, hSizeSec, wname, scailingArg ) {
				
				this->decaySec = decaySec;
				this->scailing = scailingArg;
				
				this->prepareForProcessing();
			}

			Ratemap::~Ratemap () {
				
			}
			
			void Ratemap::processChunk () {	

				this->setNFR ( this->upperProcPtr->getNFR() );
											
				// Append provided input to the buffer
				this->buffer_l->appendChunk( this->upperProcPtr->getLeftLastChunkAccessor() );
				this->buffer_r->appendChunk( this->upperProcPtr->getRightLastChunkAccessor() );

				std::vector<std::shared_ptr<twoCTypeBlock<double> > > l_innerBuffer = buffer_l->getLastChunkAccesor();
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > r_innerBuffer = buffer_r->getLastChunkAccesor();
				
				std::size_t dim1_l, dim2_l, dim1_r, dim2_r;
				double *firstValue1_l, *firstValue2_l, *firstValue1_r, *firstValue2_r;
				
				// FILTERING
				for ( std::size_t ii = 0 ; ii < this->get_nChannels() ; ++ii ) {

					// LEFT						
					dim1_l = l_innerBuffer[ii]->array1.second;
					dim2_l = l_innerBuffer[ii]->array2.second;
							
					firstValue1_l = l_innerBuffer[ii]->array1.first;
					firstValue2_l = l_innerBuffer[ii]->array2.first;
					
					if ( dim1_l > 0 )
						rmFilter_l[ii]->exec ( firstValue1_l, dim1_l, firstValue1_l );
					if ( dim2_l > 0 )
						rmFilter_l[ii]->exec ( firstValue2_l, dim2_l, firstValue2_l );
					
					// RIGHT	
					dim1_r = r_innerBuffer[ii]->array1.second;
					dim2_r = r_innerBuffer[ii]->array2.second;
							
					firstValue1_r = r_innerBuffer[ii]->array1.first;
					firstValue2_r = r_innerBuffer[ii]->array2.first;
					
					if ( dim1_r > 0 )
						rmFilter_r[ii]->exec ( firstValue1_r, dim1_r, firstValue1_r );
					if ( dim2_r > 0 )
						rmFilter_r[ii]->exec ( firstValue2_r, dim2_r, firstValue2_r );											
					
				}
							
				// The buffer should be linearized for windowing.
				this->buffer_l->linearizeBuffer();
				this->buffer_r->linearizeBuffer();
								
				l_innerBuffer = buffer_l->getWholeBufferAccesor();
				r_innerBuffer = buffer_r->getWholeBufferAccesor();
				
				// Quick control of dimensionality
				assert( l_innerBuffer.size() == r_innerBuffer.size() );

				std::size_t totalFrames = floor( ( this->buffer_l->getSize() - ( this->wSize - this->hSize ) ) / this->hSize );
								
				// Creating a chunk of zeros.
				this->zerosVector.resize( totalFrames, 0 );
				
				// Creating a accesor to it (The data on zerosVector is continious)
				this->zerosAccecor->array1.first = zerosVector.data(); this->zerosAccecor->array2.first = nullptr;
				this->zerosAccecor->array1.second = zerosVector.size(); this->zerosAccecor->array2.second = 0;

				// Appending this chunk to all channels of the PMZ.
				leftPMZ->appendChunk( zerosAccecor );
				rightPMZ->appendChunk( zerosAccecor );
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > lastChunkOfPMZ = leftPMZ->getLastChunkAccesor();
				std::vector<std::shared_ptr<twoCTypeBlock<double> > > rightChunkOfPMZ = rightPMZ->getLastChunkAccesor();

				std::size_t ii;
				uint32_t n_start;
				for ( ii = 1 /* TODO : 0 */ ; ii < totalFrames ; ++ii ) {

					n_start = ii * this->hSize;

					// Loop on the channels : Got better run-time results when not creating threads.
					for ( std::size_t jj = 0 ; jj < this->fb_nChannels ; ++jj )
						processChannel( l_innerBuffer[jj]->array1.first + n_start, r_innerBuffer[jj]->array1.first + n_start, lastChunkOfPMZ[jj]->getPtr(ii), rightChunkOfPMZ[jj]->getPtr(ii) );
				}

				this->buffer_l->pop_chunk( ii * this->hSize );
				this->buffer_r->pop_chunk( ii * this->hSize );
			}
			
			// getters
			const double Ratemap::get_rm_decaySec() {return this->decaySec;}
			const scalingType Ratemap::get_rm_scailing() {return this->scailing;}
  
			// setters			
			void Ratemap::set_rm_decaySec(const double arg) {this->decaySec=arg; this->prepareForProcessing ();}
			void Ratemap::set_rm_scailing(const scalingType arg) {this->scailing=arg; this->prepareForProcessing ();}
