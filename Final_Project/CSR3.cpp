#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include "Matrix.h"
#include "CSR3.h"
#include <iomanip>
using namespace std;

namespace Matrixes {
	void CSR3::setElement(int i, int j, double element) {
		int low = rowIndex[j];
		int up = rowIndex[j + 1];
		if (low == up)
			throw out_of_range("Line is empty");
		else {
			for (int k = 0; k < up - low; k++) {
				if (columns[low + k] == i)
					values[low + k] = element;
				else throw out_of_range("This element is equal to zero");
			}
		}

	}
	double CSR3::getElement(int i, int j) {
		int low = rowIndex[j];
		int up = rowIndex[j + 1];
		if (low == up)
			throw out_of_range("Line is empty");
		else {
			for (int k = 0; k < up - low; k++) {
				if (columns[low + k] == i)
					return(values[low + k]);
				else throw out_of_range("This element is equal to zero");
			}
		}
	}
	CSR3 CSR3::Submatrix(int from_i, int to_i, int from_j, int to_j) {
		CSR3 sub;
		sub.M = to_j - from_j;
		sub.N = to_i - from_i;
		sub.rowIndex.resize(sub.M + 1);

		int count = 0;
		sub.rowIndex[0] = 0;

		for (int j = from_j; j < to_j; j++) {
			int low = rowIndex[j];
			int up = rowIndex[j + 1];

			for (int k = low; k < up; k++) {
				int col = columns[k];
				if (col >= from_i && col < to_i) {
					sub.values.push_back(values[k]);
					sub.columns.push_back(col - from_i);
					count++;
				}
			}
			sub.rowIndex[j - from_j + 1] = count;
		}

		sub.L = count;
		return sub;
	}

	CSR3 CSR3::Unary(int n) {
		CSR3 res;
		res.M = n;
		res.N = n;
		res.L = n;
		res.values.resize(res.L);
		res.columns.resize(res.L);
		res.rowIndex.resize(res.M + 1);
		for (int i = 0; i < n; i++) {
			res.values[i] = 1.0;
			res.columns[i] = i;
			res.rowIndex[i] = i;
		}
		res.rowIndex[res.M] = n;
		res.rowIndex.push_back(n);
		return res;
	}

	CSR3 CSR3::Read(string filename) {
		int i, j;
		double v;
		CSR3 result;
		ifstream fin(filename);
		fin >> result.M >> result.N >> result.L;
		result.values.resize(result.L);
		result.columns.resize(result.L);
		vector <int> row(result.M + 1, -1);
		result.rowIndex = row;
		result.rowIndex[0] = 0;
		for (int k = 0; k < result.L; k++) {
			fin >> j >> i >> v;
			result.values[k] = v;
			result.columns[k] = i;
			result.rowIndex[j + 1] = k + 1;
		}
		result.rowIndex[result.M] = result.L;
		for (int l = 1; l < result.rowIndex.size(); l++) {
			if (result.rowIndex[l] < 0)
				result.rowIndex[l] = result.rowIndex[l - 1];
		}
		cout << "Read success" << endl;
		return result;
	}

	void CSR3::Write(string filename) {
		int j = 0;
		ofstream fout(filename);
		fout << M << " " << N << " " << L << "\n";
		for (int j = 0; j < M; j++) {
			for (int i = rowIndex[j]; i < rowIndex[j + 1]; i++)
				fout << j << " " << columns[i] << " " << values[i] << "\n";
		}
	}

	Matrix CSR3::Convert() {
		Matrix Matr(N, M, 0.0);
		for (int j = 0; j < M; j++) {
			for (int k = rowIndex[j]; k < rowIndex[j + 1]; k++) {
				Matr.set_elem(columns[k], j, values[k]);
			}
		}
		cout << "Convert success" << endl;
		return Matr;
	}

	void CSR3::Transpose() {
		CSR3 res = this->new_Transpose();
		this->M = res.M;
		this->N = res.N;
		this->L = res.L;
		this->values = res.values;
		this->columns = res.columns;
		this->rowIndex = res.rowIndex;
	}
	CSR3 CSR3::new_Transpose() {
		cout << "Transpose" << endl;
		CSR3 transposed;
		transposed.M = N;
		transposed.N = M;
		transposed.L = L;

		transposed.values.resize(L);
		transposed.columns.resize(L);
		transposed.rowIndex.resize(transposed.M + 1, 0);

		for (int k = 0; k < L; k++) {
			int col = columns[k];  
			transposed.rowIndex[col + 1]++;
		}

		for (int i = 1; i <= transposed.M; i++) {
			transposed.rowIndex[i] += transposed.rowIndex[i - 1];
		}

		std::vector<int> current_pos = transposed.rowIndex;  

		for (int j = 0; j < M; j++) {
			for (int k = rowIndex[j]; k < rowIndex[j + 1]; k++) {
				int i = columns[k];
				double val = values[k];

				int pos = current_pos[i];
				transposed.values[pos] = val;
				transposed.columns[pos] = j;
				current_pos[i]++;
			}
		}

		return transposed;
	}

	CSR3 operator+(const CSR3& A, const CSR3& B) {
		CSR3 C;
		C.M = A.M; C.N = A.N;
		C.rowIndex.resize(C.M + 1);
		C.rowIndex[0] = 0;

		for (int j = 0; j < A.M; j++) {
			int a_start = A.rowIndex[j];
			int a_end = A.rowIndex[j + 1];
			int b_start = B.rowIndex[j];
			int b_end = B.rowIndex[j + 1];

			int a_now = a_start, b_now = b_start; 

			while (a_now < a_end && b_now < b_end) {
				int a_col = A.columns[a_now];
				int b_col = B.columns[b_now];

				if (a_col < b_col) {
					C.values.push_back(A.values[a_now]);
					C.columns.push_back(a_col);
					a_now++;
				}
				else if (b_col < a_col) {
					C.values.push_back(B.values[b_now]);
					C.columns.push_back(b_col);
					b_now++;
				}
				else {
					double sum = A.values[a_now] + B.values[b_now];
					if (abs(sum) > 1e-12) { 
						C.values.push_back(sum);
						C.columns.push_back(a_col);
					}
					a_now++;
					b_now++;
				}
			}

			while (a_now < a_end) {
				C.values.push_back(A.values[a_now]);
				C.columns.push_back(A.columns[a_now]);
				a_now++;
			}

			while (b_now < b_end) {
				C.values.push_back(B.values[b_now]);
				C.columns.push_back(B.columns[b_now]);
				b_now++;
			}

			C.rowIndex[j + 1] = C.values.size();
		}

		C.L = C.values.size();
		return C;
	}

	CSR3 CSR3::operator*(double c) const {
		CSR3 result(*this);
		for (int i = 0; i < result.L; i++) {
			result.values[i] *= c;
		}
		return result;
	}

	CSR3 CSR3::operator-() const {
		return *this * (-1.0);
	}

	CSR3 CSR3::operator-(CSR3& B) const {
		return *this + (-B);
	}

	vector <double> CSR3::operator()(vector <double> vec) const {
		vector<double> result(N, 0.0);

		for (int j = 0; j < M; j++) {
			double sum = 0.0;
			for (int k = rowIndex[j]; k < rowIndex[j + 1]; k++) {
				sum += values[k] * vec[columns[k]];
			}
			result[j] = sum;
		}

		return result;
	}
	double CSR3::operator[](pair<int, int> coordinates) const {
		int i = coordinates.first;
		int j = coordinates.second;
		for (int idx = rowIndex[i]; idx < rowIndex[i + 1]; idx++) {
			if (columns[idx] == j) {
				return values[idx];
			}
		}

		return 0.0;
	}
}