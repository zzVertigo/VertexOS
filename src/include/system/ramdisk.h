#pragma once

#include <runtime.h>

#include <system/filesystem.h>
#include <system/kheap.h>
#include <system/drivers/display.h>
#include <system/chelpers.h>

typedef struct {
    u32 nfiles;
} initrd_header_t;

typedef struct {
    u8 magic;
    s8 name[128];
    u32 offset;
    u32 length;
} initrd_file_header_t;

fs_node_t *init_fs(u32 address);