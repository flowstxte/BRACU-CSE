// Build: gcc -O2 -std=c17 -Wall -Wextra mkfs_minivsfs.c -o mkfs_builder
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <getopt.h>
#include <sys/stat.h>

#define BS 4096u // block size
#define INODE_SIZE 128u
#define ROOT_INO 1u
#define DIRECT_MAX 12

uint64_t g_random_seed = 0; // This should be replaced by seed value from the CLI.

// below contains some basic structures you need for your project
// you are free to create more structures as you require
#pragma pack(push, 1)
typedef struct
{
    // CREATE YOUR SUPERBLOCK HERE
    // ADD ALL FIELDS AS PROVIDED BY THE SPECIFICATION
    uint32_t magic;
    uint32_t version;
    uint32_t block_size;
    uint64_t total_blocks;
    uint64_t inode_count;
    uint64_t inode_bitmap_start;
    uint64_t inode_bitmap_blocks;
    uint64_t data_bitmap_start;
    uint64_t data_bitmap_blocks;
    uint64_t inode_table_start;
    uint64_t inode_table_blocks;
    uint64_t data_region_start;
    uint64_t data_region_blocks;
    uint64_t root_inode;
    uint64_t mtime_epoch;
    uint32_t flags;
    // THIS FIELD SHOULD STAY AT THE END
    // ALL OTHER FIELDS SHOULD BE ABOVE THIS
    uint32_t checksum; // crc32(superblock[0..4091])
} superblock_t;
#pragma pack(pop)
_Static_assert(sizeof(superblock_t) == 116, "superblock must fit in one block");

#pragma pack(push, 1)
typedef struct
{
    // CREATE YOUR INODE HERE
    // IF CREATED CORRECTLY, THE STATIC_ASSERT ERROR SHOULD BE GONE
    uint16_t mode;
    uint16_t links;
    uint32_t uid;
    uint32_t gid;
    uint64_t size_bytes;
    uint64_t atime;
    uint64_t mtime;
    uint64_t ctime;
    uint32_t direct[DIRECT_MAX];
    uint32_t reserved_0;
    uint32_t reserved_1;
    uint32_t reserved_2;
    uint32_t proj_id;
    uint32_t uid16_gid16;
    uint64_t xattr_ptr;
    // THIS FIELD SHOULD STAY AT THE END
    // ALL OTHER FIELDS SHOULD BE ABOVE THIS
    uint64_t inode_crc; // low 4 bytes store crc32 of bytes [0..119]; high 4 bytes 0
} inode_t;
#pragma pack(pop)
_Static_assert(sizeof(inode_t) == INODE_SIZE, "inode size mismatch");

#pragma pack(push, 1)
typedef struct
{
    // CREATE YOUR DIRECTORY ENTRY STRUCTURE HERE
    // IF CREATED CORRECTLY, THE STATIC_ASSERT ERROR SHOULD BE GONE
    uint32_t inode_no;
    uint8_t type;
    char name[58];
    uint8_t checksum; // XOR of bytes 0..62
} dirent64_t;
#pragma pack(pop)
_Static_assert(sizeof(dirent64_t) == 64, "dirent size mismatch");

// ==========================DO NOT CHANGE THIS PORTION=========================
// These functions are there for your help. You should refer to the specifications to see how you can use them.
// ====================================CRC32====================================
uint32_t CRC32_TAB[256];
void crc32_init(void)
{
    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t c = i;
        for (int j = 0; j < 8; j++)
            c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        CRC32_TAB[i] = c;
    }
}
uint32_t crc32(const void *data, size_t n)
{
    const uint8_t *p = (const uint8_t *)data;
    uint32_t c = 0xFFFFFFFFu;
    for (size_t i = 0; i < n; i++)
        c = CRC32_TAB[(c ^ p[i]) & 0xFF] ^ (c >> 8);
    return c ^ 0xFFFFFFFFu;
}
// ====================================CRC32====================================

// WARNING: CALL THIS ONLY AFTER ALL OTHER SUPERBLOCK ELEMENTS HAVE BEEN FINALIZED
static uint32_t superblock_crc_finalize(superblock_t *sb)
{
    sb->checksum = 0;
    uint32_t s = crc32((void *)sb, BS - 4);
    sb->checksum = s;
    return s;
}

// WARNING: CALL THIS ONLY AFTER ALL OTHER SUPERBLOCK ELEMENTS HAVE BEEN FINALIZED
void inode_crc_finalize(inode_t *ino)
{
    uint8_t tmp[INODE_SIZE];
    memcpy(tmp, ino, INODE_SIZE);
    // zero crc area before computing
    memset(&tmp[120], 0, 8);
    uint32_t c = crc32(tmp, 120);
    ino->inode_crc = (uint64_t)c; // low 4 bytes carry the crc
}

// WARNING: CALL THIS ONLY AFTER ALL OTHER SUPERBLOCK ELEMENTS HAVE BEEN FINALIZED
void dirent_checksum_finalize(dirent64_t *de)
{
    const uint8_t *p = (const uint8_t *)de;
    uint8_t x = 0;
    for (int i = 0; i < 63; i++)
        x ^= p[i]; // covers ino(4) + type(1) + name(58)
    de->checksum = x;
}

void print_usage(const char *program_name)
{
    printf("Usage: %s --image <output.img> --size-kib <180..4096> --inodes <128..512>\n", program_name);
}

int main(int argc, char *argv[])
{
    crc32_init();

    // WRITE YOUR DRIVER CODE HERE
    // PARSE YOUR CLI PARAMETERS
    char *out_img = NULL;
    uint64_t fs_size = 0;
    uint64_t max_inodes = 0;

    static struct option options[] = {
        {"image", required_argument, 0, 'i'},
        {"size-kib", required_argument, 0, 's'},
        {"inodes", required_argument, 0, 'n'},
        {0, 0, 0, 0}};

    int opt;
    while ((opt = getopt_long(argc, argv, "i:s:n:", options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'i':
            out_img = optarg;
            break;
        case 's':
            fs_size = strtoull(optarg, NULL, 10);
            break;
        case 'n':
            max_inodes = strtoull(optarg, NULL, 10);
            break;
        default:
            print_usage(argv[0]);
            return 1;
        }
    }

    if (!out_img || fs_size == 0 || max_inodes == 0)
    {
        fprintf(stderr, "Error: Missing required arguments\n");
        print_usage(argv[0]);
        return 1;
    }

    if (fs_size < 180 || fs_size > 4096 || (fs_size % 4) != 0)
    {
        fprintf(stderr, "Error: size-kib must be between 180 and 4096 and multiple of 4\n");
        return 1;
    }

    if (max_inodes < 128 || max_inodes > 512)
    {
        fprintf(stderr, "Error: inodes must be between 128 and 512\n");
        return 1;
    }

    // THEN CREATE YOUR FILE SYSTEM WITH A ROOT DIRECTORY
    uint64_t total_blocks = (fs_size * 1024) / BS;
    uint64_t inode_table_block_count = (max_inodes * INODE_SIZE + BS - 1) / BS;
    uint64_t data_blocks = total_blocks - 3 - inode_table_block_count;

    if (data_blocks < 1)
    {
        fprintf(stderr, "Error: Not enough space for data region\n");
        return 1;
    }

    superblock_t sb = {
        .magic = 0x4D565346,
        .version = 1,
        .block_size = BS,
        .total_blocks = total_blocks,
        .inode_count = max_inodes,
        .inode_bitmap_start = 1,
        .inode_bitmap_blocks = 1,
        .data_bitmap_start = 2,
        .data_bitmap_blocks = 1,
        .inode_table_start = 3,
        .inode_table_blocks = inode_table_block_count,
        .data_region_start = 3 + inode_table_block_count,
        .data_region_blocks = data_blocks,
        .root_inode = ROOT_INO,
        .mtime_epoch = time(NULL),
        .flags = 0};

    inode_t root_dir = {
        .mode = 0040000,
        .links = 2,
        .uid = 0,
        .gid = 0,
        .size_bytes = 2 * sizeof(dirent64_t),
        .atime = sb.mtime_epoch,
        .mtime = sb.mtime_epoch,
        .ctime = sb.mtime_epoch,
        .direct = {sb.data_region_start, 0},
        .reserved_0 = 0,
        .reserved_1 = 0,
        .reserved_2 = 0,
        .proj_id = 6,
        .uid16_gid16 = 0,
        .xattr_ptr = 0};

    dirent64_t dot = {ROOT_INO, 2, ".", 0};
    dirent64_t dotdot = {ROOT_INO, 2, "..", 0};

    // THEN SAVE THE DATA INSIDE THE OUTPUT IMAGE
    FILE *file = fopen(out_img, "wb");
    if (!file)
    {
        fprintf(stderr, "Error: Cannot create output file %s: %s\n", out_img, strerror(errno));
        return 1;
    }

    uint8_t *filesystem_image = calloc(total_blocks, BS);
    if (!filesystem_image)
    {
        fprintf(stderr, "Error: Cannot allocate memory for image\n");
        fclose(file);
        return 1;
    }
    memcpy(filesystem_image, &sb, sizeof(superblock_t));
    superblock_crc_finalize((superblock_t *)filesystem_image);
    uint8_t *inode_bitmap = filesystem_image + (sb.inode_bitmap_start * BS);
    inode_bitmap[0] = 0x01;

    uint8_t *data_bitmap = filesystem_image + (sb.data_bitmap_start * BS);
    data_bitmap[0] = 0x01;
    inode_crc_finalize(&root_dir);
    uint8_t *inode_table = filesystem_image + (sb.inode_table_start * BS);
    memcpy(inode_table, &root_dir, sizeof(inode_t));
    dirent_checksum_finalize(&dot);
    dirent_checksum_finalize(&dotdot);
    uint8_t *root_data = filesystem_image + (sb.data_region_start * BS);
    memcpy(root_data, &dot, sizeof(dirent64_t));
    memcpy(root_data + sizeof(dirent64_t), &dotdot, sizeof(dirent64_t));

    size_t bytes_written = fwrite(filesystem_image, 1, total_blocks * BS, file);
    if (bytes_written != total_blocks * BS)
    {
        fprintf(stderr, "Error: Failed to write complete image\n");
        free(filesystem_image);
        fclose(file);
        return 1;
    }

    printf("Successfully created MiniVSFS image: %s\n", out_img);
    printf("Size: %lu KiB, Inodes: %lu, Total blocks: %lu\n", fs_size, max_inodes, total_blocks);

    free(filesystem_image);
    fclose(file);
    return 0;
}