/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Interface of encrypt decrypt
 * Author: tanshengguo
 * Create: 2019-10-23
 */

#ifndef CRYPTO_COMMUH_H
#define CRYPTO_COMMUH_H

#include <stdint.h>
#include <sys/types.h>

#define GCM_KEY_LENGTH_256 32
#define GCM_IV_BITS 96
#define GCM_IV_BYTES 12
#define AES_BLOCK_SIZE 16
#define OPENSSL_API __attribute__((visibility("default")))
#define OPENSSL_UNUSED __attribute__((unused))

OPENSSL_API extern ssize_t CryptoCommuGetEncryptOverhead();
OPENSSL_API extern ssize_t CryptoCommuEncrypt(const uint8_t *key, const void *in, size_t inLen, void *out,
    size_t outLen);
OPENSSL_API extern ssize_t CryptoCommuDecrypt(const uint8_t *key, const void *in, size_t inLen, void *out,
    size_t outLen);

#endif // CRYPTO_COMMUH_H
