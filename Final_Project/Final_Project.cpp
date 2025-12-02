#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <tuple>
#include "CSR3.h"    
#include "SLAY.h"    
#include "Matrix.h"

using namespace std;
using namespace Matrixes;

void createDenseTestFile(const string& filename, int rows, int cols, const vector<double>& data) {
    ofstream fout(filename);
    if (!fout.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return;
    }
    fout << rows << " " << cols << endl;

    int count = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (count < data.size()) {
                fout << data[count++] << " ";
            }
            else {
                fout << "0.0 ";
            }
        }
        fout << endl;
    }
    fout.close();
}

void createSparseTestFile(const string& filename, int M, int N, int L, const vector<tuple<int, int, double>>& data) {
    ofstream fout(filename);
    if (!fout.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return;
    }
    fout << M << " " << N << " " << L << endl;
    for (const auto& item : data) {
        fout << get<0>(item) << " " << get<1>(item) << " " << get<2>(item) << endl;
    }
    fout.close();
}

int main() {
    cout << "==========================================" << endl;
    cout << "     MODULE: DENSE CHOLESKY TESTS         " << endl;
    cout << "==========================================" << endl << endl;

    cout << "[TEST 1] Solving valid SPD matrix (3x3)..." << endl;

    string d_file1 = "dense_test_valid.txt";
    createDenseTestFile(d_file1, 3, 3, {
        4.0, 1.0, 1.0,
        1.0, 4.0, 1.0,
        1.0, 1.0, 4.0
        });

    try {
        Matrix A = Matrix::from_file(d_file1);
        vector<double> b = { 6.0, 6.0, 6.0 };

        vector<double> x = SLAYSolver::solve_cholesky(A, b);

        cout << "Result x: { ";
        bool passed = true;
        for (double val : x) {
            cout << fixed << setprecision(4) << val << " ";
            if (abs(val - 1.0) > 1e-4) passed = false;
        }
        cout << "}" << endl;

        if (passed) cout << "-> STATUS: PASSED" << endl;
        else        cout << "-> STATUS: FAILED" << endl;

    }
    catch (const exception& e) {
        cout << "-> STATUS: FAILED (Exception: " << e.what() << ")" << endl;
    }
    cout << "------------------------------------------" << endl << endl;

    cout << "[TEST 2] Testing non-positive definite matrix..." << endl;

    string d_file2 = "dense_test_invalid.txt";
    createDenseTestFile(d_file2, 2, 2, {
        1.0, 2.0,
        2.0, 1.0
        });

    try {
        Matrix A = Matrix::from_file(d_file2);
        vector<double> b = { 1.0, 1.0 };

        vector<double> x = SLAYSolver::solve_cholesky(A, b);

        cout << "-> STATUS: FAILED (Should have thrown exception)" << endl;

    }
    catch (const runtime_error& e) {
        cout << "Caught expected exception: " << e.what() << endl;
        cout << "-> STATUS: PASSED" << endl;
    }
    catch (const exception& e) {
        cout << "-> STATUS: FAILED (Wrong exception type)" << endl;
    }

    cout << "==========================================" << endl;
    cout << "     MODULE: SPARSE CHOLESKY TESTS        " << endl;
    cout << "==========================================" << endl << endl;

    cout << "[TEST 1] Solving valid SPD matrix (3x3)..." << endl;
    string s_file1 = "sparse_test_valid.txt";
    createSparseTestFile(s_file1, 3, 3, 7, {
        {0, 0, 4.0}, {0, 1, 1.0},
        {1, 0, 1.0}, {1, 1, 4.0}, {1, 2, 1.0},
        {2, 1, 1.0}, {2, 2, 4.0}
        });

    try {
        CSR3 A = CSR3::Read(s_file1);
        vector<double> b = { 5.0, 6.0, 5.0 };

        vector<double> x = SLAYSolver::solve_cholesky(A, b);

        cout << "Result x: { ";
        bool passed = true;
        for (double val : x) {
            cout << fixed << setprecision(4) << val << " ";
            if (abs(val - 1.0) > 1e-4) passed = false;
        }
        cout << "}" << endl;

        if (passed) cout << "-> STATUS: PASSED" << endl;
        else        cout << "-> STATUS: FAILED" << endl;

    }
    catch (const exception& e) {
        cout << "-> STATUS: FAILED (Exception: " << e.what() << ")" << endl;
    }
    cout << "------------------------------------------" << endl << endl;

    cout << "[TEST 2] Testing non-positive definite matrix..." << endl;
    string s_file2 = "sparse_test_invalid.txt";
    createSparseTestFile(s_file2, 2, 2, 4, {
        {0, 0, 1.0}, {0, 1, 2.0},
        {1, 0, 2.0}, {1, 1, 1.0}
        });

    try {
        CSR3 A = CSR3::Read(s_file2);
        vector<double> b = { 1.0, 1.0 };

        vector<double> x = SLAYSolver::solve_cholesky(A, b);

        cout << "-> STATUS: FAILED (Should have thrown exception)" << endl;

    }
    catch (const runtime_error& e) {
        cout << "Caught expected exception: " << e.what() << endl;
        cout << "-> STATUS: PASSED" << endl;
    }
    catch (...) {
        cout << "-> STATUS: FAILED (Wrong exception type)" << endl;
    }

    return 0;
}