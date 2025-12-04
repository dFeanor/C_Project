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
        static std::vector<double> read_vector_from_file(const std::string& filename);
        static void write_vector_to_file(const std::string& filename, const std::vector<double>& v);
	private:

		//SLAYSolver() {}
	};

} 
#endif
