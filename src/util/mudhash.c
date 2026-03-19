#include <assert.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

#include "mud/util/mudhash.h"

void mudhash_sha256(const char* input, char* output) {
  assert(input);
  assert(output);

  unsigned char hash[SHA256_DIGEST_LENGTH];
  size_t len = strnlen(input, MAX_SHA256_INPUT_LENGTH);
  unsigned int hash_len = SHA256_DIGEST_LENGTH;

  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
  EVP_DigestUpdate(ctx, input, len);
  EVP_DigestFinal_ex(ctx, hash, &hash_len);
  EVP_MD_CTX_free(ctx);

  int idx = 0;

  for (idx = 0; idx < SHA256_DIGEST_LENGTH; idx++) {
    sprintf(output + (idx * 2), "%02x", hash[idx]);
  }

  output[SHA256_HEX_SIZE - 1] = '\0';
}
