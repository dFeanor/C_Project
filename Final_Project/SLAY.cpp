#include "SLAY.h"
#include <stdexcept>
#include <cmath>
#include <map>
#include <vector>
#include <string>
#include <iostream>

namespace Matrixes {

	static double get_vec_norm(const std::vector<double>& v) {
		double sum = 0.0;
		for (double val : v) sum += val * val;
		return std::sqrt(sum);
	}

	std::vector<double> SLAYSolver::solve_cholesky(const Matrix& A, const std::vector<double>& b) {
		int n = A.get_row_num();

		if (n != A.get_col_num()) {
			throw std::invalid_argument("Matrix A must be square for Cholesky decomposition.");
		}
		if (n != b.size()) {
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
			L[{j, j}] = std::sqrt(value_under_sqrt);

			for (int i = j + 1; i < n; ++i) {
				double sum_sub_diag = 0.0;
				for (int k = 0; k < j; ++k) {
					sum_sub_diag += L[{i, k}] * L[{j, k}];
				}
				L[{i, j}] = (A[{i, j}] - sum_sub_diag) / L[{j, j}];
			}
		}

		std::vector<double> y(n);
		for (int i = 0; i < n; ++i) {
			double sum = 0.0;
			for (int j = 0; j < i; ++j) {
				sum += L[{i, j}] * y[j];
			}
			y[i] = (b[i] - sum) / L[{i, i}];
		}

		std::vector<double> x(n);
		for (int i = n - 1; i >= 0; --i) {
			double sum = 0.0;
			for (int j = i + 1; j < n; ++j) {
				sum += L[{j, i}] * x[j];
			}
			x[i] = (y[i] - sum) / L[{i, i}];
		}

		double norm_b = get_vec_norm(b);
		if (norm_b < 1e-16) norm_b = 1.0;

		std::vector<double> residual(n);
		for (int i = 0; i < n; ++i) {
			double Ax_i = 0.0;
			for (int j = 0; j < n; ++j) {
				Ax_i += A[{i, j}] * x[j];
			}
			residual[i] = b[i] - Ax_i;
		}

		double norm_res = get_vec_norm(residual);
		double rel_error = norm_res / norm_b;

		if (rel_error > 1e-8) {
			throw std::runtime_error("Solution accuracy check failed. Relative error: " + std::to_string(rel_error));
		}

		return x;
	}

	std::vector<double> SLAYSolver::solve_cholesky(const CSR3& A, const std::vector<double>& b) {
		int N = A.N;

		if (N != A.M) {
			throw std::invalid_argument("Error: CSR Matrix must be square.");
		}
		if (b.size() != N) {
			throw std::invalid_argument("Error: Vector b size does not match matrix size.");
		}

		std::vector<std::map<int, double>> L_rows(N);

		for (int i = 0; i < N; i++) {
			std::map<int, double>& current_L_row = L_rows[i];
			int start_idx = A.rowIndex[i];
			int end_idx = A.rowIndex[i + 1];

			for (int k = start_idx; k < end_idx; k++) {
				int col = A.columns[k];
				if (col <= i) {
					current_L_row[col] = A.values[k];
				}
			}

			int min_col = (current_L_row.empty()) ? i : current_L_row.begin()->first;

			for (int j = min_col; j < i; j++) {
				double sum = 0.0;
				for (auto const& [k, val_jk] : L_rows[j]) {
					if (k >= j) break;
					if (current_L_row.count(k)) {
						sum += current_L_row[k] * val_jk;
					}
				}

				double A_ij = current_L_row.count(j) ? current_L_row[j] : 0.0;
				double val = (A_ij - sum) / L_rows[j][j];

				if (std::abs(val) > 1e-15) {
					current_L_row[j] = val;
				}
				else {
					current_L_row.erase(j);
				}
			}

			double sum_diag = 0.0;
			for (auto const& [k, val_ik] : current_L_row) {
				if (k < i) {
					sum_diag += val_ik * val_ik;
				}
			}

			double A_ii = current_L_row.count(i) ? current_L_row[i] : 0.0;
			double val_sq = A_ii - sum_diag;

			if (val_sq <= 0) {
				throw std::runtime_error("Cholesky Error: Matrix is not positive definite at row " + std::to_string(i));
			}

			current_L_row[i] = std::sqrt(val_sq);
		}

		std::vector<double> y(N, 0.0);
		for (int i = 0; i < N; i++) {
			double sum = 0.0;
			for (auto const& [k, val_ik] : L_rows[i]) {
				if (k < i) {
					sum += val_ik * y[k];
				}
			}
			y[i] = (b[i] - sum) / L_rows[i][i];
		}

		std::vector<std::map<int, double>> LT_rows(N);
		for (int i = 0; i < N; i++) {
			for (auto const& [j, val] : L_rows[i]) {
				LT_rows[j][i] = val;
			}
		}

		std::vector<double> x = y;
		for (int i = N - 1; i >= 0; i--) {
			double sum = 0.0;
			for (auto const& [k, val_ik] : LT_rows[i]) {
				if (k > i) {
					sum += val_ik * x[k];
				}
			}
			x[i] = (y[i] - sum) / LT_rows[i][i];
		}

		double norm_b = get_vec_norm(b);
		if (norm_b < 1e-16) norm_b = 1.0;

		std::vector<double> residual(N);
		for (int i = 0; i < N; ++i) {
			double Ax_i = 0.0;
			for (int k = A.rowIndex[i]; k < A.rowIndex[i + 1]; ++k) {
				int col = A.columns[k];
				double val = A.values[k];
				Ax_i += val * x[col];
			}
			residual[i] = b[i] - Ax_i;
		}

		double norm_res = get_vec_norm(residual);
		double rel_error = norm_res / norm_b;

		if (rel_error > 1e-8) {
			throw std::runtime_error("CSR Solution accuracy check failed. Relative error: " + std::to_string(rel_error));
		}

		return x;
	}

}