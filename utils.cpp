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