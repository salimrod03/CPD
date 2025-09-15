/*#include <iostream>
#include <vector>
#include <chrono>
using namespace std;
using namespace chrono;

int main() {
    int N;
    cout << "N: ";
    cin >> N;

    vector<double> matA(N * N, 1.0);
    vector<double> matB(N * N, 1.0);
    vector<double> matC(N * N, 0.0);

    auto start = high_resolution_clock::now();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double suma = 0.0;
            for (int k = 0; k < N; k++) {
                suma += matA[i * N + k] * matB[k * N + j];
            }
            matC[i * N + j] = suma;
        }
    }
    auto end = high_resolution_clock::now();
    cout << "Tiempo: "
        << duration_cast<milliseconds>(end - start).count()
        << " ms" << endl;

    return 0;
}
*/