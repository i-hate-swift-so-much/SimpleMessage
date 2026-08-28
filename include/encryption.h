#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/random.h>
#include <stdbool.h>
#include "gmp.h"

struct PublicKey{
    uint64_t x;
    uint64_t y;
};

struct KeyPair{
    struct PublicKey publicKey;
    uint64_t privateKey;
};

struct KeyPair* GenerateKeys();