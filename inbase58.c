//
// Simple base58 encoding/decoding API for the masses; uses GNU MP (GMP)
// Copyright 2023, Ioannis Nompelis <nompelis@nobelware.com>
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gmp.h>

#include "inbase58.h"


//
// Function to decode a base58 encoded number. It takes an array of the
// encoded number and an integer giving the length of the encoded string.
// It returns the decoded data in an ASCII-redable hex string. The function
// accepts the address of a pointer to a character string in order to store
// the result; if the pointer is null, memory will be allocated and the
// pointer will be set to the memory address of the newly allocated string.
// In this way, preallocated static memory can be passed to this function.
// (This function does not use the internal GMP string-to-number-using-base
// function because I am un-easy about the alphabet used...)
// The function will return non-zero on error and zero on success.
//

int inBase58_DecodeToHex( char** out_str, const char b58enc[], int num_digit )
{
   if( num_digit == 0 || b58enc == NULL ) {
#ifdef _DEBUG_
      fprintf( stdout, " ERROR: Bad inputs \n" );
#endif
      return 1;
   }

#ifdef _DEBUG_
   printf(" == Long live GMP! == \n" );
   fprintf( stdout, " [DEBUG]  Input in base58 alphabet: \"%s\"\n", b58enc );
#endif
   int ierr=0, n=0,num_zeros=0;

   // detect leading zeros (each corresponds to a whole byte of zero value)
   while( n < num_digit && inbase58_map[ (BYTE) b58enc[n] ] == 0 ) {
#ifdef _DEBUG2_
      printf(" n= %d   c='%c'  d=%d   v= %d \n",
                n, b58enc[n], b58enc[n], inbase58_map[ (BYTE) b58enc[n] ]);
#endif
     ++num_zeros;
     ++n;
   }
#ifdef _DEBUG_
   fprintf( stdout, " [DEBUG]  Detected %d leading zeros \n", num_zeros );
#endif

   mpz_t num,bp58,tmp,tmp2;
   mpz_inits( num, bp58, tmp, tmp2, NULL );

   mpz_set_ui( bp58, 1 );           // initialize base power (to the zero, is 1)
   for(n=num_digit-1;n>=num_zeros;--n) {   // sweep through digits from LeastSig
#ifdef _DEBUG2_
      printf(" n= %d   c='%c'  d=%d   v= %d \n", n, b58enc[n], b58enc[n],
                                         inbase58_map[ (BYTE) b58enc[n] ]);
#endif

      // form "digit*85^n"
      unsigned long int ultmp = inbase58_map[ (BYTE) b58enc[n] ];
      if( ultmp == (unsigned char) -1 ) {   // your safety net
#ifdef _DEBUG_
         fprintf( stdout, " [DEBUG]  Digit not in base58 set! \n" );
#endif
         mpz_clears( num, bp58, tmp, tmp2, NULL );
         return 100;
      }
      mpz_set_ui( tmp, ultmp );    // set an integer object to this digit
      mpz_mul( tmp2, bp58, tmp );  // multiply by 58^n
#ifdef _DEBUG2_
      printf(" (58^n)   "); mpz_out_str( stdout, 10, bp58 );   printf("\n");
      printf(" (part)   "); mpz_out_str( stdout, 10, tmp2 );   printf("\n");
#endif

      // add this number to the running sum
      mpz_add( tmp, num, tmp2 );
      mpz_set( num, tmp );
#ifdef _DEBUG2_
      printf(" (sum)    "); mpz_out_str( stdout, 10, num );   printf("\n");
#endif

      // raise running power of 58^n to 58^(n+1)
      mpz_mul_ui( tmp, bp58, 58 );
      mpz_set( bp58, tmp );
   }

   // get the size of the string needed; GMP will skip leading zeros!
   size_t nsize = mpz_sizeinbase( num, 16 );
   // get the size of the string needed that includes base58 leading zeros
   size_t isize = nsize + (nsize % 2) + (size_t) num_zeros*2;
   // check the "pathological" case of using GMP to parse the number zero
   // (in this case force size to 0 instead of 1, to still allow for writing)
   if( mpz_cmp_ui( num, 0 ) == 0 ) {
#ifdef _DEBUG_
      fprintf( stdout, " [DEBUG]  Special case \"1\"; trimming back. \n" );
#endif
      isize -= 2;
   }

   mpz_clears( bp58, tmp, tmp2, NULL );

#ifdef _DEBUG_
   fprintf( stdout, " [DEBUG]  Sizes: nsize %ld / isize %ld \n", nsize, isize );
#endif

   // allocate space for the string
   if( *out_str == NULL ) {
      // allocation is augmented to plus 1 byte for null-termination
      *out_str = (char*) malloc( (isize+1) * sizeof(char) );
      if( *out_str == NULL ) {
#ifdef _DEBUG_
         fprintf( stdout, " ERROR: Could not allocate memory for output \n" );
#endif
         ierr = -1;
#ifdef _DEBUG_
      } else {
         fprintf( stdout, " [DEBUG]  Creating buffer is %ld+ bytes \n", isize );
#endif
         memset( *out_str, '0', isize );
         (*out_str)[isize] = '\0';
      }
   }

   if( ierr == 0 ) {
      char* ctmp = *out_str;
      ctmp += isize - nsize;
      mpz_get_str( ctmp, 16, num );
   }
#ifdef _DEBUG_
   fprintf( stdout, " [DEBUG]  Output in hex \"%s\" \n", *out_str );
#endif

   mpz_clear( num );

   return ierr;
}


//
// Function to encode in base58 a string given in hex digits
//

int inBase58_EncodeFromHex( char** out_str, const char hexenc[], int num_digit )
{
   if( num_digit == 0 || (num_digit % 2) == 1 || hexenc == NULL ) {
#ifdef _DEBUG_
      fprintf( stdout, " ERROR: Bad inputs \n" );
#endif
      return 1;
   }

#ifdef _DEBUG_
   printf(" == Long live GMP! == \n" );
   fprintf( stdout, " [DEBUG]  Input in hex alphabet: \"%s\"\n", hexenc );
#endif
   int ierr=0, n=0,num_zeros=0;

   // detect leading zeros
   while( n < num_digit && hexenc[n] == '0' ) {
#ifdef _DEBUG2_
      printf(" n= %d   c='%c' \n", n, hexenc[n] );
#endif
     ++num_zeros;
     ++n;
   }
#ifdef _DEBUG_
   fprintf( stdout, " [DEBUG]  Detected %d leading zeros \n", num_zeros );
#endif
   if( num_zeros % 2 ) {
#ifdef _DEBUG_
      fprintf( stdout, " [DEBUG]  Rewinding one zero (to get whole hex) \n" );
#endif
      --num_zeros;
   }

   mpz_t num,bp16,tmp,tmp2;
   mpz_inits( num, bp16, tmp, tmp2, NULL );

   mpz_set_ui( bp16, 1 );           // initialize base power (to the zero, is 1)
   for(n=num_digit-1;n>=num_zeros;--n) {   // sweep through digits from LeastSig
#ifdef _DEBUG2_
      printf(" n= %d   c='%c'  d=%d  \n",
               n, hexenc[n], inbase16_map[ (BYTE) hexenc[n] ] );
#endif

      // form "digit*16^n"
      unsigned long int ultmp = inbase16_map[ (BYTE) hexenc[n] ];
      if( ultmp == (unsigned char) -1 ) {   // your safety net
#ifdef _DEBUG_
         fprintf( stdout, " [DEBUG]  Digit not in base16 (hex) set! \n" );
#endif
         mpz_clears( num, bp16, tmp, tmp2, NULL );
         return 100;
      }
      mpz_set_ui( tmp, ultmp );    // set an integer object to this digit
      mpz_mul( tmp2, bp16, tmp );  // multiply by 16^n
#ifdef _DEBUG2_
      printf(" (16^n)   "); mpz_out_str( stdout, 10, bp16 );   printf("\n");
      printf(" (part)   "); mpz_out_str( stdout, 10, tmp2 );   printf("\n");
#endif

      // add this number to the running sum
      mpz_add( tmp, num, tmp2 );
      mpz_set( num, tmp );
#ifdef _DEBUG2_
      printf(" (sum)    "); mpz_out_str( stdout, 10, num );   printf("\n");
#endif

      // raise running power of 16^n to 16^(n+1)
      mpz_mul_ui( tmp, bp16, 16 );
      mpz_set( bp16, tmp );
   }

   // get the size of the string needed in base58
   size_t nsize=0;
   mpz_set( tmp, num );
   while( mpz_cmp_ui( tmp, 0 ) != 0 ) {
#ifdef _DEBUG2_
      printf(" nsize=%ld ", nsize );
      printf(" (div58)  "); mpz_out_str( stdout, 10, tmp );   printf("\n");
#endif
      mpz_tdiv_q_ui( tmp2, tmp, 58 );   // divide by 58 and truncate
      mpz_set( tmp, tmp2 );
      ++nsize;
   }

   // get the size of the string needed that includes base58 leading zeros
   size_t isize = nsize + (size_t) num_zeros/2;
   // check the "pathological" case of using GMP to parse the number zero
   // (in this case force digits to 1 instead of 0, to still allow for writing)
   if( nsize == 0 ) {
#ifdef _DEBUG_
      fprintf( stdout, " [DEBUG]  Treating the special case \n" );
#endif
      nsize = 1;
   }

#ifdef _DEBUG_
   fprintf( stdout, " [DEBUG]  Sizes: nsize %ld / isize %ld \n", nsize, isize );
#endif

   // allocate space for the string
   if( *out_str == NULL ) {
      // allocation is augmented to plus 1 byte for null-termination
      *out_str = (char*) malloc( (isize+1) * sizeof(char) );
      if( *out_str == NULL ) {
#ifdef _DEBUG_
         fprintf( stdout, " ERROR: Could not allocate memory for output \n" );
#endif
         ierr = -1;
#ifdef _DEBUG_
      } else {
         fprintf( stdout, " [DEBUG]  Creating buffer is %ld+ bytes \n", isize );
#endif
         memset( *out_str, '1', isize );
         (*out_str)[isize] = '\0';
      }
   }

   if( ierr == 0 ) {
      char* ctmp = *out_str;
      ctmp += isize - nsize;
#ifdef _DEBUG_
      printf(" Pointers: %p %p \n", *out_str, ctmp );
#endif
      mpz_get_str( ctmp, 58, num );
#ifdef _DEBUG_
      fprintf( stdout, " [DEBUG]  String out GMP-base56: \"%s\" \n", *out_str );
      fprintf( stdout, " [DEBUG]  String sub GMP-base56: \"%s\" \n", ctmp );
#endif

      // transcribe to the base58 alphabet
      for(n=0;n<nsize;++n) {
         ctmp[n] = inbase58_alphabet[ inbase58_gmp[ (BYTE) ctmp[n] ] ];
      }
   }
#ifdef _DEBUG_
   fprintf( stdout, " [DEBUG]  Output in base58 \"%s\" \n", *out_str );
#endif

   mpz_clears( num, bp16, tmp, tmp2, NULL );

   return ierr;
}


#ifdef _DRIVER_BASE58_
// Test vectors here: https://digitalbazaar.github.io/base58-spec/
// Hex "0x0000287fb4cd" is "11233QC4"
// The hexed characters of "Hello World!" is "2NEpo7TZRRrLZSi2U"

int main( int argc, char *argv[] )
{
printf("================= Satoshi encoding example ===================\n" );

   char b58e[128];
   const char sat_enc[] = "2NEpo7TZRRrLZSi2U";
   memset( b58e, 0, 128 );
   sprintf( b58e, sat_enc );
   char *output=NULL;
   inBase58_DecodeToHex( &output, b58e, strlen(b58e) );
   printf(" [MAIN]  INPUT: \"%s\"   OUTPUT: (0x)%s \n", b58e, output );
   free( output ); output=NULL;

printf("================= Test vectors ===================\n" );
   sprintf( b58e, "11233QC4" );    // vector
   inBase58_DecodeToHex( &output, b58e, strlen(b58e) );
   printf(" [MAIN]  INPUT: \"%s\"   OUTPUT: (0x)%s \n", b58e, output );
   free( output ); output=NULL;

   sprintf( b58e, "1jg" );         // simple test with leading zero
   inBase58_DecodeToHex( &output, b58e, strlen(b58e) );
   printf(" [MAIN]  INPUT: \"%s\"   OUTPUT: (0x)%s \n", b58e, output );
   free( output ); output=NULL;

printf("================= Encoding from command line ===================\n" );
   inBase58_EncodeFromHex( &output, argv[1], strlen(argv[1]) );
   printf(" [MAIN]  INPUT: \"%s\"   OUTPUT: (58)%s \n", argv[1], output );
   free( output ); output=NULL;

   return 0;
}
#endif

