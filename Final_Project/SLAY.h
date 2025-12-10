#pragma once
#ifndef SLAY_H
#define SLAY_H

#include <string>
#include <vector>
#include <map>
#include "Matrix.h"
#include "CSR3.h" 
#include <iomanip>

namespace Matrixes {
	struct CholeskyDecomposition {
		int N;
		// L хранится по строкам как список пар {col, val}
		std::vector<std::vector<std::pair<int, double>>> L_rows;
		std::vector<double> L_diag;
	};

	class SLAYSolver {
	public:
		static std::vector<double> solve_cholesky(const Matrix& A, const std::vector<double>& b);
		static std::vector<double> solve_cholesky(const CSR3& A, const std::vector<double>& b);
        static std::vector<double> read_vector_from_file(const std::string& filename);
        static void write_vector_to_file(const std::string& filename, const std::vector<double>& v);

		// НОВЫЕ МЕТОДЫ
		static CholeskyDecomposition factorize(const CSR3& A);
		static std::vector<double> solve_from_factors(const CholeskyDecomposition& L, const std::vector<double>& b);
	private:

		//SLAYSolver() {}
	};

} 
#endif
