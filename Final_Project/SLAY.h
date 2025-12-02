#pragma once
#ifndef SLAY_H
#define SLAY_H

#include <string>
#include <vector>
#include <map>
#include "Matrix.h"
#include "CSR3.h" 

namespace Matrixes {

	class SLAYSolver {
	public:
		static std::vector<double> solve_cholesky(const Matrix& A, const std::vector<double>& b);
		static std::vector<double> solve_cholesky(const CSR3& A, const std::vector<double>& b);
	private:

		SLAYSolver() {}
	};

} 
#endif
