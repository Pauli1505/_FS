
#include "fs_driver.h"
#include <string.h>

int main() {
    // Mount the filesystem
    fs_mount();

    // Create a file
    file_handle_t file = fs_create_file(0);  // Create a regular file

    // Write data to the file
    const char* data = "Hello, kernel world!";
    fs_write_file(&file, (const uint8_t*)data, strlen(data));

    // Read the data back
    uint8_t buffer[64];
    fs_read_file(&file, buffer, 64);
    func("Read data: %s\n", buffer); // replace func with the kernels printf version

    // Sync and close the file
    fs_sync();
    fs_close(&file);

    return 0;
}
