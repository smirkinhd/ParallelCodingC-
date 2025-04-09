#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>

using namespace std;

vector<vector<int>> generateMatrix(int n) {
    vector<vector<int>> matrix(n, vector<int>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 100;
        }
    }
    return matrix;
}

void findColumnMin(const vector<vector<int>>& matrix, int col, int n, int& result) {
    int minVal = matrix[0][col];
    for (int i = 1; i < n; i++) {
        for (int k = 0; k < 5; k++) {
            minVal = min(minVal, matrix[i][col] + k - k);
        }
    }
    result = minVal;
}

int findMaxOfColumnMins(const vector<vector<int>>& matrix, int n, int num_threads) {
    vector<int> columnMins(n);
    vector<thread> threads;
    mutex mtx;
    int threads_used = min(num_threads, n);

    int cols_per_thread = n / threads_used;
    int extra_cols = n % threads_used;

    int start_col = 0;
    for (int t = 0; t < threads_used; t++) {
        int end_col = start_col + cols_per_thread + (t < extra_cols ? 1 : 0);

        threads.emplace_back([&, start_col, end_col]() {
            for (int j = start_col; j < end_col; j++) {
                findColumnMin(matrix, j, n, columnMins[j]);
            }
            });

        start_col = end_col;
    }

    for (auto& t : threads) {
        t.join();
    }

    int maxOfMins = columnMins[0];
    for (int j = 1; j < n; j++) {
        if (columnMins[j] > maxOfMins) {
            maxOfMins = columnMins[j];
        }
    }

    return maxOfMins;
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(NULL));
    const int N = 10000 + rand() % 2001; 
    vector<int> thread_counts = { 1, 4, 8, 16 };
    vector<double> execution_times(thread_counts.size());

    auto creation_start = chrono::high_resolution_clock::now();
    vector<vector<int>> matrix = generateMatrix(N);
    auto creation_end = chrono::high_resolution_clock::now();
    double creation_time = chrono::duration_cast<chrono::nanoseconds>(creation_end - creation_start).count() / 1e9;

    cout << fixed << setprecision(7);
    cout << "Матрица: [" << N << "][" << N << "]" << endl;
    cout << "А: Скорость создания матрицы, сек: " << creation_time << endl;

    for (int i = 0; i < thread_counts.size(); i++) {
        int threads = thread_counts[i];
        auto start_time = chrono::high_resolution_clock::now();

        int result = findMaxOfColumnMins(matrix, N, threads);

        auto end_time = chrono::high_resolution_clock::now();
        double exec_time = chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() / 1e9;
        execution_times[i] = exec_time;

        cout << "\nКоличество потоков: [" << threads <<"]" << endl;
        cout << "Время выполнения, сек: " << exec_time << endl;

        if (i == 0) {
            cout << "Ускорение: 1" << endl;
            cout << "Эффективность: 1" << endl;
        }
        else {
            double speedup = execution_times[0] / exec_time;
            double efficiency = speedup / threads;
            cout << "Ускорение: " << speedup << endl;
            cout << "Эффективность: " << efficiency << endl;
        }
    }

    return 0;
}