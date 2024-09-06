#ifndef INODE_FS_H
#define INODE_FS_H

#include <stdint.h>
#include <stddef.h>

// Kernel-provided functions (to be implemented in the kernel environment)
void read_block(uint32_t block_num, uint8_t* buffer);
void write_block(uint32_t block_num, const uint8_t* buffer);
void* kmalloc(size_t size);
void kfree(void* ptr);
void kprintf(const char* format, ...); // Kernel print function for debugging

// Filesystem constants
#define BLOCK_SIZE 4096
#define NUM_BLOCKS 1024
#define NUM_INODES 128
#define INODE_SIZE sizeof(inode_t)
#define SUPERBLOCK_OFFSET 0
#define INODE_TABLE_OFFSET 1

// Superblock structure definition
typedef struct {
    uint32_t num_inodes;
    uint32_t num_blocks;
    uint32_t free_blocks;
    uint32_t free_inodes;
} superblock_t;

// Inode structure definition
typedef struct {
    uint32_t size;
    uint32_t direct_blocks[10]; // Direct block pointers
    uint8_t type;                // 0 for file, 1 for directory
} inode_t;

// Function prototypes
void read_superblock();
void write_superblock();
void load_inode_table();
void write_inode_table();
void set_bitmap(uint8_t* bitmap, int index);
void clear_bitmap(uint8_t* bitmap, int index);
int find_free_bitmap(uint8_t* bitmap, int size);
int alloc_inode();
int alloc_block();
int create_file(uint8_t file_type);
int write_file(int inode_idx, const uint8_t* data, int size);
void sync_fs();
void mount_fs();

#endif // INODE_FS_H
