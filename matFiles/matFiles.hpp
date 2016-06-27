#ifndef MATFILES_HPP
#define MATFILES_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>

#include "mat.h"

namespace matFiles {
	
	int readMatFile(const char *file, std::vector <std::vector<double> >& earSignals, double *fsHz);
	
	int writeMatFile();
	
}; /* namespace matFiles */

#endif /* MATFILES_HPP */
