#include "SLAY.h"
#include <stdexcept> // Для выбрасывания исключений std::invalid_argument и std::logic_error

namespace Matrixes {
	Matrix SLAYSolver::solve_cholesky(const Matrix& A, const Matrix& b) {
		// --- Шаг 0: Проверка корректности входных данных ---
		int n = A.get_row_num();
		if (n != A.get_col_num()) {
			throw std::invalid_argument("Matrix A must be square for Cholesky decomposition.");
		}
		if (n != b.get_row_num() || b.get_col_num() != 1) {
			throw std::invalid_argument("Dimension mismatch between matrix A and vector b.");
		}

		//// Проверка на симметричность
		//const double EPS = 1e-12;
		//for (int i = 0; i < n; ++i) {
		//	for (int j = i + 1; j < n; ++j) {
		//		if (std::abs(A[{i, j}] - A[{j, i}]) > EPS) {
		//			throw std::invalid_argument("Matrix A must be symmetric for Cholesky decomposition.");
		//		}
		//	}
		//}

		// --- Шаг 1: Разложение Холецкого (A = L * L^T) ---
		// Реализация псевдокода с картинки. L - это C в псевдокоде.
		Matrix L(n, n, 0.0);
		for (int j = 0; j < n; ++j) {
			// Вычисление диагонального элемента L[j, j]
			double sum_diag = 0.0;
			for (int k = 0; k < j; ++k) {
				sum_diag += L[{j, k}] * L[{j, k}]; // c_jk^2
			}

			double value_under_sqrt = A[{j, j}] - sum_diag;
			if (value_under_sqrt <= 0) {
				throw std::runtime_error("Matrix is not positive-definite.");
			}
			L[{j, j}] = sqrt(value_under_sqrt);

			// Вычисление поддиагональных элементов в столбце j
			for (int i = j + 1; i < n; ++i) {
				double sum_sub_diag = 0.0;
				for (int k = 0; k < j; ++k) {
					sum_sub_diag += L[{i, k}] * L[{j, k}]; // c_ik * c_jk
				}
				L[{i, j}] = (A[{i, j}] - sum_sub_diag) / L[{j, j}];
			}
		}

		// --- Шаг 2: Прямая подстановка (решение Ly = b) ---
		Matrix y(n, 1);
		for (int i = 0; i < n; ++i) {
			double sum = 0.0;
			for (int j = 0; j < i; ++j) {
				sum += L[{i, j}] * y[{j, 0}];
			}
			y[{i, 0}] = (b[{i, 0}] - sum) / L[{i, i}];
		}

		// --- Шаг 3: Обратная подстановка (решение L^T * x = y) ---
		Matrix x(n, 1);
		for (int i = n - 1; i >= 0; --i) {
			double sum = 0.0;
			for (int j = i + 1; j < n; ++j) {
				sum += L[{j, i}] * x[{j, 0}]; // Используем L[j, i], так как матрица L^T
			}
			x[{i, 0}] = (y[{i, 0}] - sum) / L[{i, i}];
		}

		return x;
	}
}