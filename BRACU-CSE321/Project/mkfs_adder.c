#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <getopt.h>
#include <sys/stat.h>

#define BS 4096u
#define INODE_SIZE 128u
#define ROOT_INO 1u
#define DIRECT_MAX 12
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

uint64_t find_free_inode(uint8_t *inode_bitmap, uint64_t max_inodes)
{
    for (uint64_t byte_idx = 0; byte_idx < (max_inodes + 7) / 8; byte_idx++)
    {
        for (int bit_idx = 0; bit_idx < 8; bit_idx++)
        {
            uint64_t inode_num = byte_idx * 8 + bit_idx + 1;
            if (inode_num > max_inodes)
                break;

            if (!(inode_bitmap[byte_idx] & (1 << bit_idx)))
            {
                return inode_num;
            }
        }
    }
    return 0;
}

uint64_t find_free_data_block(uint8_t *data_bitmap, uint64_t data_blocks_count)
{
    for (uint64_t byte_idx = 0; byte_idx < (data_blocks_count + 7) / 8; byte_idx++)
    {
        for (int bit_idx = 0; bit_idx < 8; bit_idx++)
        {
            uint64_t block_num = byte_idx * 8 + bit_idx;
            if (block_num >= data_blocks_count)
                break;

            if (!(data_bitmap[byte_idx] & (1 << bit_idx)))
            {
                return block_num;
            }
        }
    }
    return 0;
}

void mark_bitmap_bit(uint8_t *bitmap, uint64_t bit_position)
{
    uint64_t byte_idx = bit_position / 8;
    int bit_idx = bit_position % 8;
    bitmap[byte_idx] |= (1 << bit_idx);
}

void print_usage(const char *program_name)
{
    printf("Usage: %s --input <input.img> --output <output.img> --file <filename>\n", program_name);
}

int main(int argc, char *argv[])
{
    crc32_init();

    // WRITE YOUR DRIVER CODE HERE
    // PARSE YOUR CLI PARAMETERS
    char *inp_img = NULL;
    char *out_img = NULL;
    char *file_add = NULL;

    static struct option options[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"file", required_argument, 0, 'f'},
        {0, 0, 0, 0}};

    int opt;
    while ((opt = getopt_long(argc, argv, "i:o:f:", options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'i':
            inp_img = optarg;
            break;
        case 'o':
            out_img = optarg;
            break;
        case 'f':
            file_add = optarg;
            break;
        default:
            print_usage(argv[0]);
            return 1;
        }
    }

    if (!inp_img || !out_img || !file_add)
    {
        fprintf(stderr, "Error: Missing required arguments\n");
        print_usage(argv[0]);
        return 1;
    }

    struct stat input_file_stat;
    if (stat(file_add, &input_file_stat) != 0)
    {
        fprintf(stderr, "Error: Cannot access file %s: %s\n", file_add, strerror(errno));
        return 1;
    }

    if (!S_ISREG(input_file_stat.st_mode))
    {
        fprintf(stderr, "Error: %s is not a regular file\n", file_add);
        return 1;
    }

    FILE *filesystem_file = fopen(inp_img, "rb");
    if (!filesystem_file)
    {
        fprintf(stderr, "Error: Cannot open input image %s: %s\n", inp_img, strerror(errno));
        return 1;
    }

    superblock_t sb;
    if (fread(&sb, sizeof(superblock_t), 1, filesystem_file) != 1)
    {
        fprintf(stderr, "Error: Cannot read superblock\n");
        fclose(filesystem_file);
        return 1;
    }

    if (sb.magic != 0x4D565346)
    {
        fprintf(stderr, "Error: Invalid filesystem magic number\n");
        fclose(filesystem_file);
        return 1;
    }

    fseek(filesystem_file, 0, SEEK_END);
    long filesystem_size = ftell(filesystem_file);
    fseek(filesystem_file, 0, SEEK_SET);

    uint8_t *filesystem_data = malloc(filesystem_size);
    if (!filesystem_data)
    {
        fprintf(stderr, "Error: Cannot allocate memory for image\n");
        fclose(filesystem_file);
        return 1;
    }

    if (fread(filesystem_data, 1, filesystem_size, filesystem_file) != filesystem_size)
    {
        fprintf(stderr, "Error: Cannot read input image\n");
        free(filesystem_data);
        fclose(filesystem_file);
        return 1;
    }
    fclose(filesystem_file);

    // THEN ADD THE SPECIFIED FILE TO YOUR FILE SYSTEM
    uint8_t *inode_bitmap = filesystem_data + (sb.inode_bitmap_start * BS);
    uint8_t *data_bitmap = filesystem_data + (sb.data_bitmap_start * BS);
    uint8_t *inode_table = filesystem_data + (sb.inode_table_start * BS);
    uint8_t *data_region = filesystem_data + (sb.data_region_start * BS);

    uint64_t blocks_required = (input_file_stat.st_size + BS - 1) / BS;
    if (blocks_required > DIRECT_MAX)
    {
        fprintf(stderr, "Error: File too large, exceeds 12 direct blocks\n");
        free(filesystem_data);
        return 1;
    }

    uint64_t available_inode = find_free_inode(inode_bitmap, sb.inode_count);
    if (available_inode == 0)
    {
        fprintf(stderr, "Error: No free inodes available\n");
        free(filesystem_data);
        return 1;
    }

    uint64_t available_blocks = 0;
    for (uint64_t i = 0; i < sb.data_region_blocks; i++)
    {
        uint64_t byte_idx = i / 8;
        int bit_idx = i % 8;
        if (!(data_bitmap[byte_idx] & (1 << bit_idx)))
        {
            available_blocks++;
        }
    }

    if (available_blocks < blocks_required)
    {
        fprintf(stderr, "Error: Not enough free data blocks\n");
        free(filesystem_data);
        return 1;
    }

    FILE *source_file = fopen(file_add, "rb");
    if (!source_file)
    {
        fprintf(stderr, "Error: Cannot open file %s: %s\n", file_add, strerror(errno));
        free(filesystem_data);
        return 1;
    }

    inode_t new_file_inode = {
        .mode = 0100000,
        .links = 1,
        .uid = 0,
        .gid = 0,
        .size_bytes = input_file_stat.st_size,
        .atime = time(NULL),
        .mtime = input_file_stat.st_mtime,
        .ctime = time(NULL),
        .reserved_0 = 0,
        .reserved_1 = 0,
        .reserved_2 = 0,
        .proj_id = 6,
        .uid16_gid16 = 0,
        .xattr_ptr = 0};

    for (uint64_t i = 0; i < blocks_required; i++)
    {
        uint64_t available_block = find_free_data_block(data_bitmap, sb.data_region_blocks);
        if (available_block == 0)
        {
            fprintf(stderr, "Error: Cannot find free data block\n");
            fclose(source_file);
            free(filesystem_data);
            return 1;
        }

        new_file_inode.direct[i] = sb.data_region_start + available_block;
        mark_bitmap_bit(data_bitmap, available_block);

        uint8_t *block_location = data_region + (available_block * BS);
        size_t bytes_to_copy = BS;
        if (i == blocks_required - 1)
        {
            bytes_to_copy = input_file_stat.st_size - (i * BS);
        }

        size_t bytes_copied = fread(block_location, 1, bytes_to_copy, source_file);
        if (bytes_copied != bytes_to_copy)
        {
            fprintf(stderr, "Error: Cannot read file data\n");
            fclose(source_file);
            free(filesystem_data);
            return 1;
        }
    }
    fclose(source_file);

    for (int i = blocks_required; i < DIRECT_MAX; i++)
    {
        new_file_inode.direct[i] = 0;
    }

    mark_bitmap_bit(inode_bitmap, available_inode - 1);

    inode_crc_finalize(&new_file_inode);
    inode_t *inode_table_entries = (inode_t *)inode_table;
    memcpy(&inode_table_entries[available_inode - 1], &new_file_inode, sizeof(inode_t));

    inode_t *root_inode_entry = &inode_table_entries[ROOT_INO - 1];
    uint64_t root_data_block_offset = root_inode_entry->direct[0] - sb.data_region_start;

    dirent64_t *root_directory_entries = (dirent64_t *)(data_region + (root_data_block_offset * BS));
    int available_entry_slot = -1;
    int current_entry_count = 0;

    for (int i = 0; i < BS / sizeof(dirent64_t); i++)
    {
        if (root_directory_entries[i].inode_no == 0)
        {
            available_entry_slot = i;
            break;
        }
        if (root_directory_entries[i].inode_no != 0)
        {
            current_entry_count++;
        }
    }

    if (available_entry_slot == -1)
    {
        fprintf(stderr, "Error: Root directory is full\n");
        free(filesystem_data);
        return 1;
    }

    dirent64_t new_directory_entry = {
        .inode_no = available_inode,
        .type = 1};

    char *filename_only = strrchr(file_add, '/');
    if (filename_only)
    {
        filename_only++;
    }
    else
    {
        filename_only = file_add;
    }

    if (strlen(filename_only) >= 58)
    {
        fprintf(stderr, "Error: Filename too long (max 57 characters)\n");
        free(filesystem_data);
        return 1;
    }

    strcpy(new_directory_entry.name, filename_only);
    dirent_checksum_finalize(&new_directory_entry);

    memcpy(&root_directory_entries[available_entry_slot], &new_directory_entry, sizeof(dirent64_t));

    root_inode_entry->size_bytes = (current_entry_count + 1) * sizeof(dirent64_t);
    root_inode_entry->links++;
    root_inode_entry->mtime = time(NULL);
    inode_crc_finalize(root_inode_entry);

    superblock_t *superblock_ptr = (superblock_t *)filesystem_data;
    superblock_crc_finalize(superblock_ptr);

    // UPDATE THE .IMG FILE ON DISK
    FILE *output_file = fopen(out_img, "wb");
    if (!output_file)
    {
        fprintf(stderr, "Error: Cannot create output file %s: %s\n", out_img, strerror(errno));
        free(filesystem_data);
        return 1;
    }

    if (fwrite(filesystem_data, 1, filesystem_size, output_file) != filesystem_size)
    {
        fprintf(stderr, "Error: Cannot write output image\n");
        fclose(output_file);
        free(filesystem_data);
        return 1;
    }

    printf("Successfully added file %s to filesystem\n", filename_only);
    printf("File size: %ld bytes, blocks used: %lu\n", input_file_stat.st_size, blocks_required);

    fclose(output_file);
    free(filesystem_data);
    return 0;
}