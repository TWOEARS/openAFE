#include "matFiles.h"

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

	int matFiles::writeMatFile() {
		
	}
