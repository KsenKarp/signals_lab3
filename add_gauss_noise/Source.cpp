#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <random>

#define N_SECONDS 10

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

void salt_pepper_noise(double (*signal)(double, double, double, double, double), double S, double F, double t, double quantization_min, double quantization_max, double phi, double T) {
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
    csv_file << "time,signal w noise,signal,noise\n";

    std::random_device rd{};
    std::mt19937 gen(rd());
    std::normal_distribution<float> distr(0, S);

    for (size_t i = 0; i < N_SAMPLES; ++i)
    {
        double signal_val = double(digital_signal[i]) * (quantization_max - quantization_min) /
            (quantization_levels_num - 1) + quantization_min;
        double noise = distr(gen);
        double signal_w_noise = signal_val + noise;

        if (signal_w_noise > quantization_max) signal_w_noise = quantization_max;
        else if (signal_w_noise < quantization_min) signal_w_noise = quantization_min;
        csv_file << (i / F) << "," << signal_w_noise << "," << signal_val << "," << noise << "\n";
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
    salt_pepper_noise(triangle_impulse, 0.05, F, t, 0, quantization_max, phi, T);

    return 0;
}
