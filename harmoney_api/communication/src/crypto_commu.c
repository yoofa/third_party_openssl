/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: Implement encrypt decrpyt.
 * Author: luoxinqiang
 * Create: 2019-10-23
 */

#include "../include/crypto_commu.h"
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include "crypto_commu_log.h"

ssize_t CryptoCommuGetEncryptOverhead()
{
    return GCM_IV_BYTES + AES_BLOCK_SIZE;
}

ssize_t CryptoCommuEncrypt(const uint8_t *key, const void *in, size_t inLen, void *out, size_t outLen)
{
    if ((in == NULL) || ((ssize_t)inLen <= 0) || (out == NULL) || ((ssize_t)outLen <= 0)) {
        DB_LOGE("invalid parameters");
        return -1;
    }

    uint8_t *outBuf = (uint8_t *)(out);
    if (RAND_bytes(outBuf, GCM_IV_BYTES) != 1) {
        DB_LOGE("%s: generate iv fail!\n", __func__);
        return -1;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    do {
        ssize_t overheadLen = CryptoCommuGetEncryptOverhead();
        if (((ssize_t)(outLen - overheadLen) <= 0) || (inLen + overheadLen) > outLen) {
            DB_LOGE("outBuf small");
            break;
        }
        // the begin of outBuf is key
        if (EVP_EncryptInit(ctx, EVP_aes_256_gcm(), key, outBuf) != 1) {
            DB_LOGE("%s: init ctx fail!\n", __func__);
            break;
        }

        const uint8_t *inBuf = (const uint8_t *)(in);
        int outBufLen;
        int inBufLen = (int)(inLen);
        // outBuf + IV length is context
        if (EVP_EncryptUpdate(ctx, outBuf + GCM_IV_BYTES, &outBufLen, inBuf, inBufLen) != 1) {
            DB_LOGE("%s: update fail!\n", __func__);
            break;
        }

        int outLenFinal;
        if (EVP_EncryptFinal_ex(ctx, outBuf + GCM_IV_BYTES, &outLenFinal) < 0) {
            DB_LOGE("%s: final fail!\n", __func__);
            break;
        }
        // the last 16 byte of outbuf is tag
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, AES_BLOCK_SIZE, outBuf + GCM_IV_BYTES + inBufLen) != 1) {
            DB_LOGE("%s: get tag fail!\n", __func__);
            break;
        }
        EVP_CIPHER_CTX_free(ctx);
        return outBufLen + CryptoCommuGetEncryptOverhead();
    } while (0);
    // error handler
    EVP_CIPHER_CTX_free(ctx);
    return -1;
}

ssize_t CryptoCommuDecrypt(const uint8_t *key, const void *in, size_t inLen, void *out, size_t outLen)
{
    if ((key == NULL) || (in == NULL) || ((ssize_t)inLen <= 0) || (out == NULL) || ((ssize_t)outLen <= 0)) {
        DB_LOGE("invalid parameters");
        return -1;
    }

    const uint8_t *inBuf = (const uint8_t *)(in);
    uint8_t *outBuf = (uint8_t *)(out);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    // |---iv(12)-- | -----text----- | ----tag(16)----|
    do {
        ssize_t overheadLen = CryptoCommuGetEncryptOverhead();
        if (((ssize_t)(inLen - overheadLen) <= 0) || ((inLen - overheadLen) > outLen)) {
            DB_LOGE("outBuf small");
            break;
        }

        if (EVP_DecryptInit(ctx, EVP_aes_256_gcm(), key, inBuf) != 1) {
            DB_LOGE("%s: init ctx fail!\n", __func__);
            break;
        }

        int inBufLen = (int)(inLen);
        int outBufLen = 0;
        if (EVP_DecryptUpdate(ctx, outBuf, &outBufLen, inBuf + GCM_IV_BYTES, inBufLen - GCM_IV_BYTES - AES_BLOCK_SIZE)
            != 1) {
            DB_LOGE("%s: update fail!\n", __func__);
            break;
        }

        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, AES_BLOCK_SIZE,
                                (void *)(inBuf + GCM_IV_BYTES + outBufLen)) != 1) {
            DB_LOGE("%s: set tag fail!\n", __func__);
            break;
        }

        int outLenFinal;
        if (EVP_DecryptFinal_ex(ctx, outBuf, &outLenFinal) < 0) {
            break;
        }
        EVP_CIPHER_CTX_free(ctx);
        DB_LOGD("decrypt success");
        return outBufLen;
    } while (0);
    // error handler
    EVP_CIPHER_CTX_free(ctx);
    DB_LOGE("%s: decrypt fail!\n", __func__);
    return -1;
}
