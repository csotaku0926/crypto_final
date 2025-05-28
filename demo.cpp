#include "utils.hpp"
#include "LWE.hpp"

/**
 * show LWE key exchange process
 */
void show_key_exchange(int N, int Q) {
    
    // A, N, Q is public
    LWE lwe_Alice(N, Q);
    lwe_Alice.gen_public_key();
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
    printf("Alice's keystream: %d\n");
    
    int key_Bob = lwe_Bob.gen_exchange_keystream(k_Bob);
    printf("Bob's keystream: %d\n");
    puts("");
}

/**
 * demonstrate LWE encrypt/deccrypt
 */
void show_encrypt_and_decrypt(int N, int Q) {
    vector<bool> bits{1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1};
    int m = bits.size();

    LWE lwe(N, Q);
    printf("B: [%d, %d]\n", lwe.B[0], lwe.B[1]);
    lwe.gen_public_key();

    vvint c1(N, vint(m, 0));
    vvint c2(1, vint(m, 0));

    int n_cnt = 0;
    int n_try = 1000;
    vector<bool> dec_bits(m);
    
    for (int i=0; i<n_try; i++) {
        bits[0] = i % 2;
        lwe.encrypt_bits(bits, c1, c2);
        lwe.decrypt_bits(dec_bits, c1, c2);
        for (int j=0; j<m; j++) {
            n_cnt += (dec_bits[j] == bits[j]);
        }
    }

    n_try *= m;
    printf("success rate: %d/%d = %.2f%%\n", n_cnt, n_try, (double)n_cnt / (double)n_try * 100.0);

}

/** the main function */
int main(void) {
    int N = 23, Q = 371;

    show_encrypt_and_decrypt(N, Q);
    show_key_exchange(N, Q);

    int fd = open("test.txt", O_CREAT);
}