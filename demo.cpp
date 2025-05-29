#include "utils.hpp"
#include "LWE.hpp"

/**
 * show LWE key exchange process
 */
void show_key_exchange(int N, int Q) {
    
    // A, N, Q is public
    LWE lwe_Alice(N, Q);
    lwe_Alice.gen_public_key(NULL);
    vvint A(N, vint(N, 0));
    A = lwe_Alice.A;
    
    LWE lwe_Bob(N, Q);
    lwe_Bob._gen_A(N, Q);
    lwe_Bob.A = A;

    // Alice generates exchange info p_A
    vvint p_Alice;
    lwe_Alice.gen_key_for_exchange(p_Alice, A, Q, false);
    // printf("Alice's exchange info p_A:\n");
    // print_matrix(p_Alice);

    // Bob generates exchange info p_B
    vvint p_Bob;
    lwe_Bob.gen_key_for_exchange(p_Bob, A, Q, true);
    // printf("Bob's exchange info p_B:\n");
    // print_matrix(p_Bob);

    // Bob computes k_B on receiving p_A
    int k_Bob = 0;
    lwe_Bob.compute_exchanged_key(k_Bob, p_Alice, A, Q, true);
    printf("Bob's exchanged key k_B: %d\n", k_Bob);

    // Alice computes k_A on receiving p_B
    int k_Alice = 0;
    lwe_Alice.compute_exchanged_key(k_Alice, p_Bob, A, Q, false);
    printf("Alice's exchanged key k_A: %d\n", k_Alice);
    puts("");

    // generate key stream (one-byte for now)
    int key_Alice = lwe_Alice.gen_exchange_keystream(k_Alice);
    printf("Alice's keystream: %d\n", key_Alice);
    
    int key_Bob = lwe_Bob.gen_exchange_keystream(k_Bob);
    printf("Bob's keystream: %d\n", key_Bob);
    puts("");
}

/**
 * demonstrate LWE encrypt/deccrypt
 */
void show_encrypt_and_decrypt(int N, int Q, int B, FILE *store_fp) {
    int m = 500;
    vector<bool> bits; 
    get_rand_vector(bits, m);

    LWE lwe(N, Q);
    // printf("B: [%d, %d]\n", lwe.B[0], lwe.B[1]);
    lwe._set_error_bound(B);
    lwe.gen_public_key(NULL);

    vvint c1(N, vint(m, 0));
    vvint c2(1, vint(m, 0));

    int n_cnt = 0;
    vector<bool> dec_bits(m);
    
    lwe.encrypt_bits(bits, c1, c2);
    lwe.decrypt_bits(dec_bits, c1, c2);
    for (int j=0; j<m; j++) {
        n_cnt += (dec_bits[j] == bits[j]);
    }
    
    int success_rate = (double)n_cnt / (double)m * 100.0;
    // printf("success rate: %d/%d = %.2f%%\n", n_cnt, n_try, success_rate);

    // if provided fp, save to it
    if (!store_fp) return;

    int f = fprintf(store_fp, "%d %d %d %d\n", N, Q, B, success_rate);
    if (f < 0) ERR_EXIT("fprintf");
}

/**
 * showcase the CVP explotation on sparse A
 * 
 * we have many sol: 
 * (s', e') = (s + u_i, e - Au_i)
 * 
 * basis:
 * ( A I)
 * (qI 0)
 * 
 * target: (t, 0)
 */
void show_sparse_attack(int Q) {
    int N = 4;
    int m = 50;
    // define a sparse matrix
    vvint A{
        vint{1, 0, 1, 0},
        vint{0, 1, 1, 1},
        vint{1, 0, 0, 1},
        vint{0, 1, 0, 0}
    };

    LWE lwe(N, Q); 
    lwe.gen_public_key(&A);
    // given A, t
    puts("A:");
    print_matrix(lwe.A);
    puts("t:");
    print_matrix(lwe.y);

    // [[1 0 -1 0 1 0 0 0][0 -1 1 1 0 1 0 0][-1 0 0 1 0 0 1 0][0 -1 0 0 0 0 0 1][1073 ]]
    // create basis
    print_CVP_inputs(lwe.A, lwe.Q, lwe.y);

    /**
     * [s'e'] = [-2 5 -2 -2 | -1 -3 1 -2] = [7 2 7 2] + u_i, [-3 -1 3 0] - A * u_i
     */
    puts("");
    print_matrix(lwe.e); // [-3 -1 3 0]
    puts("");
    print_matrix(lwe.sk); // [7 2 7 2]

    // CVP - (-e, s) = (t, 0)
    // verify (-e, s):  [-2 5 -2 -2 -1 -3 1 -2] - [-3 6 -2 -2 0 0 0 0 ]
    vvint cvp_s{vint{-6}, vint{-6}, vint{-8}, vint{-4}};
    vvint cvp_e{vint{5}, vint{3}, vint{1}, vint{3}};

    vvint As(N, vint(1, 0));
    vvint cvp_y(N, vint(1, 0));
    matmul(A, cvp_s, As);
    for (int i=0; i<N; i++)
        cvp_y[i][0] = (As[i][0] + cvp_e[i][0]) % lwe.Q;
    puts("face your sin");
    print_matrix(cvp_y);

    vector<bool> bits; 
    get_rand_vector(bits, m);

    // encrypt
    vvint c1(N, vint(m, 0));
    vvint c2(1, vint(m, 0));
    lwe.encrypt_bits(bits, c1, c2);
}

/** the main function */
int main(int argc, const char* argv[]) {
    if (argc != 4) {
        puts("usage: ./lwe <N> <Q> <B>");
        return -1;
    }
    
    int N = atoi(argv[1]), Q = atoi(argv[2]);
    int B = atoi(argv[3]); // customized error bound

    printf("[demo] using parameters: N=%d, Q=%d, B=%d\n", N, Q, B);

    // save result to txt
    // FILE *fp = fopen("test.txt", "w+");
    // if (!fp) ERR_EXIT("open");

    // for (int dB=0; dB<Q; dB++)
    //     show_encrypt_and_decrypt(N, Q, dB, fp);
    
    // fclose(fp);
    // show_key_exchange(N, Q);

    show_sparse_attack(Q);
}