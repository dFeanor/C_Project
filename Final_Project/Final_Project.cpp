// #include <iostream>
// #include <vector>
// #include <fstream>
// #include <cmath>
// #include <iomanip>
// #include <tuple>
// #include "CSR3.h"    
// #include "SLAY.h"    
// #include "Matrix.h"

// using namespace std;
// using namespace Matrixes;

// void createDenseTestFile(const string& filename, int rows, int cols, const vector<double>& data) {
//     ofstream fout(filename);
//     if (!fout.is_open()) {
//         cerr << "Error: Could not create file " << filename << endl;
//         return;
//     }
//     fout << rows << " " << cols << endl;

//     int count = 0;
//     for (int i = 0; i < rows; ++i) {
//         for (int j = 0; j < cols; ++j) {
//             if (count < data.size()) {
//                 fout << data[count++] << " ";
//             }
//             else {
//                 fout << "0.0 ";
//             }
//         }
//         fout << endl;
//     }
//     fout.close();
// }

// void createSparseTestFile(const string& filename, int M, int N, int L, const vector<tuple<int, int, double>>& data) {
//     ofstream fout(filename);
//     if (!fout.is_open()) {
//         cerr << "Error: Could not create file " << filename << endl;
//         return;
//     }
//     fout << M << " " << N << " " << L << endl;
//     for (const auto& item : data) {
//         fout << get<0>(item) << " " << get<1>(item) << " " << get<2>(item) << endl;
//     }
//     fout.close();
// }

// int main() {
//     /*
//     //SLAYSolver solver;
//     cout << "==========================================" << endl;
//     cout << "     MODULE: DENSE CHOLESKY TESTS         " << endl;
//     cout << "==========================================" << endl << endl;

//     cout << "[TEST 1] Solving valid SPD matrix..." << endl;
//     string path = "module2_2\\sparse\\3\\";
//     string d_file1 = path + "matrix.txt";
//     /*createDenseTestFile(d_file1, 3, 3, {
//         4.0, 1.0, 1.0,
//         1.0, 4.0, 1.0,
//         1.0, 1.0, 4.0
//         });*/
//     /*
//     try {
//         Matrix A = Matrix::from_file(d_file1);
//         vector<double> b = SLAYSolver::read_vector_from_file(path + "b.txt");

//         vector<double> x = SLAYSolver::solve_cholesky(A, b);
//         SLAYSolver::write_vector_to_file(path + "x_out.txt", x);
//         cout << "Result x: { ";
//         bool passed = true;
//         for (double val : x) {
//             cout << fixed << setprecision(4) << val << " ";
//             //if (abs(val - 1.0) > 1e-4) passed = false;
//         }
//         cout << "}" << endl;

//         if (passed) cout << "-> STATUS: PASSED" << endl;
//         else        cout << "-> STATUS: FAILED" << endl;

//     }
//     catch (const exception& e) {
//         cout << "-> STATUS: FAILED (Exception: " << e.what() << ")" << endl;
//     }
//     cout << "------------------------------------------" << endl << endl;
//     /*
//     cout << "[TEST 2] Testing non-positive definite matrix..." << endl;

//     string d_file2 = "dense_test_invalid.txt";
//     createDenseTestFile(d_file2, 2, 2, {
//         1.0, 2.0,
//         2.0, 1.0
//         });

//     try {
//         Matrix A = Matrix::from_file(d_file2);
//         vector<double> b = SLAYSolver::read_vector_from_file("bdense2.txt");//{ 1.0, 1.0 };

//         vector<double> x = SLAYSolver::solve_cholesky(A, b);
//         SLAYSolver::write_vector_to_file("densex_out2.txt", x);
//         cout << "-> STATUS: FAILED (Should have thrown exception)" << endl;

//     }
//     catch (const runtime_error& e) {
//         cout << "Caught expected exception: " << e.what() << endl;
//         cout << "-> STATUS: PASSED" << endl;
//     }
//     catch (const exception& e) {
//         cout << "-> STATUS: FAILED (Wrong exception type)" << endl;
//     }
//     */
//     cout << "==========================================" << endl;
//     cout << "     MODULE: SPARSE CHOLESKY TESTS        " << endl;
//     cout << "==========================================" << endl << endl;

//     cout << "[TEST 1] Solving valid SPD matrix..." << endl;
//     string path = "module2_2\\sparse\\3\\";
//     string s_file1 = "matrix.txt";
//     /*createSparseTestFile(s_file1, 3, 3, 7, {
//         {0, 0, 4.0}, {0, 1, 1.0},
//         {1, 0, 1.0}, {1, 1, 4.0}, {1, 2, 1.0},
//         {2, 1, 1.0}, {2, 2, 4.0}
//         });*/

//     try {
//         CSR3 A = CSR3::Read(path + s_file1);
//         vector<double> b = SLAYSolver::read_vector_from_file(path + "b.txt");//{ 5.0, 6.0, 5.0 };

//         vector<double> x = SLAYSolver::solve_cholesky(A, b);
//         SLAYSolver::write_vector_to_file(path + "x_out.txt", x);
//         cout << "Result x: { ";
//         bool passed = true;
//         for (double val : x) {
//             cout << fixed << setprecision(4) << val << " ";
//             //if (abs(val - 1.0) > 1e-4) passed = false;
//         }
//         cout << "}" << endl;

//         if (passed) cout << "-> STATUS: PASSED" << endl;
//         else        cout << "-> STATUS: FAILED" << endl;

//     }
//     catch (const exception& e) {
//         cout << "-> STATUS: FAILED (Exception: " << e.what() << ")" << endl;
//     }
//     cout << "------------------------------------------" << endl << endl;
//     /*
//     cout << "[TEST 2] Testing non-positive definite matrix..." << endl;
//     string s_file2 = "sparse_test_invalid.txt";
//     createSparseTestFile(s_file2, 2, 2, 4, {
//         {0, 0, 1.0}, {0, 1, 2.0},
//         {1, 0, 2.0}, {1, 1, 1.0}
//         });

//     try {
        
//         CSR3 A = CSR3::Read(s_file2);
//         vector<double> b = SLAYSolver::read_vector_from_file("bdense2.txt");//{ 1.0, 1.0 };

//         vector<double> x = SLAYSolver::solve_cholesky(A, b);
//         SLAYSolver::write_vector_to_file("csrx_out2.txt", x);
//         cout << "-> STATUS: FAILED (Should have thrown exception)" << endl;

//     }
//     catch (const runtime_error& e) {
//         cout << "Caught expected exception: " << e.what() << endl;
//         cout << "-> STATUS: PASSED" << endl;
//     }
//     catch (...) {
//         cout << "-> STATUS: FAILED (Wrong exception type)" << endl;
//     }
//     */


    
//     return 0;
// }


// #include <iostream>
// #include <vector>
// #include <fstream>
// #include <cmath>
// #include <iomanip>
// #include <tuple>
// #include <string>

// // Подключаем твои заголовочные файлы
// #include "CSR3.h"    
// #include "SLAY.h"    
// #include "Matrix.h"

// using namespace std;
// using namespace Matrixes;

// // --- Вспомогательные функции для генерации тестов ---

// // Создает файл с разреженной матрицей (формат: M N L, затем строки: row col val)
// void createSparseMatrixFile(const string& filename, int Rows, int Cols, const vector<tuple<int, int, double>>& elements) {
//     ofstream fout(filename);
//     if (!fout.is_open()) {
//         cerr << "Error: Could not create file " << filename << endl;
//         return;
//     }
//     fout << Rows << " " << Cols << " " << elements.size() << endl;
//     for (const auto& item : elements) {
//         fout << get<0>(item) << " " << get<1>(item) << " " << get<2>(item) << endl;
//     }
//     fout.close();
// }

// // Создает файл с вектором правой части (формат: N, затем значения)
// void createVectorFile(const string& filename, const vector<double>& vec) {
//     ofstream fout(filename);
//     fout << vec.size() << endl; // Обычно формат вектора начинается с размера
//     for (double val : vec) {
//         fout << val << endl;
//     }
//     fout.close();
// }

// // Функция для чтения вектора (простая реализация, если в SLAY ее нет или она другая)
// vector<double> loadVector(const string& filename) {
//     ifstream fin(filename);
//     int n;
//     fin >> n;
//     vector<double> res(n);
//     for (int i = 0; i < n; i++) fin >> res[i];
//     return res;
// }

// int main() {
//     cout << "==========================================" << endl;
//     cout << "     MODULE 2: SPARSE CHOLESKY TESTS      " << endl;
//     cout << "==========================================" << endl << endl;

//     // ---------------------------------------------------------
//     // ТЕСТ 1: Корректная симметричная положительно определенная матрица
//     // Система:
//     // 4x + 1y = 5
//     // 1x + 4y = 5
//     // Решение должно быть: x=1, y=1
//     // ---------------------------------------------------------
//     cout << "[TEST 1] Solving valid SPD matrix (2x2)..." << endl;
    
//     string s_file1 = "test_matrix_valid.txt";
//     string b_file1 = "test_vector_valid.txt";

//     // 1. Создаем файлы с данными
//     createSparseMatrixFile(s_file1, 2, 2, {
//         {0, 0, 4.0}, {0, 1, 1.0},
//         {1, 0, 1.0}, {1, 1, 4.0}
//     });
//     createVectorFile(b_file1, { 5.0, 5.0 });

//     try {
//         // 2. Считываем
//         CSR3 A = CSR3::Read(s_file1);
//         vector<double> b = loadVector(b_file1);

//         // 3. Решаем
//         vector<double> x = SLAYSolver::solve_cholesky(A, b);

//         // 4. Проверяем результат
//         cout << "Result x: { ";
//         bool passed = true;
//         for (double val : x) {
//             cout << fixed << setprecision(4) << val << " ";
//             if (abs(val - 1.0) > 1e-4) passed = false;
//         }
//         cout << "}" << endl;

//         if (passed) cout << "-> STATUS: PASSED" << endl;
//         else        cout << "-> STATUS: FAILED (Expected {1.0000, 1.0000})" << endl;

//     }
//     catch (const exception& e) {
//         cout << "-> STATUS: FAILED (Exception: " << e.what() << ")" << endl;
//     }
//     cout << "------------------------------------------" << endl << endl;


//     // ---------------------------------------------------------
//     // ТЕСТ 2: Матрица НЕ положительно определенная
//     // Матрица:
//     // 1 2
//     // 2 1
//     // (Определитель 1-4 = -3 < 0, метод должен упасть)
//     // ---------------------------------------------------------
//     cout << "[TEST 2] Testing non-positive definite matrix..." << endl;
//     string s_file2 = "test_matrix_invalid.txt";
//     string b_file2 = "test_vector_invalid.txt";

//     createSparseMatrixFile(s_file2, 2, 2, {
//         {0, 0, 1.0}, {0, 1, 2.0},
//         {1, 0, 2.0}, {1, 1, 1.0}
//     });
//     createVectorFile(b_file2, { 1.0, 1.0 });

//     try {
//         CSR3 A = CSR3::Read(s_file2);
//         vector<double> b = loadVector(b_file2);

//         cout << "Attempting to solve..." << endl;
//         vector<double> x = SLAYSolver::solve_cholesky(A, b);
        
//         // Если мы здесь, значит ошибка не вылетела -> тест провален
//         cout << "-> STATUS: FAILED (Should have thrown exception)" << endl;
//     }
//     catch (const runtime_error& e) {
//         cout << "Caught expected exception: " << e.what() << endl;
//         cout << "-> STATUS: PASSED" << endl;
//     }
//     catch (...) {
//         cout << "-> STATUS: FAILED (Wrong exception type)" << endl;
//     }

//     cout << "------------------------------------------" << endl;
//     cout << "Tests finished." << endl;

//     return 0;
// }