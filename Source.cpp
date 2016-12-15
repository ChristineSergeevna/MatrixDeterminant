#include <iostream> 
#include <fstream>
#include <thread> 
#include <vector>
#include <ctime>  

typedef double** matrix;

void getMatr(matrix a, matrix p, int i, int j, int m) 
{
	int di = 0;
	for (int ki = 0; ki < m - 1; ki++)
	{ 
		if (ki == i) 
			di = 1;
		int dj = 0;
		for (int kj = 0; kj < m - 1; kj++)
		{
			if (kj == j) 
				dj = 1;
			p[ki][kj] = a[ki + di][kj + dj];
		}
	}
}

double det(matrix a, int m) 
{
	matrix p;
	p = new double*[m];
	for (int i = 0; i < m; i++)
		p[i] = new double[m];

	if (m < 1) std::cout << "Невозможно вычислить определитель матрицы размерности " << m;
	if (m == 1)	return a[0][0];
	if (m == 2) return a[0][0] * a[1][1] - (a[1][0] * a[0][1]);

	double d = 0;
	int k = 1;
	for (int i = 0; i < m; i++)
	{
		getMatr(a, p, i, 0, m);
		d = d + k * a[i][0] * det(p, m - 1);
		k = -k;
	}
	return d;
}

void threadWorker(int i, int threadsCount, matrix a, int m, int *subDets)
{
	for (int j = i; j < m; j += threadsCount) 
	{
		matrix p = new double*[m];
		for (int k = 0; k < m; k++)
			p[k] = new double[m];
		getMatr(a, p, j, 0, m);
		subDets[j] = det(p, m - 1);
		delete[] p;
	}
}

double parallelDet(matrix a, int m, int threadsCount)
{
	if (m < 3) return det(a, m);
	if (threadsCount < 1) threadsCount = 1;
	if (threadsCount > m) threadsCount = m;

	std::thread *threads = new std::thread[threadsCount];
	int *subDets = new int[m];
	for (int i = 0; i < threadsCount; ++i)
		threads[i] = std::thread(threadWorker, i, threadsCount, a, m, subDets);

	for (int i = 0; i < threadsCount; ++i)
		threads[i].join();

	delete[] threads;
	int d = 0, k = 1;
	for (int i = 0; i < m; ++i)
	{
		d = d + k * a[i][0] * subDets[i];
		k = -k;
	}
	delete[] subDets;
	return d;
}

int main() 
{
	clock_t time;
	std::ifstream fin("input.txt");
	matrix a;
	int m, threadsCount;
	fin >> m;
	std::cin >> threadsCount;
	a = new double*[m];
	for (int i = 0; i < m; i++) 
	{
		a[i] = new double[m];
		for (int j = 0; j < m; j++) 
			fin >> a[i][j];
	}
	time = clock();
	std::cout << parallelDet(a, m, threadsCount) << std::endl;
	std::cout << "runtime = " << (clock() - time) / 1000.0 << std::endl;
	system("pause");
	return 0;
}