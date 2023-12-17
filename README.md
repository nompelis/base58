# inbase58 v0.0

 A simple Base58 encoder/decoder that uses the GNU Multiple Precision (GMP)
 library for transcription across bases. If you find issues, let me know.

 TL;DR
 - I could not find one, other than Satoshi's, in plain C.
 - Simple is instructive, and there are Satoshi's conventions to deal with.
 - Uses the GNU Multiple Precision library; otherwise I it would be Satoshi's.

------------------------------------------------------------------------------

 If you are in the "I have questions..." camp, this is for you. This is my
 brief explanation for why Satoshi created the base58 format with leading
 zeros. And this code helps you understand transcription between the hex
 and the 58 bases, and also how to understand and treat the peculiarity
 of "leading zeros".

 First, the implied things, of working with numbers inside the computer...
 When we look at hexadeciaml representations of numbers, we can have an odd
 number of digits, and that is fine. However, when dealing with computers (in
 which 8 bits form a byte, corresponding to numbers in [0-255]), we just deal
 with leading zeros. For example, we cannot have the number 5, but the number
 5 that is written as "0000 0101". And we live with it. And so, in hex, it is
 the number 5 written as "05". Somewhere in there it is implied that we will
 work with pairs of the hex digits, from the set [0,1,2,..d,e,f]. And this is
 all fine.

 Now, Satoshi shows up and wants to make people not mistype their keys, so he
 comes up with the ingenious "base58" format. He knows that those leading
 zeros are important, and he wants to write down numbers with leading zero
 retention. Why? Because if you were to generate (at random) a Bitcoin key,
 you would be building a number just shy of 2^255 (the upper limit is just
 below the number that is "the prime order of the group" that the elliptic
 curve operations using the parameters for curve secp256k1 define). This
 number has to be stored with whatever leading zeros you encountered by the
 randomness, and you would have to pass this to a hashing function as a
 "256-bit big endian number". That is, if you came up with the number 437,
 which can be stored in 2 bytes (16 bits), and you attempted to hash that,
 you would be entering into the hash function a stream of data that has
 your number stored in the first couple of bytes, when the number should
 really occupy the last two bytes (big endian format of 256 bits, or 32
 bytes). To make this requirement somewhat explicit, Satoshi's format
 requires that the leading zeros appear. Now, he does not use a zero "0"
 or a capitalized "O" in his alphabet of base58 digits, because of potential
 errors in transcription. So he shifted it all and used "1" (the character)
 to signify "zero in base58" everywhere in the number. HOWEVER, there is also
 the special case of it leading the string that represents the number. There,
 it significes a whole byte of zeros for each '1' digit encountered. If you
 are confused, write down a small example while re-reading this paragraph.

 We have to go back and forth between bytes and his base58 numbers. How do
 we do this? I am fine converting back and forth across different bases. Also,
 Satoshi (and friends) have come up with an algorithm that does this, and
 does so quickly (it also can have "square complexity", which is not a real
 problem for dealing with Bitcoin keys). But we will not do this... we will
 let the GMP handle all the integer arithmetic on different bases. Why? If
 you are dealing with big (integer) numbers in cryptography in your code,
 you likely have to use it, and you probably should. All we need to do in
 our software is handle the sizing of the digits during conversion, dealing
 with one single pathological case along the way, but keeping it all intuitive. 
 That is what I did. Try it. If it works, use it. If you find errors or issues,
 fix it and let me know.
 
 Encoder/decoders that I have found:
 http:lenschulwitz.com/base58
 https:appdevtools.com/base58-encoder-decoder

 IN 2023/12/16 <nompelis@nobelware.com>

