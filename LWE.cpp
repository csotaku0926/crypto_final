#include "utils.hpp"

typedef unsigned int uint;
typedef vector<int> vint;
typedef vector<vector<int>> vvint;

class LWE {
public:
    int _calc_error_bound() {
        // Calculate the error bound based on N and Q
        return (int)floor(sqrt(
            (double)this->Q / (double)(4 * this->N)
        ));
    }

    // output size: (1, N)
    vvint _gen_bounded_key(int row_sz, int col_sz) {
        vvint result(row_sz, vint(col_sz, 0)); 
        for (int i = 0; i < row_sz; i++) {
            for (int j = 0; j < col_sz; j++) {
                result[i][j] = uniform_distirbution(this->B[0], this->B[1]);
            }
        }
        return result;
    }

    vvint _gen_A(int n, int q) {
        vvint A(n, vint(n, 0));
        for (int i=0; i<n; i++)
            for (int j=0; j<n; j++)
                A[i][j] = uniform_distirbution(0, q);
        return A;
    }

    int N; // Number of bits in secret key
    int Q; // Modulus
    int B[2]; // error bound
    vvint sk, e; // secret key
    vvint A, y; // public key
    
    LWE(int N, int Q) {
        srand(0xabcddcba); // Seed for reproducibility

        this->N = N;
        this->Q = Q;

        int b = this->_calc_error_bound();
        this->B[0] = -b; 
        this->B[1] = b+1;

        // gen secret key
        this->sk = this->_gen_bounded_key(this->N, 1);
    }

    /**
     * given private key sk
     * computes y = A * sk + e
     * pubkey is (A, y)
     */
    void gen_public_key() {
        // generate uniform matx modulo q
        this->A = this->_gen_A(this->N, this->Q);
        A[0] = vint{101, 173, 27};
        A[1] = vint{192, 121, 7};
        A[2] = vint{116, 223, 30};
        print_matrix(this->A);
        puts("");

        vvint sk(3, vint(1, 0));
        sk[0][0] = 2;
        sk[1][0] = this->Q + -2;
        sk[2][0] = 1;
        this->sk = sk;

        // matmul
        vvint As(this->N, vint(1, 0));
        matmul(this->A, this->sk, As);
        
        // generate error and pubkey y
        // this->e = this->_gen_bounded_key(this->N, 1);
        vvint e(3, vint(1, 0));
        e[0][0] = 0;
        e[1][0] = -2 + this->Q;
        e[2][0] = 1;
        this->e = e;

        this->y = vvint(this->N, vint(1, 0));
        for (int i=0; i<this->N; i++)
            this->y[i][0] = (As[i][0] + e[i][0]) % this->Q;
         
        print_matrix(this->y);
        puts("");
    }

    /** 
     * encrypt a bit
     * 
     * y: (N, 1)
     * r: (N, m)
     * :output: c1: (N, m) vector
     * :output: c2: (1, m)
     */ 
    void encrypt_bits(vector<bool> bits, vvint& c1, vvint& c2) {
        int m = bits.size();
        int N = this->N;
        // zero init c1, c2
        c1 = vvint(N, vint(m, 0));
        c2 = vvint(1, vint(m, 0));
        // generate random vector r
        vvint r = this->_gen_bounded_key(N, m);
        // vvint r(3, vint(1, 0));
        // r[0][0] = 2;
        // r[1][0] = this->Q + -2;
        // r[2][0] = this->Q + -1;

        vvint z = this->_gen_bounded_key(N, m);
        // vvint z(3, vint(1, 0));
        // z[0][0] = 0;
        // z[1][0] = 1;
        // z[2][0] = -2 + this->Q;

        vvint z_prime = this->_gen_bounded_key(1, m);
        // vvint z_prime(1, vint(1, 0));
        // z[0][0] = -2 + this->Q;

        // Ar = A^T @ r
        vvint At(N, vector<int>(N, 0));
        transpose(this->A, At); // -> (m, N)
        vvint Ar(N, vector<int>(m, 0));
        matmul(At, r, Ar); // -> (N, m)

        // c1 = Ar + z --> (N, m)
        for (int i=0; i<N; i++)
            for (int j=0; j<m; j++)
                c1[i][j] += (Ar[i][j] + z[i][j]) % this->Q;
        
        // print_matrix(c1); puts("");

        // c2 = y @ r + z'+ m * Q//2 --> (1, m)
        vvint y_T(1, vint(N, 0)); // (1, N)
        transpose(this->y, y_T);

        vvint rY(1, vint(m, 0));
        matmul(y_T, r, rY); // (1, M)
        for (int i=0; i<m; i++) {
            c2[0][i] = rY[0][i] + z_prime[0][i] + bits[i] * (this->Q / 2);
            c2[0][i] %= this->Q;
        }
        // print_matrix(c2); puts("");
    } 

    /**
     * c1: (N, m) vector
     * c2: (1, m) vector
     * s: (N, 1) secret key
     * 0 if |c2 - st^T c1 mod q| < q/4
     */
    void decrypt_bits(vector<bool>& result, const vvint c1, const vvint c2) {
        int m = c1[0].size();
        int N = this->N;
        vvint sk_T(1, vint(N, 0));
        transpose(this->sk, sk_T); // -> (1, N)

        vvint sA(1, vint(m, 0));
        matmul(sk_T, c1, sA); // -> (1, m)
        
        // decrypt bit-by-bit
        result = vector<bool>(m, 0);
        for (int i=0; i<m; i++) {
            int delta = (c2[0][i] - sA[0][i]) % this->Q;
            delta = (delta < 0) ? (delta + this->Q) : delta; // ensure positive
            // printf("delta: %d\n", delta);
            result[i] = (3 * this->Q / 4 >= delta && delta >= this->Q / 4) ? true : false;
        }
    } 
};

int main(void) {
    int N = 3, Q = 229;
    vector<bool> bits{1}; //, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1};
    int m = bits.size();

    LWE lwe(N, Q);
    printf("B: [%d, %d]\n", lwe.B[0], lwe.B[1]);
    lwe.gen_public_key();

    vvint c1(N, vint(m, 0));
    vvint c2(1, vint(m, 0));

    int n_cnt = 0;
    int n_try = 5000;
    vector<bool> dec_bits(m);
    
    for (int i=0; i<n_try; i++) {
        bits[0] = i % 2;
        print_vector(bits);
        lwe.encrypt_bits(bits, c1, c2);
        lwe.decrypt_bits(dec_bits, c1, c2);
        for (bool bit : dec_bits) {
            // printf("%d ", bit);
            n_cnt += (bit == bits[0]);
        }
    }


    printf("success rate: %d/%d = %.2f%%\n", n_cnt, n_try, (double)n_cnt / (double)n_try * 100.0);
}