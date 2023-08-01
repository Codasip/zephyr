/*
 * Copyright (c) 2019 Tavish Naruka <tavishnaruka@gmail.com>
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Sample which uses the filesystem API and SDHC driver */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>
#include <ff.h>

#include <zephyr/drivers/entropy.h>

#if defined( CONFIG_MBEDTLS_HMAC_DRBG_ENABLED )
#include "mbedtls/hmac_drbg.h"

#else
#error Need to set "CONFIG_MBEDTLS_HMAC_DRBG_ENABLED=y" in the prj.conf file
#endif

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
#include "mbedtls/memory_buffer_alloc.h"

unsigned char buf[16000];
#endif


LOG_MODULE_REGISTER(main);

#define DISK_DRIVE_NAME "SD"
#define DISK_MOUNT_PT "/"DISK_DRIVE_NAME":"
#define MAX_PATH 128

static FATFS fat_fs;

/* mounting info */
static struct fs_mount_t mp = {
    .type = FS_FATFS,
    .fs_data = &fat_fs,
};


#define BUFFER_MAX 1024
static unsigned char buffer[ BUFFER_MAX ];


#define LINE_MAX 1024
static char line[ LINE_MAX ];

static void cat( const char *path_filename )
{
    int err;

    struct fs_file_t zfp;

    fs_file_t_init( &zfp );
    err = fs_open( &zfp, path_filename, FS_O_READ );

    if ( err == 0 )
    {
        int idx = 0;

        /* Read in lines */
        while ( fs_read( &zfp, &line[ idx ], 1 ) == 1 )
        {
            if ( line[ idx ] == 0x0a || idx >= ( LINE_MAX - 2 ) )
            {
                line[ idx + 1 ] = '\0';
                idx = 0;

                printk( "%s", line );
            }
            else
            {
                idx++;
            }
        }

        fs_close( &zfp );
    }
    else
    {
        printk("Can't open \"%s\" error %d\n", path_filename, err );
    }
}


typedef int (*get_random_fn_t)(void *, unsigned char *, size_t);

static void write_trng( const char *path_filename, get_random_fn_t get_random_fn, void *random_context, size_t len )
{
    int err;

    struct fs_file_t zfp;

    printk("Writing %d random bytes to file \"%s\"\n", len, path_filename );

    fs_file_t_init( &zfp );
    err = fs_open( &zfp, path_filename, FS_O_CREATE | FS_O_RDWR );

    if ( err == 0 )
    {
        int buf_len;
        for( ; len > 0; len -= buf_len )
        {
            buf_len = ( BUFFER_MAX < len ) ? BUFFER_MAX : len;
            
            /* Get random data */
            get_random_fn( random_context, buffer, buf_len );
            
            if ( fs_write( &zfp, &buffer, buf_len ) != buf_len )
            {
                printk( "Error writing %s\n", path_filename );
                break;
            }
        }

        fs_close( &zfp );

        printk("File \"%s\" written OK\n", path_filename );
    }
    else
    {
        printk("Can't open \"%s\" error %d\n", path_filename, err );
    }
}

/*
*  Note the fatfs library is able to mount only strings inside _VOLUME_STRS
*  in ffconf.h
*/
static const char *disk_mount_pt = DISK_MOUNT_PT;

#define ENTROPY_LENGTH (1024UL * 1024 * 1024)   /* 1GB */
#define RANDOM_TRNG_FILENAME   DISK_MOUNT_PT "/trng.dat"
#define RANDOM_DRBG_FILENAME   DISK_MOUNT_PT "/drbg.dat"

int main(void)
{
    /* raw disk i/o */
    do {
        static const char *disk_pdrv = DISK_DRIVE_NAME;
        uint64_t memory_size_mb;
        uint32_t block_count;
        uint32_t block_size;

        if (disk_access_init(disk_pdrv) != 0) {
            LOG_ERR("Storage init ERROR!");
            break;
        }

        if (disk_access_ioctl(disk_pdrv,
                DISK_IOCTL_GET_SECTOR_COUNT, &block_count)) {
            LOG_ERR("Unable to get sector count");
            break;
        }
        LOG_INF("Block count %u", block_count);

        if (disk_access_ioctl(disk_pdrv,
                DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) {
            LOG_ERR("Unable to get sector size");
            break;
        }
        printk("Sector size %u\n", block_size);

        memory_size_mb = (uint64_t)block_count * block_size;
        printk("Memory Size(MB) %u\n", (uint32_t)(memory_size_mb >> 20));
    } while (0);

    mp.mnt_point = disk_mount_pt;

    int res = fs_mount(&mp);

    if (res == FR_OK) {
        printk("Disk mounted.\n");

        cat(DISK_MOUNT_PT "/zephyr-logo.txt");

        /* Get Codasip TRNG Entropy data and write to SD Card */
        {
            const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_entropy));
            
            printk( "Get Codasip TRNG Entropy data and write to SD Card as " RANDOM_TRNG_FILENAME "\n"  );

            (void)memset(buffer, 0, BUFFER_MAX);

            /* Use entropy_get_entropy() as the random number source */
            write_trng( RANDOM_TRNG_FILENAME, (int (*)(void *, unsigned char *, size_t)) entropy_get_entropy, (void *) dev, ENTROPY_LENGTH );
        }

        /* Get Codasip HMAC DRBG data (using Codasip TRNG Entropy data as a seed) and write to SD Card */
        {
            const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_entropy));
            mbedtls_hmac_drbg_context ctx;
            const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type( MBEDTLS_MD_SHA1 );
            
            printk( "Get Codasip HMAC DRBG data (using Codasip TRNG Entropy data as a seed) and write to SD Card as " RANDOM_DRBG_FILENAME "\n"  );

            (void)memset(buffer, 0, BUFFER_MAX);

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
            mbedtls_memory_buffer_alloc_init(buf, sizeof(buf));
#endif

            mbedtls_hmac_drbg_init( &ctx );

#if 0
            /* Original non-Entropy software RNG as used in mbedtls_hmac_drbg_self_test() in hmac_drbg.c */
            mbedtls_hmac_drbg_seed( &ctx, md_info,
                                    hmac_drbg_self_test_entropy, (void *) entropy_pr,
                                    NULL, 0 );
#else

            /* Using Codasip Entropy TRNG as the seed */
            mbedtls_hmac_drbg_seed( &ctx, md_info,
                                    (int (*)(void *, unsigned char *, size_t)) entropy_get_entropy, (void *) dev,
                                    NULL, 0 );
#endif

            mbedtls_hmac_drbg_set_prediction_resistance( &ctx, MBEDTLS_HMAC_DRBG_PR_ON );

            /* Use mbedtls_hmac_drbg_random() as the random number source */
            write_trng( RANDOM_DRBG_FILENAME, (int (*)(void *, unsigned char *, size_t)) mbedtls_hmac_drbg_random, (void *) &ctx, ENTROPY_LENGTH );
        }

    } else {
        printk("Error mounting disk.\n");
    }


    while (1) {
        k_sleep(K_MSEC(1000));
    }
    return 0;
}
