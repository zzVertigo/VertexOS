#include <system/filesystem.h>

fs_node_t *fs_root = 0;

u32 read_fs(fs_node_t *node, u32 offset, u32 size, u8 *buffer) {
    if (node->read != NULL)
        return node->read(node, offset, size, buffer);
    else
        return NULL;
}

u32 write_fs(fs_node_t *node, u32 offset, u32 size, u8 *buffer) {
    if (node->write != NULL)
        return node->write(node, offset, size, buffer);
    else
        return NULL;
}

void open_fs(fs_node_t *node, u8 read, u8 write) {
    if (node->open != NULL)
        return node->open(node);
}

void close_fs(fs_node_t *node) {
    if (node->close != NULL)
        return node->close(node);
}

struct dirent *readdir_fs(fs_node_t *node, u32 index) {
    if ((node->flags & 0x7) == FS_DIRECTORY && node->readdir != NULL) 
        return node->readdir(node, index);
    else
        return NULL;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name) {
    if ((node->flags & 0x7) == FS_DIRECTORY && node->finddir != NULL)
        return node->finddir(node, name);
    else
        return NULL;
}