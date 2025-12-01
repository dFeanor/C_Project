#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <iomanip>
#include "CSR3.h"    
#include "SLAY.h"    

using namespace std;
using namespace Matrixes;

void createTestFile(const string& filename, int M, int N, int L, const vector<tuple<int, int, double>>& data) {
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
    cout << "     MODULE 2: SPARSE CHOLESKY TESTS      " << endl;
    cout << "==========================================" << endl << endl;

    cout << "[TEST 1] Solving valid SPD matrix (3x3)..." << endl;
    string file1 = "test_valid.txt";
    createTestFile(file1, 3, 3, 7, {
        {0, 0, 4.0}, {0, 1, 1.0},
        {1, 0, 1.0}, {1, 1, 4.0}, {1, 2, 1.0},
        {2, 1, 1.0}, {2, 2, 4.0}
    });

    try {
        CSR3 A = CSR3::Read(file1);
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
        else        cout << "-> STATUS: FAILED (Values incorrect)" << endl;

    } catch (const exception& e) {
        cout << "-> STATUS: FAILED (Exception: " << e.what() << ")" << endl;
    }
    cout << "------------------------------------------" << endl << endl;

    cout << "[TEST 2] Testing non-positive definite matrix (Error handling)..." << endl;
    string file2 = "test_invalid.txt";
    createTestFile(file2, 2, 2, 4, {
        {0, 0, 1.0}, {0, 1, 2.0},
        {1, 0, 2.0}, {1, 1, 1.0}
    });

    try {
        CSR3 A = CSR3::Read(file2);
        vector<double> b = { 1.0, 1.0 };
        
        vector<double> x = SLAYSolver::solve_cholesky(A, b);
        
        cout << "-> STATUS: FAILED (Method should have thrown an exception but didn't)" << endl;

    } catch (const runtime_error& e) {
        cout << "Caught expected exception: " << e.what() << endl;
        cout << "-> STATUS: PASSED (Error correctly detected)" << endl;
    } catch (...) {
        cout << "-> STATUS: FAILED (Wrong exception type)" << endl;
    }

    return 0;
}