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
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(main);

#define DISK_DRIVE_NAME   "SD"
#define DISK_MOUNT_PT     "/" DISK_DRIVE_NAME ":"
#define MAX_PATH          128
#define SOME_FILE_NAME    "some.dat"
#define SOME_DIR_NAME     "some"
#define SOME_REQUIRED_LEN MAX(sizeof(SOME_FILE_NAME), sizeof(SOME_DIR_NAME))

static FATFS fat_fs;

/* mounting info */
static struct fs_mount_t mp = {
        .type = FS_FATFS,
        .fs_data = &fat_fs,
};

static int lsdir(const char *path);
static void cat(const char *path_filename);

#ifdef CONFIG_SAMPLE_FATFS_CREATE_SOME_ENTRIES
static bool create_some_entries(const char *base_path)
{
        char path[MAX_PATH];
        struct fs_file_t file;
        int base = strlen(base_path);

        fs_file_t_init(&file);

        if (base >= (sizeof(path) - SOME_REQUIRED_LEN)) {
                LOG_ERR("Not enough concatenation buffer to create file paths");
                return false;
        }

        LOG_INF("Creating some dir entries in %s", base_path);
        strncpy(path, base_path, sizeof(path));

        path[base++] = '/';
        path[base] = 0;
        strcat(&path[base], SOME_FILE_NAME);

        if (fs_open(&file, path, FS_O_CREATE) != 0) {
                LOG_ERR("Failed to create file %s", path);
                return false;
        }
        fs_close(&file);

        path[base] = 0;
        strcat(&path[base], SOME_DIR_NAME);

        if (fs_mkdir(path) != 0) {
                LOG_ERR("Failed to create dir %s", path);
                /* If code gets here, it has at least successes to create the
                 * file so allow function to return true.
                 */
        }
        return true;
}
#endif

#define DISK_DRIVE_NAME "SD"
#define DISK_MOUNT_PT   "/" DISK_DRIVE_NAME ":"
#define MAX_PATH        128

/*
 *  Note the fatfs library is able to mount only strings inside _VOLUME_STRS
 *  in ffconf.h
 */
static const char *disk_mount_pt = DISK_MOUNT_PT;

#define COUNTING_FILENAME DISK_MOUNT_PT "/counting.dat"

#define BUFFER_MAX 1024
static unsigned char buffer[BUFFER_MAX];

static void write_counting(const char *path_filename, size_t len)
{
        int err, ctr = 0;

        struct fs_file_t zfp;

        printk("Writing %d counting bytes to file \"%s\"\n", (int) len, path_filename);

        fs_file_t_init(&zfp);
        err = fs_open(&zfp, path_filename, FS_O_CREATE | FS_O_RDWR);

        if (err == 0) {
                size_t buf_len, written = 0, written_last_update = 0;

                for (; len > 0; len -= buf_len) {
                        buf_len = (BUFFER_MAX < len) ? BUFFER_MAX : len;

                        /* Get counting data */
                        // get_random_fn( random_context, buffer, buf_len );
                        uint32_t idx;
                        for (idx = 0; idx < buf_len; idx++) {
                                buffer[idx] = ctr++;
                        }

                        if (fs_write(&zfp, &buffer, buf_len) != buf_len) {
                                printk("Error writing %s\n", path_filename);
                                break;
                        }

                        written += buf_len;
                        if (written >= written_last_update + 0x10000) {
                                printk("Written 0x%x to file %s\n", (unsigned int) written, path_filename);
                                written_last_update = written;
                        }
                }

                fs_close(&zfp);

                printk("File \"%s\" written OK\n", path_filename);
        } else {
                printk("Can't open \"%s\" error %d\n", path_filename, err);
        }
}

int main(void)
{
        /* raw disk i/o */
        do {
                static const char *disk_pdrv = DISK_DRIVE_NAME;
                uint64_t memory_size_mb;
                uint32_t block_count;
                uint32_t block_size;

                printk("disk_access_init(disk_pdrv)\n");
                if (disk_access_init(disk_pdrv) != 0) {
                        LOG_ERR("Storage init ERROR!");
                        break;
                }

                printk("disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_COUNT, &block_count)\n");
                if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_COUNT, &block_count)) {
                        LOG_ERR("Unable to get sector count");
                        break;
                }
                LOG_INF("Block count %u", block_count);

                printk("disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_SIZE, &block_size)\n");
                if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) {
                        LOG_ERR("Unable to get sector size");
                        break;
                }
                printk("Sector size %u\n", block_size);

                memory_size_mb = (uint64_t)block_count * block_size;
                printk("Memory Size(MB) %u\n", (uint32_t)(memory_size_mb >> 20));

                mp.mnt_point = disk_mount_pt;

                printk("fs_mount(&mp)\n");
                int res = fs_mount(&mp);

                if (res == FR_OK) {
                        printk("Disk mounted.\n");

                        cat(DISK_MOUNT_PT "/zephyr-logo.txt");

                        if (lsdir(disk_mount_pt) == 0) {
#ifdef CONFIG_SAMPLE_FATFS_CREATE_SOME_ENTRIES
                                if (create_some_entries(disk_mount_pt)) {
                                        lsdir(disk_mount_pt);
                                }
#endif
                        }
                } else {
                        printk("Error mounting disk.\n");
                        break;
                }
        } while (0);

        write_counting(COUNTING_FILENAME, 0x1000000);

        while (1) {
                k_sleep(K_MSEC(1000));
        }
        return 0;
}

/* List dir entry by path
 *
 * @param path Absolute path to list
 *
 * @return Negative errno code on error, number of listed entries on
 *         success.
 */
static int lsdir(const char *path)
{
        int res;
        struct fs_dir_t dirp;
        static struct fs_dirent entry;
        int count = 0;

        fs_dir_t_init(&dirp);

        /* Verify fs_opendir() */
        res = fs_opendir(&dirp, path);
        if (res) {
                printk("Error opening dir %s [%d]\n", path, res);
                return res;
        }

        printk("\nListing dir %s ...\n", path);
        for (;;) {
                /* Verify fs_readdir() */
                res = fs_readdir(&dirp, &entry);

                /* entry.name[0] == 0 means end-of-dir */
                if (res || entry.name[0] == 0) {
                        break;
                }

                if (entry.type == FS_DIR_ENTRY_DIR) {
                        printk("[DIR ] %s\n", entry.name);
                } else {
                        printk("[FILE] %s (size = %zu)\n", entry.name, entry.size);
                }
                count++;
        }

        /* Verify fs_closedir() */
        fs_closedir(&dirp);
        if (res == 0) {
                res = count;
        }

        return res;
}

#define LINE_MAX 1024

static char line[LINE_MAX];

static void cat(const char *path_filename)
{
        int err;

        struct fs_file_t zfp;

        fs_file_t_init(&zfp);
        err = fs_open(&zfp, path_filename, FS_O_READ);

        if (err == 0) {
                int idx = 0;

                /* Read in lines */
                while (fs_read(&zfp, &line[idx], 1) == 1) {
                        if (line[idx] == 0x0a || idx >= (LINE_MAX - 2)) {
                                line[idx + 1] = '\0';
                                idx = 0;

                                printk("%s", line);
                        } else {
                                idx++;
                        }
                }

                fs_close(&zfp);
        } else {
                printk("Can't open \"%s\" error %d", path_filename, err);
        }
}
