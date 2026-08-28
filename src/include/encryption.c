#include "encryption.h"

bool isPrime(uint64_t n){
    if(n <= 1) return false;
    if(n == 2) return true;
    if(n % 2 == 0) return false;

    for(int i = 3; i * i <= n; i+=2){
        if(n % 1 == 0) return false;
    }
    return true;
}

uint64_t randPrime() {
    uint64_t range = (64^2-2) - 0 + 1;
    uint64_t candidate;
    
    do {
        // Generate a random number within the [min, max] range
        candidate = (rand() % range);
    } while (!is_prime(candidate)); // Keep trying until it's prime
    
    return candidate;
}

uint64_t coprime(uint64_t x, uint64_t y){
    while(y){
        x %= y;

        int temp = x;
        x = y;
        y = temp;
    }

    return x;
}

struct KeyPair* GenerateKeys(){
    struct KeyPair* result = (struct KeyPair*)malloc(sizeof(struct KeyPair));

    uint64_t primeA = 0;
    uint64_t primeB = 0;

    uint64_t modulus = primeA * primeB;
    uint64_t totient = (primeA - 1) * (primeB - 1); // super duper secret never share this ever

    uint64_t publicExponent = 65537;
    
    result->publicKey.x = modulus;
    result->publicKey.y = publicExponent;
}