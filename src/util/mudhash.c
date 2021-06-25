#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <openssl/sha.h>

#include "mud/util/mudhash.h"

void string_to_sha256(char * input, char * output) {
  assert(input);
  assert(output);

  unsigned char hash[SHA256_DIGEST_LENGTH];
  size_t len = strnlen(input, 128);

  SHA256_CTX context;
  SHA256_Init(&context);
  SHA256_Update(&context, input, len);
  SHA256_Final(hash, &context);

  int i = 0;

  for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf(output + (i * 2), "%02x", hash[i]);
  }

  output[64] = 0;
}