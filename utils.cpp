#include "utils.hpp"

int uniform_distirbution(int rangeLow, int rangeHigh) {
    // Generate a uniform distribution in the range [rangeLow, rangeHigh)
    double unit_rand = rand() / (1.0 + (double)RAND_MAX);
    int rand_range = rangeHigh - rangeLow + 1;
    int rand_scaled = (int)(unit_rand * rand_range) + rangeLow;
    return rand_scaled;
}

void print_vector(const vector<int>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) 
        printf("%d ", vec[i]);
    puts("");
}

void print_matrix(const vector<vector<int>>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        for (size_t j = 0; j < vec[0].size(); ++j) 
            printf("%d ", vec[i][j]);
        puts("");
    }
}

void matmul(const vector<vector<int>> a, 
    const vector<vector<int>> b, vector<vector<int>>& result) {
    if (a[0].size() != b.size()) {
        printf("size mismatch: %d != %d\n", a[0].size(), b.size());
        return;
    }

    int m = a.size();
    int n = a[0].size();
    int p = b[0].size();

    for (int i=0; i<m; i++)
        for (int j=0; j<p; j++) {
            result[i][j] = 0;
            for (int k=0; k<n; k++) 
                result[i][j] += a[i][k] * b[k][j];
        }
}