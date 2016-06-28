#include "matFiles.hpp"

	int matFiles::readMatFile(const char *file, std::vector <std::vector<double> >& earSignals, double *fsHz) {
	  MATFile *pmat;
	  const char **dir;
	  const char *name;
	  int	  ndir;
	  int	  i;
	  mxArray *pa;
	  mxArray *var;
	  const size_t *dims;
	  size_t ndims;
	  double *data;
		
	  /*
	   * Open file to get directory
	   */
	  pmat = matOpen(file, "r");
	  if (pmat == NULL) {
		printf("Error opening file %s\n", file);
		return(1);
	  }
	  
	  /*
	   * get directory of MAT-file
	   */
	  dir = (const char **)matGetDir(pmat, &ndir);
	  if (dir == NULL) {
		printf("Error reading directory of file %s\n", file);
		return(1);
	  }
	  mxFree(dir);

	  /* In order to use matGetNextXXX correctly, reopen file to read in headers. */
	  if (matClose(pmat) != 0) {
		printf("Error closing file %s\n",file);
		return(1);
	  }
	  pmat = matOpen(file, "r");
	  if (pmat == NULL) {
		printf("Error reopening file %s\n", file);
		return(1);
	  }

	  /* Get headers of all variables */
	  /* Examining the header for each variable */
	  for (i=0; i < ndir; i++) {
		pa = matGetNextVariableInfo(pmat, &name);
		var = matGetVariable(pmat, name);
		data = mxGetPr(var);
		
		if (pa == NULL) {
		printf("Error reading in file %s\n", file);
		return(1);
		}

		if ( strcmp(name,"earSignals") == 0 )  {
			ndims = mxGetNumberOfDimensions(pa);
			dims = mxGetDimensions(pa);
			
			earSignals.resize(ndims);
			
			for ( size_t ii = 0 ; ii < ndims ; ++ii )
				earSignals[ii].resize(dims[0],0);
				
			size_t ii, iii;
			for ( ii = 0 ; ii < dims[0] ; ++ii )
				earSignals[0][ii] =  data[ii];

			for ( ii = dims[0], iii = 0 ; ii < dims[0] * 2 ; ++ii, ++iii )
				earSignals[1][iii] =  data[ii];
				
		} else	if ( strcmp(name,"fsHz") == 0 ) {
			ndims = mxGetNumberOfDimensions(pa);
			dims = mxGetDimensions(pa);
			
			assert( ndims == 2 );
			assert( dims[0] == 1 );
							
			*fsHz = data[0];
		}
			
		mxDestroyArray(pa);
	  }

	  if (matClose(pmat) != 0) {
		  printf("Error closing file %s\n",file);
		  return(1);
	  }
	  return(0);
	}

	int matFiles::writeTDSMatFile(const char *file, std::shared_ptr<openAFE::twoCTypeBlock<double> > left, std::shared_ptr<openAFE::twoCTypeBlock<double> > right, double fsHz) {
		
		MATFile *pmat;

		/* Variables for mxArrays  */
		mxArray *pn_l, *pn_r, *pn2;

		pmat = matOpen(file, "w");
		if (pmat == NULL) {
		  printf("Error creating file");
		return(EXIT_FAILURE);
		}
		int status;
		/* EAR SIGNAL : LEFT */
		uint32_t leftSize = left->array1.second + left->array2.second;
		pn_l = mxCreateDoubleMatrix(1,leftSize,mxREAL);
		   if (pn_l == NULL) {
				printf("Unable to create mxArray with mxCreateDoubleMatrix\n");
				return(1);
		}

		memcpy( mxGetPr(pn_l), left->array1.first, left->array1.second * sizeof(double) );
		memcpy( mxGetPr(pn_l) + left->array1.second, left->array2.first, left->array2.second * sizeof(double) );
						
		status = matPutVariable(pmat, "left", pn_l);
		if ((status) != 0) {
			printf("Error writing.\n");
			return(EXIT_FAILURE);
		}

		/* EAR SIGNAL : RIGHT */
		uint32_t rightSize = right->array1.second + right->array2.second;
		pn_r = mxCreateDoubleMatrix(1,rightSize,mxREAL);
		   if (pn_r == NULL) {
				printf("Unable to create mxArray with mxCreateDoubleMatrix\n");
				return(1);
		}

		memcpy( mxGetPr(pn_r), right->array1.first, right->array1.second * sizeof(double) );
		memcpy( mxGetPr(pn_r) + right->array1.second, right->array2.first, right->array2.second * sizeof(double) );
						
		 status = matPutVariable(pmat, "right", pn_r);
		 if ((status) != 0) {
			  printf("Error writing.\n");
			  return(EXIT_FAILURE);
		  }
				
		/* fsHZ */
		pn2 = mxCreateDoubleScalar(fsHz);
		   if (pn2 == NULL) {
				printf("Unable to create mxArray with mxCreateDoubleMatrix\n");
				return(1);
		}
		  
		status = matPutVariable(pmat, "fsHz", pn2);
		 if ((status) != 0) {
			  printf("Error writing.\n");
			  return(EXIT_FAILURE);
		  }
		  
		  return(0);	  
	}
