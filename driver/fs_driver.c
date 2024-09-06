#include "fs_driver.h"
#include "fs.h"  // Include the original filesystem header that contains your structures
#include <string.h> // For memcpy

// Function to mount the filesystem (initialize and load data from disk)
void fs_mount() {
    mount_fs();  // Call the filesystem mount function defined in fs.c
}

// Function to create a new file in the filesystem
file_handle_t fs_create_file(uint8_t file_type) {
    file_handle_t handle;
    handle.inode_idx = create_file(file_type);
    if (handle.inode_idx < 0) {
        // Error, no free inodes
        kprintf("Error: Unable to create file, no free inodes.\n");
    }
    handle.offset = 0;  // Initialize file offset to 0
    return handle;
}

// Function to write data to a file
int fs_write_file(file_handle_t* file, const uint8_t* data, int size) {
    if (!file || file->inode_idx < 0) {
        kprintf("Error: Invalid file handle\n");
        return -1;
    }

    int result = write_file(file->inode_idx, data, size);
    if (result == 0) {
        file->offset += size;  // Update file offset after writing
    }

    return result;
}

// Function to read data from a file
int fs_read_file(file_handle_t* file, uint8_t* buffer, int size) {
    if (!file || file->inode_idx < 0) {
        kprintf("Error: Invalid file handle\n");
        return -1;
    }

    inode_t* inode = &inode_table[file->inode_idx];

    // Check if the read size exceeds file size
    if (file->offset + size > inode->size) {
        size = inode->size - file->offset;  // Adjust size to available data
    }

    int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    for (int i = 0; i < blocks_needed; i++) {
        int block_idx = inode->direct_blocks[i];
        uint8_t block_buffer[BLOCK_SIZE];
        read_block(block_idx, block_buffer);

        int to_copy = size - (i * BLOCK_SIZE) > BLOCK_SIZE ? BLOCK_SIZE : size - (i * BLOCK_SIZE);
        memcpy(buffer + (i * BLOCK_SIZE), block_buffer, to_copy);
    }

    file->offset += size;  // Update offset after reading
    return size;  // Return the number of bytes read
}

// Function to sync filesystem (superblock, inodes) to disk
void fs_sync() {
    sync_fs();  // Call the filesystem sync function defined in fs.c
}

// Function to close the file handle (currently, we don't need cleanup)
void fs_close(file_handle_t* file) {
    // No special cleanup is needed for now, but we reset the file handle
    file->inode_idx = -1;
    file->offset = 0;
}
