#ifndef FS_DRIVER_H
#define FS_DRIVER_H

#include <stdint.h>
#include <stddef.h>

// Define a structure to represent a file handle
typedef struct {
    int inode_idx;
    uint32_t offset;
} file_handle_t;

// Function to mount the filesystem (initializes in-memory structures)
void fs_mount();

// Function to create a new file in the filesystem
file_handle_t fs_create_file(uint8_t file_type);

// Function to write data to a file
int fs_write_file(file_handle_t* file, const uint8_t* data, int size);

// Function to read data from a file
int fs_read_file(file_handle_t* file, uint8_t* buffer, int size);

// Function to sync filesystem (superblock, inodes) to disk
void fs_sync();

// Function to close the file handle (cleanup if necessary)
void fs_close(file_handle_t* file);

#endif // FS_DRIVER_H
