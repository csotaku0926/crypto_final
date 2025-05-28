#include <cmath>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <stdint.h>
#include <fcntl.h>

using namespace std;

typedef unsigned int uint;
typedef vector<int> vint;
typedef vector<vector<int>> vvint;

class LWE;

int uniform_distirbution(int rangeLow, int rangeHigh);
void print_vector(const vector<int>& vec);
void print_vector(const vector<bool>& vec);
void print_matrix(const vector<vector<int>>& vec);
int matmul(const vector<vector<int>> a, 
    const vector<vector<int>> b, vector<vector<int>>& result);
void transpose(const vector<vector<int>>& a, 
    vector<vector<int>>& result);
vvint transpose(const vvint& a);