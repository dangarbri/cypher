# Cypher

Cypher is your encryption cracking toolkit.
The provided functionality is based on the cryptography deficiencies described in the [cryptopals challenges](https://www.cryptopals.com/).
The toolkit is designed to perform the attacks in a semi-generic way.
For example for a padding oracle attack, the algorithm is the same, but the oracle may change, so facilities are provided for you to provide your own oracle while the toolkit implements the overall attack algorithm.

## Technical Details

I've chosen to write this in pure C, specifically C17.
I chose C due to the lack of nearly all overhead besides the standard library.
I was originally going to use rust until I saw that it took 12+ function calls to call `println!`.
Encryption is very computationally heavy, so I want to reduce the overhead as much as I can.

I want this library to be portable and efficient, so I try to use only standard library functions and no platform specific extensions.
Unfortunately I'm not brave enough or knowledgable enough to use `-nostdlib` like busybox, which is portable enough to run on the air we breathe, but I assume using only the standard library should allow it to run at least least Mac, Linux, and Windows, or any other platform that has a C17 compiler.

I chose cmake as the build system. I considered autotools, but while autotools aims to be portable, it's generally not supported by windows due to the required bash scripts. I know there are projects like mingw and cygwin which aim to provide UNIX-like functionality for windows, but cmake is much less of a hassle to set up, and I know it runs on all platforms with relatively little setup required.
