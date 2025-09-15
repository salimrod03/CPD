/*#include <iostream>
#include <vector>
#include <chrono>
using namespace std;
using namespace chrono;

int main() {
    int N;
    cout << "N: ";
    cin >> N;

    vector<double> A(N * N, 1.0);
    vector<double> x(N, 1.0);
    vector<double> y(N, 0.0);

    //caso i y j
    auto start1 = high_resolution_clock::now();
    for (int i = 0; i < N; i++) {
        y[i] = 0.0;
        for (int j = 0; j < N; j++) {
            y[i] += A[i * N + j] * x[j];
        }
    }
    auto end1 = high_resolution_clock::now();
    cout << "Tiempo fila por columna: "
        << duration_cast<milliseconds>(end1 - start1).count()
        << " ms" << endl;

    //caso j e i
    fill(y.begin(), y.end(), 0.0);
    auto start2 = high_resolution_clock::now();
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            y[i] += A[i * N + j] * x[j];
        }
    }
    auto end2 = high_resolution_clock::now();
    cout << "Tiempo columna por fila: "
        << duration_cast<milliseconds>(end2 - start2).count()
        << " ms" << endl;

    return 0;
}
*/