/*
 * Copyright (C) 2024 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_CRYPTO_CIPHER_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_CRYPTO_CIPHER_H_

/* Needs to be the same as the algorithms and modes specified in include/zephyr/crypto/cipher.h  */

/** Cipher Algorithm */
#define DT_BINDINGS_CRYPTO_CIPHER_ALGO_AES      1


/**
 * Possible cipher mode options.
 *
 * More to be added as required.
 */
#define DT_BINDINGS_CRYPTO_CIPHER_MODE_NONE     0

#define DT_BINDINGS_CRYPTO_CIPHER_MODE_ECB      1
#define DT_BINDINGS_CRYPTO_CIPHER_MODE_CBC      2
#define DT_BINDINGS_CRYPTO_CIPHER_MODE_CTR      3
#define DT_BINDINGS_CRYPTO_CIPHER_MODE_CCM      4
#define DT_BINDINGS_CRYPTO_CIPHER_MODE_GCM      5

#define DT_BINDINGS_CRYPTO_CIPHER_MODE_FAKE     99

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_CRYPTO_CIPHER_H_ */
