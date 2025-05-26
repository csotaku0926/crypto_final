#include "utils.hpp"

class LWE {
    int _calc_error_bound() {
        // Calculate the error bound based on N and Q
        return (int)floor(sqrt(
            (double)this->Q / (double)(4 * this->N)
        ));
    }

    // output size: (1, N)
    vector<vector<int>> _gen_bounded_key(int sz) {
        vector<vector<int>> result(sz, vector<int>(1, 0)); 
        for (int i = 0; i < sz; i++) {
            result[i][0] = uniform_distirbution(this->B[0], this->B[1]);
        }
        return result;
    }

    vector<vector<int>> _gen_A(int n, int q) {
        vector<vector<int>> A(n, vector<int>(n, 0));
        for (int i=0; i<n; i++)
            for (int j=0; j<n; j++)
                A[i][j] = uniform_distirbution(0, q);
        return A;
    }

public:
    int N; // Number of bits in secret key
    int Q; // Modulus
    int B[2]; // error bound
    vector<vector<int>> sk, e; // secret key
    vector<vector<int>> A, y; // public key
    
    LWE(int N, int Q) {
        srand(0xabcd); // Seed for reproducibility

        this->N = N;
        this->Q = Q;

        int b = this->_calc_error_bound();
        this->B[0] = -b; 
        this->B[1] = b+1;

        // secret key
        this->sk = this->_gen_bounded_key(this->N);
    }

    /**
     * given private key sk
     * computes y = A * sk + e
     * pubkey is (A, y)
     */
    void gen_public_key() {
        // generate uniform matx modulo q
        this->A = this->_gen_A(this->N, this->Q);
        // matmul
        vector<vector<int>> As(this->N, vector<int>(1, 0));
        matmul(this->A, this->sk, As);

        // generate error and pubkey y
        this->e = this->_gen_bounded_key(this->N);
        this->y = vector<vector<int>>(this->N, vector<int>(1, 0));
        for (int i=0; i<this->N; i++)
            this->y[i][0] = As[i][0] + e[i][0];

        print_matrix(this->y);
    }
};

int main(void) {
    int N = 4, Q = 256;

    LWE lwe(N, Q);
    lwe.gen_public_key();
    
    print_matrix(lwe.A);
    print_matrix(lwe.sk);
    print_matrix(lwe.e);

}