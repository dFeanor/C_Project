#include <stdexcept>
#include <fstream>

#include "Matrix.h"

namespace Matrixes {

	Matrix::Matrix(int row_num, int col_num, double value) : 
		m_row_num(row_num), m_col_num(col_num), m_data(row_num * col_num, value) {}

	//Matrix::Matrix(int row_num, string type) : Matrix(row_num, row_num) {
	//	if (type == "eye") {
	//		for (int i = 0; i < row_num; ++i) {
	//			m_data[i * row_num + i] = 1.0;
	//		}
	//	}
	//	else {
	//		throw invalid_argument("Других типов матриц нет");
	//	}
	//}

	int Matrix::get_row_num() const {
		return m_row_num;
	}

	int Matrix::get_col_num() const {
		return m_col_num;
	}

	void Matrix::check_index(int row_ind, int col_ind, string message) const {
		if (row_ind < 0 || row_ind >= m_row_num || col_ind < 0 || col_ind >= m_col_num) {
			throw out_of_range(message);
		}
	}

	double Matrix::get_elem(int row_ind, int col_ind) const {
		check_index(row_ind, col_ind, "get_elem");
		return m_data[row_ind * m_col_num + col_ind];
	}

	void Matrix::set_elem(int row_ind, int col_ind, double value) {
		check_index(row_ind, col_ind, "set_elem");
		m_data[row_ind * m_col_num + col_ind] = value;
	}

	Matrix Matrix::sub_matrix(int row_start, int col_start, int row_end, int col_end) const {
		check_index(row_start, col_start, "sub_matrix start");
		check_index(row_end - 1, col_end - 1, "sub_matrix end");
		Matrix result(row_end - row_start, col_end - col_start);
		for (int i = 0; i < row_end - row_start; ++i) {
			for (int j = 0; j < col_end - col_start; ++j) {
				result.m_data[i * (col_end - col_start) + j] = m_data[(i + row_start) * m_col_num + j + col_start];
			}
			//memcpy(result.m_data.data() + i * (col_end - col_start),
			//	m_data.data() + (row_start + i) * m_col_num + col_start,
			//	sizeof(double) * (col_end - col_start));
		}
		return result;
	}

	Matrix Matrix::eye(int row_num) {
		Matrix result(row_num, row_num);
		for (int i = 0; i < row_num; ++i) {
			result.m_data[i * row_num + i] = 1.0;
		}
		return result;
	}

	void Matrix::write(string file_name) const {
		ofstream fout(file_name);
		fout << m_row_num << " " << m_col_num << endl;
		for (int i = 0; i < m_row_num; ++i) {
			for (int j = 0; j < m_col_num; ++j) {
				fout << m_data[i * m_col_num + j] << " ";
			}
			fout << endl;
		}
	}

	Matrix Matrix::from_file(string file_name) {
		ifstream fin(file_name);
		int row_num = 0;
		int col_num = 0;
		fin >> row_num >> col_num;
		Matrix matrix(row_num, col_num);
		for (int i = 0; i < row_num; ++i) {
			for (int j = 0; j < col_num; ++j) {
				fin >> matrix.m_data[i * col_num + j];
			}
		}
		return matrix;
	}

	double& Matrix::operator[](tuple<int, int> ind) {
		return m_data[get<0>(ind) * m_col_num + get<1>(ind)];

	}

	double Matrix::operator[](tuple<int, int> ind) const {
		return m_data[get<0>(ind) * m_col_num + get<1>(ind)];
	}


	Matrix Matrix::operator+(const Matrix& m) const {
		Matrix result(m_row_num, m_col_num);
		for (int i = 0; i < m_row_num; ++i) {
			for (int j = 0; j < m_col_num; ++j) {
				int ind = i * m_col_num + j;
				result.m_data[ind] = m_data[ind] + m.m_data[ind];
			}
		}
		return result;
	}

	Matrix Matrix::operator*(double scal) const {
		Matrix result(m_row_num, m_col_num);
		for (int i = 0; i < m_row_num; ++i) {
			for (int j = 0; j < m_col_num; ++j) {
				int ind = i * m_col_num + j;
				result.m_data[ind] = m_data[ind] * scal;
			}
		}
		return result;

	}

	Matrix Matrix::operator-() const {
		return *this * (-1.0);
	}

	Matrix Matrix::operator-(const Matrix& m) const {
		return *this + (-m);
	}

	Matrix Matrix::operator*(const Matrix& m) const {
		if (m_col_num != m.m_row_num) {
			throw std::invalid_argument("Matrices dimensions are incompatible for multiplication!");
		}
		Matrix result(m_row_num, m.m_col_num);
		for (int i = 0; i < m_row_num; ++i) {
			for (int j = 0; j < m.m_col_num; ++j) {
				result[{i, j}] = 0;
				for (size_t k = 0; k < m_col_num; ++k) {
					result[{i, j}] += (*this)[{i, k}] * m[{k, j}];
				}
			}
		}
		return result;

	}

}