# Security and Whatnot
This document describes simplified vault structure, some of the more important secure considerations, password security and... philosophy :)

## The Vault Structure
1. User sets container passwords, which are immediately hashed using password hashing algorithm `Argon2id`
2. User adds passwords, files and other strings, which are put together in a YAML document
3. The document is encrypted using AEAD algorithm `XSalsa20-Poly1305`
4. Encryption step is repeated for each password on cipher with metadata
5. Additionally last layer is obfuscated using the AEAD algorithm

## Security Considerations
- `Poly1305` resists cipher tampering attemps if key and nonce pair is used only once.
- `XSalsa20` is still a strong symmetric encryption algorithm, although `ChaCha20` and it's variants are more widely used.
- Nonces are used only once per layer for each vault instance. There's no practical limit on generated messages with the same key, as the nonce is 192-bit cryptographically random value.
- `Argon2id` is used with single-use salts, providing resiliency against known password attacks.
- User is required to provide several strong passwords and some obfuscation pass phrases before them. See minimums below.

## Password Minimums
The weakest point of the construction is a user-readable password. Short and trivial passwords are called *pass phrases*, they are not considered secure.
A strong password must use:

- minimum 14 characters
- upper- and lowercase letters
- numbers
- special characters (e.g. ! @ #)

In such case selected passwords may be found in a space of tens/hundreds/thousands quadrillions combinations.

## So... what's the point if single password can be sufficient
It's all about minimizing the attack surface and purpose of the container.

Long-term container can be stolen, leaked or taken over in many other ways. Offline attacks are easier to perform,
because there is no server telling you to stop for 1 day after 20 attempts to find correct password.
This is made to be as most repelling as possible. To withstand as many generations of performance improvements as possible.
Excessive security which may never be tried out.

Ideally you should change your passwords every year or so. You must log in to some (e-mail) accounts every year or two,
otherwise they get deleted...
