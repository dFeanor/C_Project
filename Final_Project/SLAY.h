#pragma once
#ifndef SLAY_H
#define SLAY_H

#include <string>
#include <vector>
#include "Matrix.h"

namespace Matrixes {

	class SLAYSolver {
	public:
		static Matrix solve_cholesky(const Matrix& A, const Matrix& b);

	private:

		SLAYSolver() {}
	};

} 
#endif
