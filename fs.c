#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Assume these functions are provided by the kernel
void read_block(uint32_t block_num, uint8_t* buffer);
void write_block(uint32_t block_num, const uint8_t* buffer);
void* kmalloc(size_t size);
void kfree(void* ptr);
void kprintf(const char* format, ...); // Kernel print function for debugging

#define BLOCK_SIZE 4096
#define NUM_BLOCKS 1024
#define NUM_INODES 128
#define INODE_SIZE sizeof(inode_t)
#define SUPERBLOCK_OFFSET 0
#define INODE_TABLE_OFFSET 1

// Superblock structure
typedef struct {
    uint32_t num_inodes;
    uint32_t num_blocks;
    uint32_t free_blocks;
    uint32_t free_inodes;
} superblock_t;

// Inode structure
typedef struct {
    uint32_t size;
    uint32_t direct_blocks[10]; // Direct block pointers
    uint8_t type;                // 0 for file, 1 for directory
} inode_t;

// Block bitmap for tracking free blocks
uint8_t block_bitmap[NUM_BLOCKS / 8];
uint8_t inode_bitmap[NUM_INODES / 8];

// Global superblock and inode table
superblock_t superblock;
inode_t* inode_table;  // Pointer to the inode table loaded into memory

// Function to read the superblock from disk
void read_superblock() {
    uint8_t buffer[BLOCK_SIZE];
    read_block(SUPERBLOCK_OFFSET, buffer);
    superblock = *((superblock_t*)buffer); // Load the superblock into memory
}

// Function to write the superblock to disk
void write_superblock() {
    uint8_t buffer[BLOCK_SIZE];
    *((superblock_t*)buffer) = superblock;
    write_block(SUPERBLOCK_OFFSET, buffer);
}

// Function to load the inode table into memory
void load_inode_table() {
    inode_table = (inode_t*)kmalloc(NUM_INODES * INODE_SIZE);
    for (int i = 0; i < (NUM_INODES * INODE_SIZE) / BLOCK_SIZE; i++) {
        read_block(INODE_TABLE_OFFSET + i, (uint8_t*)inode_table + (i * BLOCK_SIZE));
    }
}

// Function to write the inode table back to disk
void write_inode_table() {
    for (int i = 0; i < (NUM_INODES * INODE_SIZE) / BLOCK_SIZE; i++) {
        write_block(INODE_TABLE_OFFSET + i, (uint8_t*)inode_table + (i * BLOCK_SIZE));
    }
}

// Helper function to set a bit in a bitmap
void set_bitmap(uint8_t* bitmap, int index) {
    bitmap[index / 8] |= (1 << (index % 8));
}

// Helper function to clear a bit in a bitmap
void clear_bitmap(uint8_t* bitmap, int index) {
    bitmap[index / 8] &= ~(1 << (index % 8));
}

// Helper function to find the first free bit in a bitmap
int find_free_bitmap(uint8_t* bitmap, int size) {
    for (int i = 0; i < (int)size; i++) {  // Cast size to int for comparison
        if ((bitmap[i / 8] & (1 << (i % 8))) == 0) {
            return i;
        }
    }
    return -1; // No free space
}

// Function to allocate a free inode
int alloc_inode() {
    int inode_idx = find_free_bitmap(inode_bitmap, NUM_INODES);
    if (inode_idx >= 0) {
        set_bitmap(inode_bitmap, inode_idx);
        superblock.free_inodes--;
        return inode_idx;
    }
    return -1;  // No free inodes
}

// Function to allocate a free block
int alloc_block() {
    int block_idx = find_free_bitmap(block_bitmap, NUM_BLOCKS);
    if (block_idx >= 0) {
        set_bitmap(block_bitmap, block_idx);
        superblock.free_blocks--;
        return block_idx;
    }
    return -1;  // No free blocks
}

// Function to create a file
int create_file(uint8_t file_type) {
    int inode_idx = alloc_inode();
    if (inode_idx < 0) {
        kprintf("No free inodes!\n");
        return -1;
    }

    inode_t* inode = &inode_table[inode_idx];
    inode->size = 0;
    inode->type = file_type;

    return inode_idx;
}

// Function to write data to a file
int write_file(int inode_idx, const uint8_t* data, int size) {
    inode_t* inode = &inode_table[inode_idx];
    int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int i = 0; i < blocks_needed; i++) {
        int block_idx = alloc_block();
        if (block_idx < 0) {
            kprintf("No free blocks!\n");
            return -1;
        }
        write_block(block_idx, data + i * BLOCK_SIZE);
        inode->direct_blocks[i] = block_idx;
    }

    inode->size = size;
    return 0;
}

// Function to sync the filesystem (superblock, inodes) to disk
void sync_fs() {
    write_superblock();
    write_inode_table();
}

// Function to mount the filesystem (read from disk into memory)
void mount_fs() {
    read_superblock();
    load_inode_table();
}

int main() {
    // Initialize filesystem (just an example of how it would run)
    mount_fs();  // Mount filesystem from disk

    // Example: Create a file and write to it
    int inode_idx = create_file(0);  // Create a regular file
    if (inode_idx >= 0) {
        const char* data = "Hello, kernel world!";
        write_file(inode_idx, (const uint8_t*)data, strlen(data));
        sync_fs();  // Sync the filesystem to disk
    }

    return 0;
}
