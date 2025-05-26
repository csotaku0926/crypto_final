#include <cmath>
#include <vector>
#include <cstdlib>
#include <cstdio>

using namespace std;

int uniform_distirbution(int rangeLow, int rangeHigh);
void print_vector(const vector<int>& vec);
void print_matrix(const vector<vector<int>>& vec);
void matmul(const vector<vector<int>> a, 
    const vector<vector<int>> b, vector<vector<int>>& result);