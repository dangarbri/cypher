# Oracles

These so-called oracles are simple programs which are meant to simulate vulnerable applications.

They are here to demonstrate how cipher can be used to test the security of other applications.

## Encryption Oracle

`encryption_oracle.c` is a program which encrypts data using a randomly generated key (the data is not meant to be decrypted) and chooses to encrypt either in ECB or CBC mode.

This demonstrates that the simple act of encrypting a user's data can leak information about the encryption mechanism.