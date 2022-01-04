#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <iomanip>

double sum(std::vector<double> a)
{
	double s = 0;
	for (int i = 0; i < a.size(); i++)
	{
		s += a[i];
	}
	return s;
}

double mean(std::vector<double> a)
{
	return sum(a) / a.size();
}

double sqsum(std::vector<double> a)
{
	double s = 0;
	for (int i = 0; i < a.size(); i++)
	{
		s += pow(a[i], 2);
	}
	return s;
}

double stdev(std::vector<double> nums)
{
	double N = nums.size();
	return pow(sqsum(nums) / N - pow(sum(nums) / N, 2), 0.5);
}

std::vector<double> operator-(std::vector<double> a, double b)
{
	std::vector<double> retvect;
	for (int i = 0; i < a.size(); i++)
	{
		retvect.push_back(a[i] - b);
	}
	return retvect;
}

std::vector<double> operator*(std::vector<double> a, std::vector<double> b)
{
	std::vector<double> retvect;
	for (int i = 0; i < a.size(); i++)
	{
		retvect.push_back(a[i] * b[i]);
	}
	return retvect;
}

double pearsoncoeff(std::vector<double> X, std::vector<double> Y)
{
	return sum((X - mean(X))*(Y - mean(Y))) / (X.size()*stdev(X)* stdev(Y));
}

//int main() {
//	/* Enter your code here. Read input from STDIN. Print output to STDOUT */
//	int N;
//	cin >> N;
//	vector<double> X(N);
//	vector<double> Y(N);
//
//	for (int i = 0; i < X.size(); i++)
//	{
//		cin >> X[i];
//	}
//	for (int i = 0; i < Y.size(); i++)
//	{
//		cin >> Y[i];
//	}
//
//	cout << fixed << setprecision(3) << pearsoncoeff(X, Y) << endl;
//
//	return 0;
//}
