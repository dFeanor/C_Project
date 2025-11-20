#pragma once
#ifndef SLAY_H
#define SLAY_H

#include <string>
#include <vector>
#include "Matrix.h"

namespace Matrixes {

	/**
	 * @class SLAYSolver
	 * @brief Класс, содержащий статические методы для решения Систем Линейных Алгебраических Уравнений (СЛАУ).
	 *
	 * Предоставляет реализацию различных алгоритмов для решения систем вида Ax = b,
	 * где A - матрица коэффициентов, x - вектор неизвестных, b - вектор правой части.
	 */
	class SLAYSolver {
	public:
		/**
		 * @brief Решает СЛАУ Ax=b.
		 *
		 * (Здесь будет реализация конкретного метода, который вы выберете)
		 *
		 * @param A Квадратная матрица коэффициентов размера n x n.
		 * @param b Вектор-столбец правой части размера n x 1.
		 * @return Вектор-столбец решений x размера n x 1.
		 * @throws std::invalid_argument если матрица A не квадратная или размеры A и b несовместимы.
		 */

		static Matrix solve_cholesky(const Matrix& A, const Matrix& b);

	private:
		// Конструктор сделан приватным, так как класс содержит только статические методы
		// и не предназначен для создания экземпляров.
		SLAYSolver() {}
	};

} // namespace lecture4

#endif