// Build: gcc -O2 -std=c17 -Wall -Wextra vsfs_reader.c -o vsfs_reader
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define BS 4096u
#define INODE_SIZE 128u
#define ROOT_INO 1u
#define DIRECT_MAX 12

#pragma pack(push, 1)
typedef struct
{
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
    uint32_t checksum;
} superblock_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
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
    uint64_t inode_crc;
} inode_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    uint32_t inode_no;
    uint8_t type;
    char name[58];
    uint8_t checksum;
} dirent64_t;
#pragma pack(pop)

void print_time(uint64_t timestamp)
{
    if (timestamp == 0)
    {
        printf("N/A");
        return;
    }
    time_t t = (time_t)timestamp;
    struct tm *tm_info = localtime(&t);
    printf("%04d-%02d-%02d %02d:%02d:%02d",
           tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
           tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
}

void print_superblock(superblock_t *sb)
{
    printf("=== SUPERBLOCK ===\n");
    printf("Magic: 0x%08X\n", sb->magic);
    printf("Version: %u\n", sb->version);
    printf("Block size: %u bytes\n", sb->block_size);
    printf("Total blocks: %lu\n", sb->total_blocks);
    printf("Inode count: %lu\n", sb->inode_count);
    printf("Inode bitmap: block %lu (1 block)\n", sb->inode_bitmap_start);
    printf("Data bitmap: block %lu (1 block)\n", sb->data_bitmap_start);
    printf("Inode table: block %lu (%lu blocks)\n", sb->inode_table_start, sb->inode_table_blocks);
    printf("Data region: block %lu (%lu blocks)\n", sb->data_region_start, sb->data_region_blocks);
    printf("Root inode: %lu\n", sb->root_inode);
    printf("Mount time: ");
    print_time(sb->mtime_epoch);
    printf("\n");
    printf("Flags: 0x%08X\n", sb->flags);
    printf("Checksum: 0x%08X\n", sb->checksum);
    printf("\n");
}

void print_bitmap(uint8_t *bitmap, uint64_t max_items, const char *name)
{
    printf("=== %s BITMAP ===\n", name);
    printf("Used items: ");
    int count = 0;
    for (uint64_t i = 0; i < max_items; i++)
    {
        uint64_t byte_idx = i / 8;
        int bit_idx = i % 8;
        if (bitmap[byte_idx] & (1 << bit_idx))
        {
            printf("%lu ", i + 1); // Show as 1-indexed
            count++;
        }
    }
    printf("\nTotal used: %d/%lu\n\n", count, max_items);
}

void print_inode(inode_t *inode, uint64_t inode_num)
{
    printf("--- Inode %lu ---\n", inode_num);
    printf("Mode: 0%06o (%s)\n", inode->mode,
           (inode->mode & 0040000) ? "directory" : "regular file");
    printf("Links: %u\n", inode->links);
    printf("UID: %u, GID: %u\n", inode->uid, inode->gid);
    printf("Size: %lu bytes\n", inode->size_bytes);
    printf("Access time: ");
    print_time(inode->atime);
    printf("\nModify time: ");
    print_time(inode->mtime);
    printf("\nCreate time: ");
    print_time(inode->ctime);
    printf("\nDirect blocks: ");
    for (int i = 0; i < DIRECT_MAX; i++)
    {
        if (inode->direct[i] != 0)
        {
            printf("%u ", inode->direct[i]);
        }
    }
    printf("\nProject ID: %u\n", inode->proj_id);
    printf("CRC: 0x%016lX\n", inode->inode_crc);
    printf("\n");
}

void print_directory_contents(uint8_t *data_region, inode_t *dir_inode, superblock_t *sb)
{
    if (!(dir_inode->mode & 0040000))
    {
        printf("Not a directory\n");
        return;
    }

    printf("Directory contents:\n");
    for (int i = 0; i < DIRECT_MAX && dir_inode->direct[i] != 0; i++)
    {
        uint64_t block_offset = (dir_inode->direct[i] - sb->data_region_start) * BS;
        dirent64_t *entries = (dirent64_t *)(data_region + block_offset);

        for (int j = 0; j < BS / sizeof(dirent64_t); j++)
        {
            if (entries[j].inode_no != 0)
            {
                printf("  %-20s inode=%u type=%s\n",
                       entries[j].name,
                       entries[j].inode_no,
                       entries[j].type == 1 ? "file" : "dir");
            }
        }
    }
    printf("\n");
}

void print_file_contents(uint8_t *data_region, inode_t *file_inode, superblock_t *sb)
{
    if (file_inode->mode & 0040000)
    {
        printf("This is a directory, not a file\n");
        return;
    }

    printf("File contents (%lu bytes):\n", file_inode->size_bytes);
    printf("----------------------------------------\n");

    uint64_t bytes_left = file_inode->size_bytes;
    for (int i = 0; i < DIRECT_MAX && file_inode->direct[i] != 0 && bytes_left > 0; i++)
    {
        uint64_t block_offset = (file_inode->direct[i] - sb->data_region_start) * BS;
        uint8_t *block_data = data_region + block_offset;

        uint64_t bytes_to_print = (bytes_left > BS) ? BS : bytes_left;

        // Print as text (with some safety for binary data)
        for (uint64_t j = 0; j < bytes_to_print; j++)
        {
            char c = block_data[j];
            if (c >= 32 && c <= 126)
            {
                printf("%c", c);
            }
            else if (c == '\n')
            {
                printf("\\n\n");
            }
            else if (c == '\t')
            {
                printf("\\t");
            }
            else
            {
                printf("\\x%02X", (unsigned char)c);
            }
        }

        bytes_left -= bytes_to_print;
    }
    printf("\n----------------------------------------\n\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <image.img> [inode_num]\n", argv[0]);
        printf("  If inode_num is provided, shows detailed info for that inode\n");
        printf("  If inode_num is 0, shows file contents for all regular files\n");
        return 1;
    }

    char *image_name = argv[1];
    uint64_t specific_inode = 0;
    if (argc >= 3)
    {
        specific_inode = strtoull(argv[2], NULL, 10);
    }

    // Open image file
    FILE *fp = fopen(image_name, "rb");
    if (!fp)
    {
        fprintf(stderr, "Error: Cannot open image %s\n", image_name);
        return 1;
    }

    // Get file size
    fseek(fp, 0, SEEK_END);
    long image_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Read entire image
    uint8_t *image = malloc(image_size);
    if (!image)
    {
        fprintf(stderr, "Error: Cannot allocate memory\n");
        fclose(fp);
        return 1;
    }

    if (fread(image, 1, image_size, fp) != image_size)
    {
        fprintf(stderr, "Error: Cannot read image\n");
        free(image);
        fclose(fp);
        return 1;
    }
    fclose(fp);

    // Parse filesystem
    superblock_t *sb = (superblock_t *)image;
    uint8_t *inode_bitmap = image + (sb->inode_bitmap_start * BS);
    uint8_t *data_bitmap = image + (sb->data_bitmap_start * BS);
    uint8_t *inode_table = image + (sb->inode_table_start * BS);
    uint8_t *data_region = image + (sb->data_region_start * BS);

    // Validate magic
    if (sb->magic != 0x4D565346)
    {
        fprintf(stderr, "Error: Invalid filesystem magic\n");
        free(image);
        return 1;
    }

    // Print filesystem info
    print_superblock(sb);
    print_bitmap(inode_bitmap, sb->inode_count, "INODE");
    print_bitmap(data_bitmap, sb->data_region_blocks, "DATA");

    printf("=== INODES ===\n");
    inode_t *inodes = (inode_t *)inode_table;

    if (specific_inode != 0)
    {
        // Show specific inode
        if (specific_inode > sb->inode_count)
        {
            fprintf(stderr, "Error: Inode %lu out of range\n", specific_inode);
            free(image);
            return 1;
        }

        inode_t *inode = &inodes[specific_inode - 1];
        if (inode->mode != 0)
        {
            print_inode(inode, specific_inode);

            if (inode->mode & 0040000)
            {
                print_directory_contents(data_region, inode, sb);
            }
            else
            {
                print_file_contents(data_region, inode, sb);
            }
        }
        else
        {
            printf("Inode %lu is not allocated\n", specific_inode);
        }
    }
    else
    {
        // Show all allocated inodes
        for (uint64_t i = 1; i <= sb->inode_count; i++)
        {
            uint64_t byte_idx = (i - 1) / 8;
            int bit_idx = (i - 1) % 8;

            if (inode_bitmap[byte_idx] & (1 << bit_idx))
            {
                inode_t *inode = &inodes[i - 1];
                print_inode(inode, i);

                if (inode->mode & 0040000)
                {
                    print_directory_contents(data_region, inode, sb);
                }
                else if (argc >= 3 && specific_inode == 0)
                {
                    // Show file contents if requested with inode_num = 0
                    print_file_contents(data_region, inode, sb);
                }
            }
        }
    }

    free(image);
    return 0;
}