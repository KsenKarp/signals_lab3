#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>

#define N_SECONDS 10

int probability(double p0) {
    int m = rand() % 100;
    if (m < p0 * 100) return 1;
    else return 0;
}

double impulse_p(double x, double p, double* a, int n) {
    for (int i = 0; i < n; i++) {
        if (x == a[i]) return probability(p);
    }
    return 0;
}

double triangle_impulse(double x, double T, double t, double x_m, double phi) {
    x = x - phi;
    if (x < 0) {
        while (x < 0) x = x + T;
    }
    while (x > T) x = x - T;
    if (x <= t / 2) return 2 * x_m * x / t;
    else if (t / 2 < x && x < t) return 2 * x_m - 2 * x_m * x / t;
    else return 0;
}

double rectangle_impulse(double x, double T, double t, double x_m, double phi) {
    x = x - phi;
    if (x < 0)
        while (x < 0) x = x + T;
    while (x > T) x = x - T;
    if (x <= t) return x_m;
    else return 0;
}

void salt_pepper_noise(double (*signal)(double , double , double , double , double), double p, double F, double t, double quantization_min, double quantization_max, double phi, double T) {
	//double x, double T, double t, double x_m, double phi
    int n = N_SECONDS * F;
    int N_SAMPLES = N_SECONDS * F;
    double* a = new double[N_SAMPLES];
    for (int i = 0; i < N_SAMPLES; ++i) {
        if (i % 5 == 0) a[i] = i / F;
        else a[i] = -1;
    }
    uint8_t quantization_levels_num = 64;
    uint8_t* digital_signal = new uint8_t[N_SAMPLES];
    for (int i = 0; i < N_SAMPLES; i++) digital_signal[i] = (round(signal(i / F, T, t, quantization_max, phi) * (quantization_levels_num - 1))) /
        (quantization_max - quantization_min);
    const char csv_file_name[64] = "data.csv";
    std::ofstream csv_file;
    csv_file.open(csv_file_name);
    csv_file << "time, signal w noise\n";
    for (size_t i = 0; i < N_SAMPLES; ++i)
    {
        double p0 = probability(0.5);
        double signal_val = double(digital_signal[i]) * (quantization_max - quantization_min) /
            (quantization_levels_num - 1) + quantization_min;

        csv_file << (i / F) << "," << signal_val * (1 - p0) + p0 * impulse_p(i / F, p, a, n) << /* "," << signal_val << "," << impulse_p(i / F, p, a, n) <<*/ "\n";
    }
    csv_file.close();
}

int main() {

    double F = 10.;
    double quantization_max = 2.;
    double T = 6.;
    double t = 4.;
    double phi = 0.;
    //double p, double F, double t, double quantization_min, double quantization_max, double phi, double T
    salt_pepper_noise(triangle_impulse, 0.5, F, t, 0, quantization_max, phi, T);

    return 0;
}