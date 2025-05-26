#include "utils.hpp"

class LWE {
    int _calc_error_bound() {
        // Calculate the error bound based on N and Q
        return (int)floor(sqrt(
            (double)this->N / (double)(4 * this->Q)
        ));
    }

    vector<int> _get_from_distribution(int sz) {
        vector<int> result(sz, 0); 
        for (int i = 0; i < sz; i++) {
            result[i] = uniform_distirbution(this->B[0], this->B[1]);
        }
        return result;
    }

public:
    int N; // Number of bits in secret key
    int Q; // Modulus
    int B[2]; // error bound
    vector<int> sk; // secret key
    
    LWE(int N, int Q) {
        srand(0xabcd); // Seed for reproducibility

        this->N = N;
        this->Q = Q;

        int b = this->_calc_error_bound();
        this->B[0] = -b; 
        this->B[1] = b+1;

        // secret key
        this->sk = this->_get_from_distribution(this->N);

    }
};

int main(void) {
    LWE lwe(32, 4);
    printf("B: %d\n", lwe.B[0]);
    print_vector(lwe.sk);
}