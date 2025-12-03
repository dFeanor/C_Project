#pragma once
#include <vector>
#include <string>

using namespace std;

namespace Matrixes {

	class Matrix {
		int m_row_num;
		int m_col_num;
		vector<double> m_data;
		void check_index(int row_ind, int col_ind, string message = "") const;
	public:
		Matrix(int row_num, int col_num, double value = 0.0);
		double get_elem(int row_ind, int col_ind) const;
		void set_elem(int row_ind, int col_ind, double value);
		Matrix sub_matrix(int row_start, int col_start, int row_end, int col_end) const;
		static Matrix eye(int row_num);
		void write(string file_name) const;
		static Matrix from_file(string file_name);

		int get_row_num() const;
		int get_col_num() const;

		Matrix operator+(const Matrix& m) const;
		Matrix operator*(double scal) const;
		Matrix operator-() const;
		Matrix operator-(const Matrix& m) const;
		Matrix operator*(const Matrix& m) const;
		double operator[](tuple<int, int> ind) const;
		double& operator[](tuple<int, int> ind);

	};

}