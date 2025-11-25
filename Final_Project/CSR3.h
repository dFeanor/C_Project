#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include "Matrix.h"
using namespace std;

namespace Matrixes {
	class CSR3 {
		vector<double> values;
		vector<int> columns;
		vector<int> rowIndex;
	public:
		int M;
		int N;
		int L;
		void setElement(int i, int j, double element);
		double getElement(int i, int j);
		CSR3 Submatrix(int from_i, int to_i, int from_j, int to_j);
		void Transpose();
		CSR3 new_Transpose();
		Matrix Convert();
		static CSR3 Unary(int n);
		static CSR3 Read(string filename);
		void Write(string filename);

		friend CSR3 operator+(const CSR3& A, const CSR3& B);
		CSR3 operator*(double scalar) const;
		CSR3 operator-(CSR3& B) const;
		CSR3 operator-() const;
		vector<double> operator()(vector <double> vec) const;
		double operator[](pair<int, int> indices) const;
	};
}
