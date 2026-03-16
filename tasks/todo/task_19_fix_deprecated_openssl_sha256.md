# Fix Deprecated OpenSSL SHA256 API

The SHA256 implementation uses the low-level `SHA256_CTX` / `SHA256_Init` / `SHA256_Update` / `SHA256_Final` API, deprecated since OpenSSL 3.0.

**Solution:** Replace with the EVP digest API (`EVP_MD_CTX`, `EVP_DigestInit_ex`, `EVP_DigestUpdate`, `EVP_DigestFinal_ex`).
