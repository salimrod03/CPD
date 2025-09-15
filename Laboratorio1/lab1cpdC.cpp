#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
using namespace std;
using namespace chrono;

int main() {
    int N, BS;
    cout << "Ingrese el tamaño de la matriz: ";
    cin >> N;
    cout << "Ingrese el tamaño de bloque: ";
    cin >> BS;
    vector<double> A(N * N, 1.0);
    vector<double> B(N * N, 1.0);
    vector<double> C(N * N, 0.0);

    auto start = high_resolution_clock::now();
    for (int ii = 0; ii < N; ii += BS) {
        for (int jj = 0; jj < N; jj += BS) {
            for (int kk = 0; kk < N; kk += BS) {
                int iimax = min(ii + BS, N);
                int jjmax = min(jj + BS, N);
                int kkmax = min(kk + BS, N);
                for (int i = ii; i < iimax; i++) {
                    for (int j = jj; j < jjmax; j++) {
                        double suma = 0.0;
                        for (int k = kk; k < kkmax; k++) {
                            suma += A[i * N + k] * B[k * N + j];
                        }
                        C[i * N + j] += suma;
                    }
                }
            }
        }
    }
    auto end = high_resolution_clock::now();
    cout << "Tiempo: "
        << duration_cast<milliseconds>(end - start).count()
        << " ms" << endl;
    return 0;
}
