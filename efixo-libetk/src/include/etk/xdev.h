#ifndef _ETK_XDEV_H_
#define _ETK_XDEV_H_

#include <string.h>

#include <sys/vfs.h>

#include "etk/list.h"

#define PATH_MAX_SIZE			256
#define UUID_MAX_SIZE                   37

#define FSTYPE_UNKNOWN_STR "Unknown"

#define FSTYPE_UNKNOWN     0x00
#define FSTYPE_NTFS        0x01 << 0
#define FSTYPE_FAT         0x01 << 1
#define FSTYPE_FAT12_16    0x01 << 2
#define FSTYPE_FAT32       0x01 << 3
#define FSTYPE_EXT2        0x01 << 4
#define FSTYPE_EXT3        0x01 << 5
#define FSTYPE_EXT4        0x01 << 6

struct xdev_block_t {
        char name[16];
        char syspath[PATH_MAX_SIZE]; /* /sys/... */
        char devpath[32];            /* /dev/... */
        char vendor[128];
        char model[128];
        long long int capacity;      /* size in Bytes */
        struct list_head slice_list;
        struct list_head node;
};

struct xdev_block_slice_t {
        char name[16];
        char label[32];
        char syspath[PATH_MAX_SIZE]; /* /sys/.. */
        char devpath[32];            /* /dev/.. */
        int mounted;                 /* 0 or 1 */
        int fstype;                  /* type of slice (vfat, ...) */
        char uuid[UUID_MAX_SIZE];    /* uuid */
        char mntpath[32];            /* /mnt/.. */
        long long int capacity;      /* size in Bytes */
        long long int capacity_used; /* size in Bytes */
        struct list_head node;
};

struct xdev_lp_t {
        char name[32];
        char syspath[PATH_MAX_SIZE]; /* /sys/.. */
        char devpath[32];
        char vendor[128];
        char model[128];
        char version[16];
        struct list_head cartridge_list;
        struct list_head node;
};

struct xdev_lp_cartridge_t {
        char name[128];
        unsigned int level;
        struct list_head node;
};

/*
 * Get a fresh list of block typed xdev_block_t
 *
 * @return 0 if success, -1 otherwise
 */
int xdev_list_block(struct list_head *block_list);

/*
 * cleanup list return by xdev_list_block fct
 */
void xdev_free_list_block(struct list_head *block_list);

/*
 * Get human comprehensive string of fstype code
 */
const char *xdev_fstype_ctoa(int fstype);

/*
 * Get a fresh list of printer typed xdev_lp_t
 *
 * @return 0 if success, -1 otherwise
 */
int xdev_list_printer(struct list_head *lp_list);

/*
 * cleanup list return by xdev_list_printer fct
 */
void xdev_free_list_printer(struct list_head *lp_list);

/*int xdev_list_printer(void);*/

#endif
