#include "PngProc.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <fstream>
using namespace std;

int* ImageProcessingGray(unsigned char* pOut
			 , unsigned char* pIn
			 , size_t nWidth
			 , size_t nHeight);

void fill_csv(int* gist) {
	const char csv_file_name[64] = "data.csv";
	std::ofstream csv_file;
	csv_file.open(csv_file_name);
	csv_file << "values,number\n";
	for (int i = 0; i < 256; i++) {
		csv_file << i << "," << gist[i] << "\n";
		//csv_file << i << "," << 0 << "\n";
	}
	csv_file.close();
	return;
}

double mean(int* gist, size_t nWidth, size_t nHeight) {
	double res = 0;
	for (int i = 0; i < 256; i++) {
		res = res + i * gist[i];
	}
	res = res / (nWidth * nHeight);
	return res;
}

double variance(int* gist, double mean, size_t nWidth, size_t nHeight) {
	double res = 0;
	for (int i = 0; i < 256; i++) {
		res = res + (i - mean) * (i - mean) * gist[i];
	}
	res = res / (nWidth * nHeight);
	return res;
}

int quartile(int* gist, size_t nWidth, size_t nHeight) {
	int quart1 = nWidth * nHeight / 4;
	int quart3 = nWidth * nHeight * 3 / 4;
	int res1 = 0;
	int res3 = 0;
	int i1 = 0;
	while (res1 < quart1) {
		res1 = res1 + gist[i1];
		i1++;
	}
	int i2 = 0;
	while (res3 < quart3) {
		res3 = res3 + gist[i2];
		i2++;
	}
	return i2 -i1;
}


int main(int argc, char* argv[])
{
	class CBitsPtrGuard
	{
	public:
		CBitsPtrGuard(unsigned char** pB) : m_ppBits(pB) { }
		~CBitsPtrGuard() { if (*m_ppBits) delete *m_ppBits, *m_ppBits = 0; }
	protected:
		unsigned char** m_ppBits;
	};

	// parse input parameters
	char	szInputFileName[256];
	char    szOutputFileName[256];
	if (argc < 2)
		printf("\nformat: pngtest <input_file> [<output_file>]");
	else 
	{
		strcpy(szInputFileName, argv[1]);
		if (argc > 2)
			strcpy(szOutputFileName, argv[2]);
		else
		{
			strcpy(szOutputFileName, szInputFileName);
			strcat(szOutputFileName, "_out.png");
		}
	}


	size_t nReqSize = NPngProc::readPngFile(szInputFileName, 0, 0, 0, 0);
	if (nReqSize == NPngProc::PNG_ERROR)
	{
		printf("\nError ocured while pngfile was read");
		return -1;
	}
	

	unsigned char* pInputBits = new unsigned char[nReqSize];
	if (!pInputBits)
	{
		printf("\nCan't allocate memory for image, required size is %u", nReqSize);
		return -1;
	}
	CBitsPtrGuard InputBitsPtrGuard(&pInputBits);


	unsigned char* pOutputBits = new unsigned char[nReqSize];
	if (!pOutputBits)
	{
		printf("\nCan't allocate memory for image, required size is %u", nReqSize);
		return -1;
	}


	CBitsPtrGuard OutputBitsPtrGuard(&pOutputBits);

	size_t nWidth, nHeight;
	unsigned int nBPP;

	size_t nRetSize = NPngProc::readPngFileGray(szInputFileName, pInputBits, &nWidth, &nHeight/*, &nBPP*/);
	nBPP = 8;


	// ASSERT(nRetSize == nReqSize);

	// TODO: image processing 
	int* gist = ImageProcessingGray(pOutputBits, pInputBits, nWidth, nHeight);
	fill_csv(gist);

	double mean_ = mean(gist, nWidth, nHeight);
	double variance_ = variance(gist, mean_, nWidth, nHeight);
	cout << "Mean: " << mean_ << endl;
	cout << "Variance: " << variance_ << endl;
	cout << "Quartile delta: " << quartile(gist, nWidth, nHeight) << endl;

	if (NPngProc::writePngFile(szOutputFileName, pOutputBits, nWidth, nHeight, nBPP) == NPngProc::PNG_ERROR)
	{
		printf("\nError ocuured during png file was written");
		return -1;
	}

	return 0;
}

int* ImageProcessingGray(unsigned char* pOut, unsigned char* pIn, size_t nWidth, size_t nHeight) {
	int* gist = new int[256];
	for (int i = 0; i < 256; i++) gist[i] = 0;
	for (size_t y = 0; y < nHeight; ++y) {
		for (size_t x = 0; x < nWidth; ++x) {
			gist[*pIn] = gist[*pIn] + 1;
			*pOut++ = *pIn++;	
		}
	return gist;
}
