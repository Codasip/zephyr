/*
 * Copyright (c) 2016 Intel Corporation. Original TinyCrypt Shim Layer
 * Copyright (C) 2024 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Codasip AEAD Adaptor, making it complaint to crypto API.
 */

#include <stdbool.h>
#include <string.h>
#include <zephyr/crypto/crypto.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT codasip_aead_adaptor

LOG_MODULE_REGISTER(crypto_codasip_aead_adaptor, CONFIG_CRYPTO_LOG_LEVEL);

/* From Codasip Bare Metal Examples */

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_AEAD_H
#define BAREMETAL_AEAD_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Structure describing AEAD peripheral registers */
typedef struct bm_aead_regs bm_aead_regs_t;

/** \brief Block size options */
typedef enum {
        BM_AEAD_BLOCK_SIZE_64 = 0x0,
        BM_AEAD_BLOCK_SIZE_128 = 0x1,
} bm_aead_block_size_t;

/** \brief Key size options */
typedef enum {
        BM_AEAD_KEY_SIZE_128 = 0x0,
        BM_AEAD_KEY_SIZE_160 = 0x1,
        BM_AEAD_KEY_SIZE_192 = 0x2,
        BM_AEAD_KEY_SIZE_256 = 0x3,
} bm_aead_key_size_t;

/** \brief Nonce size options */
typedef enum {
        BM_AEAD_NONCE_SIZE_16 = 0x0,
        BM_AEAD_NONCE_SIZE_24 = 0x1,
        BM_AEAD_NONCE_SIZE_32 = 0x2,
        BM_AEAD_NONCE_SIZE_48 = 0x3,
        BM_AEAD_NONCE_SIZE_64 = 0x4,
        BM_AEAD_NONCE_SIZE_96 = 0x5,
        BM_AEAD_NONCE_SIZE_128 = 0x6,
} bm_aead_nonce_size_t;

/** \brief Tag size options */
typedef enum {
        BM_AEAD_TAG_SIZE_32 = 0x0,
        BM_AEAD_TAG_SIZE_48 = 0x1,
        BM_AEAD_TAG_SIZE_64 = 0x2,
        BM_AEAD_TAG_SIZE_80 = 0x3,
        BM_AEAD_TAG_SIZE_96 = 0x4,
        BM_AEAD_TAG_SIZE_112 = 0x5,
        BM_AEAD_TAG_SIZE_128 = 0x6,
} bm_aead_tag_size_t;

/** \brief Structure holding data neccessary to service a AEAD device. */
typedef struct {
        bm_aead_regs_t      *regs;       ///< Pointer to the peripheral registers
        bm_aead_block_size_t block_size; ///< Block size
        bm_aead_key_size_t   key_size;   ///< Key size
        bm_aead_nonce_size_t nonce_size; ///< Nonce size
        bm_aead_tag_size_t   tag_size;   ///< Tag size
} bm_aead_t;

/** \brief Structure holding AEAD command parameters */
typedef struct {
        bool decrypt; /* True for decryption, False for encryption */
        uint8_t *key;
        uint8_t *nonce;
        uint8_t *payload;
        uint32_t payload_size;
        uint8_t *additional_data;
        uint32_t ad_size;
        uint8_t *data_out;
        uint8_t *tag;
} bm_aead_command_t;

/**
 * \brief Function to initialize the AEAD peripheral
 *
 * \param aead Pointer to the AEAD driver structure linked to the peripheral to initialize.
 */
void bm_aead_init(bm_aead_t *aead);

/**
 * \brief Function to run a single command in AEAD core
 *
 * \param aead Pointer to the AEAD driver structure
 * \param command Command to run
 */
void bm_aead_run(bm_aead_t *aead, bm_aead_command_t *command);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_AEAD_H */

/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define STATUS_KEY_SIZE_OFFSET   0
#define STATUS_KEY_SIZE_MASK     (0xf << STATUS_KEY_SIZE_OFFSET)
#define STATUS_BLOCK_SIZE_OFFSET 4
#define STATUS_BLOCK_SIZE_MASK   (0xf << STATUS_BLOCK_SIZE_OFFSET)
#define STATUS_NONCE_SIZE_OFFSET 8
#define STATUS_NONCE_SIZE_MASK   (0xf << STATUS_NONCE_SIZE_OFFSET)
#define STATUS_TAG_SIZE_OFFSET   12
#define STATUS_TAG_SIZE_MASK     (0xf << STATUS_TAG_SIZE_OFFSET)

#define FIFO_STATUS_INPUT_OFFSET  0
#define FIFO_STATUS_INPUT_MASK    (0xffff << FIFO_STATUS_INPUT_OFFSET)
#define FIFO_STATUS_OUTPUT_OFFSET 16
#define FIFO_STATUS_OUTPUT_MASK   (0xffff << FIFO_STATUS_OUTPUT_OFFSET)

#define CONFIG_MODE_ENC 0x1
#define CONFIG_MODE_DEC 0x0

#define COMMAND_CONFIG_SET 0x1
#define COMMAND_FINALIZE   0x2
#define COMMAND_RESET      0x3

static void aead_copy_data_in(volatile uint32_t registers[], uint8_t *data, unsigned len)
{
        unsigned reg_count = (len + 3) / 4;
        for (unsigned i = 0; i < reg_count; i++) {
                uint32_t tmp = 0;
                memcpy(&tmp, data + 4 * i, 4);
                registers[i] = tmp;
        }
}

static void aead_copy_data_out(uint8_t *data, const volatile uint32_t registers[], unsigned len)
{
        unsigned reg_count = (len + 3) / 4;
        for (unsigned i = 0; i < reg_count; i++) {
                uint32_t tmp = registers[i];
                memcpy(data + 4 * i, &tmp, 4);
        }
}

struct bm_aead_regs {
        const volatile uint32_t FEATURES;       ///< (@ 0x0000) Features register
        char _reserved1[0xfc];
        const volatile uint32_t STATUS;         ///< (@ 0x0100) Status register
        const volatile uint32_t FIFO_STATUS;    ///< (@ 0x0104) FIFO status register
        char _reserved2[0xf8];
        volatile uint32_t CONFIG;               ///< (@ 0x0200) Configuration register
        volatile uint32_t COMMAND;              ///< (@ 0x0204) Command register
        char _reserved3[0x2f8];
        volatile uint32_t PAYLOAD_SIZE;         ///< (@ 0x0500) Payload size register
        char _reserved4[0x0fc];
        volatile uint32_t AD_SIZE;              ///< (@ 0x0600) Additional data size register
        char _reserved5[0x0fc];
        volatile uint32_t KEY[0x40];            ///< (@ 0x0700) Key registers
        volatile uint32_t NONCE[0x40];          ///< (@ 0x0800) Nonce registers
        volatile uint32_t BLOCK_IN[0x40];       ///< (@ 0x0900) Block in registers
        const volatile uint32_t DATA_OUT[0x40]; ///< (@ 0x0a00) Data out registers
        const volatile uint32_t TAG[0x40];      ///< (@ 0x0b00) Tag registers
};

static const unsigned aead_block_size_table[] = {8, 16};
static const unsigned aead_key_size_table[]   = {16, 20, 24, 32};
static const unsigned aead_nonce_size_table[] = {2, 3, 4, 6, 8, 12, 16};
static const unsigned aead_tag_size_table[]   = {4, 6, 8, 10, 12, 14, 16};

void bm_aead_init(bm_aead_t *aead)
{
        /* Read AEAD parameters */
        aead->block_size = aead_block_size_table[(aead->regs->STATUS & STATUS_BLOCK_SIZE_MASK) >>
                                                 STATUS_BLOCK_SIZE_OFFSET];
        aead->key_size = aead_key_size_table[(aead->regs->STATUS & STATUS_KEY_SIZE_MASK) >>
                                             STATUS_KEY_SIZE_OFFSET];
        aead->nonce_size = aead_nonce_size_table[(aead->regs->STATUS & STATUS_NONCE_SIZE_MASK) >>
                                                 STATUS_NONCE_SIZE_OFFSET];
        aead->tag_size = aead_tag_size_table[(aead->regs->STATUS & STATUS_TAG_SIZE_MASK) >>
                                             STATUS_TAG_SIZE_OFFSET];

        aead->regs->COMMAND = COMMAND_RESET;
}

static void aead_data_in_out(bm_aead_t *aead, uint8_t *data_in, uint8_t *data_out,
                             uint32_t data_size)
{
        unsigned fifo_avail;
        unsigned blocks_ready;
        unsigned blocks_written_in = 0;
        unsigned blocks_read_out = 0;
        unsigned data_num_blocks = (data_size + aead->block_size - 1) / aead->block_size;

        while (data_out ? (blocks_read_out < data_num_blocks)
                        : (blocks_written_in < data_num_blocks)) {
                fifo_avail = (aead->regs->FIFO_STATUS & FIFO_STATUS_INPUT_MASK) >>
                             FIFO_STATUS_INPUT_OFFSET;
                blocks_ready = fifo_avail < data_num_blocks - blocks_written_in
                                       ? fifo_avail
                                       : data_num_blocks - blocks_written_in;
                for (unsigned i = 0; i < blocks_ready; ++i, ++blocks_written_in) {
                        aead_copy_data_in(aead->regs->BLOCK_IN,
                                          data_in + blocks_written_in * aead->block_size,
                                          aead->block_size);
                }

                if (data_out) {
                        fifo_avail = (aead->regs->FIFO_STATUS & FIFO_STATUS_OUTPUT_MASK) >>
                                     FIFO_STATUS_OUTPUT_OFFSET;
                        blocks_ready = fifo_avail < data_num_blocks - blocks_read_out
                                               ? fifo_avail
                                               : data_num_blocks - blocks_read_out;
                        for (unsigned i = 0; i < blocks_ready; ++i, ++blocks_read_out) {
                                aead_copy_data_out(data_out + blocks_read_out * aead->block_size,
                                                   aead->regs->DATA_OUT, aead->block_size);
                        }
                }
        };
}

void bm_aead_run(bm_aead_t *aead, bm_aead_command_t *command)
{
        /* Configure AEAD core */
        aead->regs->CONFIG = command->decrypt ? CONFIG_MODE_DEC : CONFIG_MODE_ENC;

        aead->regs->PAYLOAD_SIZE = command->payload_size;
        aead->regs->AD_SIZE = command->ad_size;

        aead_copy_data_in(aead->regs->KEY, command->key, aead->key_size);
        aead_copy_data_in(aead->regs->NONCE, command->nonce, aead->nonce_size);

        aead->regs->COMMAND = COMMAND_CONFIG_SET;

        /* Process data */
        aead_data_in_out(aead, command->additional_data, NULL, command->ad_size);
        aead_data_in_out(aead, command->payload, command->data_out, command->payload_size);
        if (command->tag != NULL) {
                aead_copy_data_out(command->tag, aead->regs->TAG, aead->tag_size);
        }

        /* Reset AEAD */
        aead->regs->COMMAND = COMMAND_FINALIZE;
        aead->regs->COMMAND = COMMAND_RESET;
}

/* Modified TinyCrypt Shim Layer for Codasip AEAD Adaptor */

struct aead_drv_state_s {
        bool exists;
        bool in_use;

        /*	struct tc_aes_key_sched_struct session_key; */

        bm_aead_t aead;        /* Info obtained from DT */
        enum cipher_algo algo; /* Info obtained from DT */
        enum cipher_mode mode; /* Info obtained from DT */
};

#define CRYPTO_MAX_SESSION  DT_NUM_INST_STATUS_OKAY(codasip_aead_adaptor)

static struct aead_drv_state_s aead_driver_state[CRYPTO_MAX_SESSION];

static int do_aead_adaptor_encrypt(struct cipher_ctx *ctx, struct cipher_aead_pkt *pkt,
                                   uint8_t *nonce)
{
        struct aead_drv_state_s *data = ctx->drv_sessn_state;
        struct cipher_pkt *op = pkt->pkt;

        bm_aead_command_t bm_aead_command;
        bm_aead_command.decrypt         = false; /* True for decryption, False for encryption */
        bm_aead_command.key             = ctx->key.bit_stream;
        bm_aead_command.nonce           = nonce;
        bm_aead_command.payload         = op->in_buf;
        bm_aead_command.payload_size    = op->in_len;
        bm_aead_command.additional_data = pkt->ad;
        bm_aead_command.ad_size         = pkt->ad_len;
        bm_aead_command.data_out        = op->out_buf;
        bm_aead_command.tag             = pkt->tag;

        bm_aead_run(&data->aead, &bm_aead_command);

        op->out_len = op->in_len;

        return 0;
}

static int do_aead_adaptor_decrypt(struct cipher_ctx *ctx, struct cipher_aead_pkt *pkt,
                                   uint8_t *nonce)
{
        struct aead_drv_state_s *data = ctx->drv_sessn_state;
        struct cipher_pkt *op = pkt->pkt;

        bm_aead_command_t bm_aead_command;
        bm_aead_command.decrypt         = true; /* True for decryption, False for encryption */
        bm_aead_command.key             = ctx->key.bit_stream;
        bm_aead_command.nonce           = nonce;
        bm_aead_command.payload         = op->in_buf;
        bm_aead_command.payload_size    = op->in_len;
        bm_aead_command.additional_data = pkt->ad;
        bm_aead_command.ad_size         = pkt->ad_len;
        bm_aead_command.data_out        = op->out_buf;
        bm_aead_command.tag             = pkt->tag;

        bm_aead_run(&data->aead, &bm_aead_command);

        op->out_len = op->in_len;

        return 0;
}

static int aead_session_setup(const struct device *dev, struct cipher_ctx *ctx,
                              enum cipher_algo algo, enum cipher_mode mode, enum cipher_op op_type)
{
        struct aead_drv_state_s *data;
        int idx;

        if (!(ctx->flags & CAP_SYNC_OPS)) {
                LOG_ERR("AEAD Adaptor Async not supported by this driver");
                return -EINVAL;
        }

        /* Find a suitable AEAD PA for the required Algorithm & Mode */
        for (idx = 0; idx < CRYPTO_MAX_SESSION; idx++) {
                if (aead_driver_state[idx].exists        == true &&
                    aead_driver_state[idx].in_use        == false &&
                    aead_driver_state[idx].algo          == algo &&
                    aead_driver_state[idx].mode          == mode &&
                    aead_driver_state[idx].aead.key_size == ctx->keylen) {
                        aead_driver_state[idx].in_use = true;
                        break;
                }
        }

        if (idx == CRYPTO_MAX_SESSION) {
                LOG_ERR("AEAD Adaptor Algorithm/Mode/Key-Size not supported or Max sessions in "
                        "progress");
                return -EINVAL;
        }

        /* Both CCM and GCM are AEAD Algorithms */
        if (op_type == CRYPTO_CIPHER_OP_ENCRYPT) {
                ctx->ops.ccm_crypt_hndlr = (ccm_op_t)do_aead_adaptor_encrypt;
                ctx->ops.gcm_crypt_hndlr = (gcm_op_t)do_aead_adaptor_encrypt;
        } else {
                ctx->ops.ccm_crypt_hndlr = (ccm_op_t)do_aead_adaptor_decrypt;
                ctx->ops.gcm_crypt_hndlr = (gcm_op_t)do_aead_adaptor_decrypt;
        }

        ctx->ops.cipher_mode = mode;

        data = &aead_driver_state[idx];

        ctx->drv_sessn_state = data;

        return 0;
}

static int aead_query_caps(const struct device *dev)
{
        return (CAP_RAW_KEY | CAP_SEPARATE_IO_BUFS | CAP_SYNC_OPS);
}

static int aead_session_free(const struct device *dev, struct cipher_ctx *sessn)
{
        struct aead_drv_state_s *data = sessn->drv_sessn_state;

        ARG_UNUSED(dev);
        data->in_use = false;

        return 0;
}

static int aead_init(const struct device *dev)
{
        static bool initialised = false;

        int i;
        ARG_UNUSED(dev);

        if (!initialised) {
                for (i = 0; i < CRYPTO_MAX_SESSION; i++) {
                        aead_driver_state[i].exists =
                                false; /* This indicates the AEAD adaptor is not available */
                        aead_driver_state[i].in_use = false;
                }

                initialised = true;
        }

        return 0;
}

static struct crypto_driver_api crypto_enc_funcs = {
        .cipher_begin_session      = aead_session_setup,
        .cipher_free_session       = aead_session_free,
        .cipher_async_callback_set = NULL,
        .query_hw_caps             = aead_query_caps,
};

/* Define a "single" crypto device with one or more AEAD Adaptor sessions,
 * use this interface to the AEAD Crypto Function(s) */
DEVICE_DEFINE(crypto_codasip_aead, CONFIG_CRYPTO_CODASIP_AEAD_DRV_NAME, &aead_init, NULL, NULL,
              NULL, POST_KERNEL, CONFIG_CRYPTO_INIT_PRIORITY, (void *)&crypto_enc_funcs);

/* The following is used to pull all the AEAD Adaptor information from the Device Tree
 * and initialise everything */
#define CODASIP_AEAD_ADAPTOR_INIT(index)                                                           \
                                                                                                   \
        static int codasip_aead_adaptor_init_##index(const struct device *dev);                    \
                                                                                                   \
        DEVICE_DT_INST_DEFINE(index, &codasip_aead_adaptor_init_##index, NULL, NULL, NULL,         \
                              POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                    \
                              &crypto_enc_funcs);                                                  \
                                                                                                   \
        static int codasip_aead_adaptor_init_##index(const struct device *dev)                     \
        {                                                                                          \
                aead_init(NULL); /* This inits everything once, but can be called many times */    \
                                                                                                   \
                if (index < CRYPTO_MAX_SESSION) {                                                  \
                        aead_driver_state[index].exists = true;                                    \
                        aead_driver_state[index].in_use = false;                                   \
                                                                                                   \
                        aead_driver_state[index].aead.regs =                                       \
                                (bm_aead_regs_t *)DT_INST_REG_ADDR(index);                         \
                        bm_aead_init(&aead_driver_state[index].aead);                              \
                                                                                                   \
                        aead_driver_state[index].algo =                                            \
                                (enum cipher_algo)DT_INST_PROP(index, algo);                       \
                        aead_driver_state[index].mode =                                            \
                                (enum cipher_mode)DT_INST_PROP(index, mode);                       \
                } else {                                                                           \
                        LOG_ERR("CRYPTO_MAX_SESSION too small for AEAD Adaptor index (%d)",        \
                                index);                                                            \
                }                                                                                  \
                                                                                                   \
                return 0;                                                                          \
        }

DT_INST_FOREACH_STATUS_OKAY(CODASIP_AEAD_ADAPTOR_INIT)
