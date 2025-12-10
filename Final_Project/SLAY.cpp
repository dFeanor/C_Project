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

	std::vector<double> SLAYSolver::read_vector_from_file(const std::string& filename) {
        std::ifstream in(filename);
        if (!in.is_open()) {
            throw std::runtime_error("Cannot open vector file: " + filename);
        }

        std::vector<double> v;
        double value;

        while (in >> value) {
            v.push_back(value);
        }

        if (v.empty()) {
            throw std::runtime_error("Vector file is empty or invalid: " + filename);
        }

        return v;
    }

    void SLAYSolver::write_vector_to_file(const std::string& filename,
                                          const std::vector<double>& v) {
        std::ofstream out(filename);
        if (!out.is_open()) {
            throw std::runtime_error("Cannot open output file: " + filename);
        }

        for (double x : v) {
            out << setprecision(16) << x << "\n";
        }
    }

    struct SparseElement {
        int col;
        double val;
    };

    std::vector<double> SLAYSolver::solve_cholesky(const CSR3& A, const std::vector<double>& b) {
        int N = A.N;
        if (N != A.M) throw std::invalid_argument("Error: CSR Matrix must be square.");
        if (b.size() != N) throw std::invalid_argument("Error: Vector b size mismatch.");

        std::vector<std::vector<SparseElement>> L_rows(N);
        std::vector<double> L_diag(N);
        std::vector<double> dense_row(N, 0.0);

        for (int i = 0; i < N; ++i) {
            int start_idx = A.rowIndex[i];
            int end_idx = A.rowIndex[i + 1];

            for (int k = start_idx; k < end_idx; ++k) {
                int col = A.columns[k];
                if (col <= i) {
                    dense_row[col] = A.values[k];
                }
            }

            for (int j = 0; j < i; ++j) {
                if (L_rows[j].empty() && std::abs(dense_row[j]) < 1e-15) continue;

                double dot = 0.0;
                for (const auto& item : L_rows[j]) {
                    dot += item.val * dense_row[item.col];
                }

                double L_ij = (dense_row[j] - dot) / L_diag[j];

                if (std::abs(L_ij) > 1e-15) {
                    dense_row[j] = L_ij;
                    L_rows[i].push_back({ j, L_ij });
                }
                else {
                    dense_row[j] = 0.0;
                }
            }

            double dot_diag = 0.0;
            for (const auto& item : L_rows[i]) {
                dot_diag += item.val * item.val;
            }

            double val_sq = dense_row[i] - dot_diag;
            if (val_sq <= 0) {
                throw std::runtime_error("Cholesky Error: Matrix not positive definite at row " + std::to_string(i));
            }
            L_diag[i] = std::sqrt(val_sq);

            dense_row[i] = 0.0;
            for (const auto& item : L_rows[i]) dense_row[item.col] = 0.0;

            for (int k = start_idx; k < end_idx; ++k) {
                int col = A.columns[k];
                if (col <= i) dense_row[col] = 0.0;
            }
        }

        std::vector<double> y(N, 0.0);
        for (int i = 0; i < N; ++i) {
            double sum = 0.0;
            for (const auto& item : L_rows[i]) {
                sum += item.val * y[item.col];
            }
            y[i] = (b[i] - sum) / L_diag[i];
        }

        std::vector<std::vector<SparseElement>> LT_rows(N);
        for (int i = 0; i < N; ++i) {
            for (const auto& item : L_rows[i]) {
                LT_rows[item.col].push_back({ i, item.val });
            }
        }

        std::vector<double> x = y;
        for (int i = N - 1; i >= 0; --i) {
            double sum = 0.0;
            for (const auto& item : LT_rows[i]) {
                sum += item.val * x[item.col];
            }
            x[i] = (y[i] - sum) / L_diag[i];
        }

        return x;
    }

}