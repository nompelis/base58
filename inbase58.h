//
// Simple base58 encoding/decoding API for the masses; uses GNU MP (GMP)
// Copyright 2023, Ioannis Nompelis <nompelis@nobelware.com>
//

#ifndef _INBASE58_H_
#define _INBASE58_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gmp.h>

typedef unsigned char BYTE;


//
// base58 alphabet indexed by the value each symbol represents
// (for index "0" you get character "1", for index "57" you get character "z")
//
const char inbase58_alphabet[] =
   "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

//
// base58 alphabet reverse map
// (indexed by the ASCII value of symbol and mapping to the value it
// represents; the "-1" in unsigned char is shorthand for "0xff")
//
const BYTE inbase58_map[] = {
   -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
   -1,  0,  1,  2,  3,  4,  5,  6,   7,  8, -1, -1, -1, -1, -1, -1,
   -1,  9, 10, 11, 12, 13, 14, 15,  16, -1, 17, 18, 19, 20, 21, -1,
   22, 23, 24, 25, 26, 27, 28, 29,  30, 31, 32, -1, -1, -1, -1, -1,
   -1, 33, 34, 35, 36, 37, 38, 39,  40, 41, 42, 43, -1, 44, 45, 46,
   47, 48, 49, 50, 51, 52, 53, 54,  55, 56, 57, -1, -1, -1, -1, -1,
};

//
// base16 (hex) alphabet reverse map
// (indexed by the ASCII value of symbol and mapping to the value it
// represents; the "-1" in unsigned char is shorthand for "0xff"; this
// allows mixed case -- like Ethereum checksum style)
//
const BYTE inbase16_map[] = {
   -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
    0,  1,  2,  3,  4,  5,  6,  7,   8,  9, -1, -1, -1, -1, -1, -1,
   -1, 10, 11, 12, 13, 14, 15, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
   -1, 10, 11, 12, 13, 14, 15, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1
};

//
// GMP "high base" (higher than 37) digit transcription map
// (GMP will return its own digits; this maps to base58 digits)
//
const BYTE inbase58_gmp[] = {
   -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1,
    0,  1,  2,  3,  4,  5,  6,  7,   8,  9, -1, -1, -1, -1, -1, -1,
   -1, 10, 11, 12, 13, 14, 15, 16,  17, 18, 19, 20, 21, 22, 23, 24,
   25, 26, 27, 28, 29, 30, 31, 32,  33, 34, 35, -1, -1, -1, -1, -1,
   -1, 36, 37, 38, 39, 40, 41, 42,  43, 44, 45, 46, 47, 48, 49, 50,
   51, 52, 53, 54, 55, 56, 57, -1,  -1, -1, -1, -1, -1, -1, -1, -1
};


int inBase58_DecodeToHex( char** out_str,
                          const char b58enc[], int num_digit );

int inBase58_EncodeFromHex( char** out_str,
                            const char hexenc[], int num_digit );

#endif

