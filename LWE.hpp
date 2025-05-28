#include "utils.hpp"

/**
 * LWE main class
 */
class LWE {
public:
    int _calc_error_bound();

    // output size: (row_sz, col_sz)
    vvint _gen_bounded_key(int row_sz, int col_sz);

    vvint _gen_A(int n, int q);

    int N; // Number of bits in secret key
    int Q; // Modulus
    int B[2]; // error bound
    vvint sk, e; // secret key
    vvint A, y; // public key
    
    LWE(int N, int Q);

    /**
     * given private key sk
     * computes y = A * sk + e
     * pubkey is (A, y)
     */
    void gen_public_key();

    /** 
     * encrypt a bit
     * 
     * y: (N, 1)
     * r: (N, m)
     * :output: c1: (N, m) vector
     * :output: c2: (1, m)
     */ 
    void encrypt_bits(vector<bool> bits, vvint& c1, vvint& c2);

    /**
     * c1: (N, m) vector
     * c2: (1, m) vector
     * s: (N, 1) secret key
     * 0 if |c2 - st^T c1 mod q| < q/4
     */
    void decrypt_bits(vector<bool>& result, const vvint c1, const vvint c2);

    /**
     * signal function for key exchange
     * Sig(v) = v belongs in E?
     * where E = {-floor(q/4), ..., round(q/4)}
     */
    bool Sig_keyexc(int v);

    /**
     * RLWE Key Exchange
     * Alice: p_A = A * s_a + 2e_a (mod q)
     * send p_A to Bob
     * Bob: p_B = A^T * s_b + 2e_b (mod q)
     * 
     * :input: result exchanged info p(N*1), signal w
     * :input: public key A(N*N), Q
     */
    void gen_key_for_exchange(vvint &p, const vvint A, const int Q, bool is_Bob);

    /**
     * On receiving p_A from Alice, Bob computes:
     * Bob:   k_B = p_A^T * s_B + 2e_B' (mod q) (e'B is integer) 
     * Alice: k_A = s_A^T * p_B + 2e_A' (mod q) (e'A is integer)
     * :input: result integer k
     * :input: exchanged info p(N*1), public key A(N*N), Q, is_Bob
     */
    void compute_exchanged_key(int &k, const vvint p, const vvint A, const int Q, bool is_Bob);

    /**
     * key stream used in both initiator and responser
     * Mod2(v,w) = (v + w * (q-1)/2) mod q mod 2
     * where w = Sig(v)
     */
    int gen_exchange_keystream(int k);
};