#include "SLAY.h"
#include <stdexcept> 

namespace Matrixes {
	Matrix SLAYSolver::solve_cholesky(const Matrix& A, const Matrix& b) {
		int n = A.get_row_num();
		if (n != A.get_col_num()) {
			throw std::invalid_argument("Matrix A must be square for Cholesky decomposition.");
		}
		if (n != b.get_row_num() || b.get_col_num() != 1) {
			throw std::invalid_argument("Dimension mismatch between matrix A and vector b.");
		}
		Matrix L(n, n, 0.0);
		for (int j = 0; j < n; ++j) {
			double sum_diag = 0.0;
			for (int k = 0; k < j; ++k) {
				sum_diag += L[{j, k}] * L[{j, k}];
			}
			double value_under_sqrt = A[{j, j}] - sum_diag;
			if (value_under_sqrt <= 0) {
				throw std::runtime_error("Matrix is not positive-definite.");
			}
			L[{j, j}] = sqrt(value_under_sqrt);
			for (int i = j + 1; i < n; ++i) {
				double sum_sub_diag = 0.0;
				for (int k = 0; k < j; ++k) {
					sum_sub_diag += L[{i, k}] * L[{j, k}]; 
				}
				L[{i, j}] = (A[{i, j}] - sum_sub_diag) / L[{j, j}];
			}
		}
		Matrix y(n, 1);
		for (int i = 0; i < n; ++i) {
			double sum = 0.0;
			for (int j = 0; j < i; ++j) {
				sum += L[{i, j}] * y[{j, 0}];
			}
			y[{i, 0}] = (b[{i, 0}] - sum) / L[{i, i}];
		}
		Matrix x(n, 1);
		for (int i = n - 1; i >= 0; --i) {
			double sum = 0.0;
			for (int j = i + 1; j < n; ++j) {
				sum += L[{j, i}] * x[{j, 0}]; 
			}
			x[{i, 0}] = (y[{i, 0}] - sum) / L[{i, i}];
		}

		return x;
	}
}
