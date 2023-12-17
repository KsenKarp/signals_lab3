#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include<algorithm>

using namespace std;

vector<double>* signal_values(const char* csv_file_name) {
	vector<double>* res = new vector<double>();
    std::ifstream csv_file;
    csv_file.open(csv_file_name);
    
    string line;
    string parse = ",";
    
    getline(csv_file, line); //heading
    while (getline(csv_file, line)){
        size_t pos = 0;
        string token[3];
        int i = 0;
        while ((pos = line.find(parse)) != string::npos) {
            token[i] = line.substr(0, pos);
            i++;
            line.erase(0, pos + parse.length());
        }
        res->push_back(stod(token[1]));
    }

    csv_file.close();
    sort(res->begin(), res->end());
    return res;
}

void gist(vector<double>* values, int n) { //n - количество окошек
    vector<int>* gist = new vector<int>(n, 0);
    double min = values->at(0);
    double max = values->back();
    double delta = (max - min) / n;

    for (int i = 0; i < values->size(); i++) {
        int j = floor((values->at(i) - min) / delta);
        if (values->at(i) == max) gist->at(n - 1) = gist->at(n - 1) + 1;
        else gist->at(j) = gist->at(j) + 1;
    }
    
    const char csv_file_name[64] = "data.csv";
    std::ofstream csv_file;
    csv_file.open(csv_file_name);
    csv_file << "values,number\n";
    csv_file << min << "," << 0 << "\n";
    for (int i = 0; i < gist->size(); i++) {
        csv_file << min << "," << gist->at(i) << "\n";
        csv_file << min + delta << "," << gist->at(i) << "\n";
        min = min + delta;
    }
    csv_file << max << "," << 0 << "\n";
    csv_file.close();
}

int main() {
    vector<double>* out = signal_values("data1.csv");
    gist(out, 10);
    return 0;
}
