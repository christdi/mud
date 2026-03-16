# Fix Deprecated OpenSSL SHA256 API

**Priority:** 19
**Created:** 2026-03-16
**Effort:** Low
**Risk:** Low
**Warnings eliminated:** 6 deprecation warnings (3 clang-tidy + 3 compiler)

## What

Replace the deprecated low-level OpenSSL SHA256 functions (`SHA256_Init`, `SHA256_Update`, `SHA256_Final`) in `src/util/mudhash.c` with the current EVP (Envelope) digest API.

## Why

OpenSSL deprecated the direct `SHA256_CTX` / `SHA256_Init` / `SHA256_Update` / `SHA256_Final` API in OpenSSL 3.0. These generate 6 deprecation warnings at build time (3 from clang-tidy, 3 from the compiler). The EVP API is the supported replacement and has been available since OpenSSL 1.0.

## How

The entire change is confined to `src/util/mudhash.c` (28 lines total).

### Current code

```c
#include <openssl/sha.h>

void mudhash_sha256(const char* input, char* output) {
  assert(input);
  assert(output);

  unsigned char hash[SHA256_DIGEST_LENGTH];
  size_t len = strnlen(input, MAX_SHA256_INPUT_LENGTH);

  SHA256_CTX context;
  SHA256_Init(&context);
  SHA256_Update(&context, input, len);
  SHA256_Final(hash, &context);

  int i = 0;
  for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf(output + (i * 2), "%02x", hash[i]);
  }

  output[SHA256_HEX_SIZE - 1] = '\0';
}
```

### Replacement using EVP API

```c
#include <openssl/evp.h>

void mudhash_sha256(const char* input, char* output) {
  assert(input);
  assert(output);

  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int hash_len = 0;
  size_t len = strnlen(input, MAX_SHA256_INPUT_LENGTH);

  EVP_MD_CTX* context = EVP_MD_CTX_new();
  EVP_DigestInit_ex(context, EVP_sha256(), NULL);
  EVP_DigestUpdate(context, input, len);
  EVP_DigestFinal_ex(context, hash, &hash_len);
  EVP_MD_CTX_free(context);

  for (unsigned int idx = 0; idx < hash_len; idx++) {
    sprintf(output + (idx * 2), "%02x", hash[idx]);
  }

  output[SHA256_HEX_SIZE - 1] = '\0';
}
```

Notes:
- `EVP_MD_CTX_new()` / `EVP_MD_CTX_free()` manage the context on the heap. Unlike the old stack-allocated `SHA256_CTX`, it must be freed.
- `EVP_MAX_MD_SIZE` (64 bytes) is large enough for any digest including SHA256 (32 bytes).
- `hash_len` is populated by `EVP_DigestFinal_ex` with the actual digest length (32 for SHA256). Using it in the loop instead of the hard-coded `SHA256_DIGEST_LENGTH` is more robust.
- The `sprintf` in the hex loop is acceptable here since `output + (idx * 2)` is always within bounds given a correctly sized output buffer. See `include/mud/util/mudhash.h` for `SHA256_HEX_SIZE` to confirm the output buffer contract.
- The loop counter rename from `i` to `idx` also resolves the `identifier-length` warning for this variable (from task 17).

### Update `CMakeLists.txt` if needed

Check that `libcrypto` is already in `target_link_libraries` (it is: the existing link line includes `ssl crypto`). No CMake changes are needed.

### Verification

```bash
make 2>&1 | grep -i "sha256\|deprecated\|openssl"
```

Should return zero results. Confirm the hash output is correct by adding a quick sanity check: SHA256("") should produce `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.
