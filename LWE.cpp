#include "utils.hpp"
#include "LWE.hpp"


int LWE::_calc_error_bound() {
    // Calculate the error bound based on N and Q
    return (int)floor(sqrt(
        (double)this->Q / (double)(4 * this->N)
    ));
}

// output size: (row_sz, col_sz)
vvint LWE::_gen_bounded_key(int row_sz, int col_sz) {
    vvint result(row_sz, vint(col_sz, 0)); 
    for (int i = 0; i < row_sz; i++) {
        for (int j = 0; j < col_sz; j++) {
            result[i][j] = uniform_distirbution(this->B[0], this->B[1]);
        }
    }
    return result;
}

vvint LWE::_gen_A(int n, int q) {
    vvint A(n, vint(n, 0));
    for (int i=0; i<n; i++)
        for (int j=0; j<n; j++)
            A[i][j] = uniform_distirbution(0, q);
    return A;
}

void LWE::_set_error_bound(int b) {
    this->B[0] = -b;
    this->B[1] = b+1;
}

LWE::LWE(int N, int Q) {
    srand(0xabcddcba); // Seed for reproducibility

    this->N = N;
    this->Q = Q;

    int b = this->_calc_error_bound();
    this->_set_error_bound(b);

    // gen secret key
    this->sk = this->_gen_bounded_key(this->N, 1);
}

void LWE::gen_public_key(vvint *A_user) {
    // generate uniform matx modulo q (if not specified by user)
    this->A = (A_user) ? *A_user : this->_gen_A(this->N, this->Q);
    vvint sk(this->N, vint(1, 0));

    vvint As(this->N, vint(1, 0));
    matmul(this->A, this->sk, As);
    
    // generate error and pubkey y
    this->e = vvint(this->N, vint(1, 0));
    this->e = this->_gen_bounded_key(N, 1);

    this->y = vvint(this->N, vint(1, 0));
    for (int i=0; i<this->N; i++)
        this->y[i][0] = (As[i][0] + e[i][0]) % this->Q;
}


void LWE::encrypt_bits(vector<bool> bits, vvint& c1, vvint& c2) {
    int m = bits.size();
    int N = this->N;
    // zero init c1, c2
    c1 = vvint(N, vint(m, 0));
    c2 = vvint(1, vint(m, 0));
    // generate random vector r
    vvint r = this->_gen_bounded_key(N, m);
    vvint z = this->_gen_bounded_key(N, m);
    vvint z_prime = this->_gen_bounded_key(1, m);
    // Ar = A^T @ r
    vvint At(N, vector<int>(N, 0));
    transpose(this->A, At); // -> (m, N)
    vvint Ar(N, vector<int>(m, 0));
    matmul(At, r, Ar); // -> (N, m)

    // c1 = Ar + z --> (N, m)
    for (int i=0; i<N; i++)
        for (int j=0; j<m; j++)
            c1[i][j] += (Ar[i][j] + z[i][j]) % this->Q;

    // c2 = y @ r + z'+ m * Q//2 --> (1, m)
    vvint y_T(1, vint(N, 0)); // (1, N)
    transpose(this->y, y_T);

    vvint rY(1, vint(m, 0));
    matmul(y_T, r, rY); // (1, M)
    for (int i=0; i<m; i++) {
        c2[0][i] = rY[0][i] + z_prime[0][i] + bits[i] * (this->Q / 2);
        c2[0][i] %= this->Q;
    }
} 


void LWE::decrypt_bits(vector<bool>& result, const vvint c1, const vvint c2) {
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

int LWE::gen_exchange_keystream(int k) {
    int w = this->Sig_keyexc(k);
    int tmp = k + w * (this->Q - 1) / 2;
    // tmp mod q
    tmp = (tmp % this->Q);
    tmp = (tmp < 0) ? tmp + this->Q : tmp;
    return tmp & 1; // mod 2
}

bool LWE::Sig_keyexc(int v) {
    int low = -this->Q / 4;
    int high = (int)round((double)this->Q / 4.0);
    return (low <= v && v <= high);
}

/**
 * note: generates new secret key and error
 */
void LWE::gen_key_for_exchange(vvint &p, const vvint A, const int Q, bool is_Bob) {
    const int N = A.size();    
    vvint As(N, vint(1, 0));
    if (!is_Bob) matmul(A, this->sk, As);
    else matmul(transpose(A), this->sk, As); // Bob uses A^T instead
    
    vvint e = this->_gen_bounded_key(N, 1);

    p = vvint(N, vint(1, 0));
    for (int i=0; i<N; i++) {
        // p = A * s + 2 * e
        p[i][0] = (As[i][0] + 2 * e[i][0]) % Q;
        p[i][0] = (p[i][0] < 0) ? (p[i][0] + Q) : p[i][0]; // ensure positive
    }
}

void LWE::compute_exchanged_key(int &k, const vvint p, const vvint A, const int Q, bool is_Bob) {
    const int N = A.size();
    // k = p * s_B + 2e_B (mod q)
    vvint sp_product(1, vint(1, 0));
    if (is_Bob) matmul(transpose(p), this->sk, sp_product);
    else matmul(transpose(this->sk), p, sp_product); // Alice uses s_A^T first

    // generate small error
    vvint e = this->_gen_bounded_key(1, 1);
    k = (sp_product[0][0] ) % Q + 2 * e[0][0];
    k = (k < 0) ? (k + Q) : k; // ensure positive
}
