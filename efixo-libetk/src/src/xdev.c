#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <mntent.h>
#include <dirent.h>
#include <sys/stat.h>
#include <endian.h>
#include <byteswap.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sysmacros.h>

#include <linux/version.h>

#include <inklevel.h>

#include "etk/xdev.h"
#include "etk/log.h"
#include "etk/string.h"
#include "etk/common.h"

/*
 * Defs
 */
#define SYSFS_BLOCK_PATH "/sys/block"
#define SYSFS_LP_PATH    "/sys/class/usb"

#define EXT_SUPERBLOCK_OFFSET                   0x400
#define EXT_SUPER_MAGIC                         0xEF53

#define EXT3_FEATURE_COMPAT_HAS_JOURNAL		0x0004

#define EXT4_FEATURE_RO_COMPAT_HUGE_FILE	0x0008
#define EXT4_FEATURE_RO_COMPAT_DIR_NLINK	0x0020
#define EXT4_FEATURE_INCOMPAT_EXTENTS		0x0040
#define EXT4_FEATURE_INCOMPAT_64BIT		0x0080

#define DOSPTMAGIC                              0xaa55

enum uuid_format {
	UUID_STRING,
	UUID_HEX_STRING,
	UUID_DCE,
	UUID_DOS,
	UUID_64BIT_LE,
	UUID_64BIT_BE,
	UUID_FOURINT,
};

struct ntfs_super_block {
	uint8_t jump[3];	// 0
	uint8_t oem_id[8];	// 3
	uint16_t bytes_per_sector;	// 11
	uint8_t sectors_per_cluster;	// 13
	uint16_t reserved_sectors;	// 14
	uint8_t fats;		// 16
	uint16_t root_entries;	// 17
	uint16_t sectors;	// 19
	uint8_t media_type;	// 21
	uint16_t sectors_per_fat;	// 22
	uint16_t sectors_per_track;	// 24
	uint16_t heads;		// 26
	uint32_t hidden_sectors;	// 28
	uint32_t large_sectors;	// 32
	uint16_t unused[2];	// 36
	uint64_t number_of_sectors;	// 40
	uint64_t mft_cluster_location;	// 48
	uint64_t mft_mirror_cluster_location;	// 56
	int8_t cluster_per_mft_record;	// 64
	uint8_t reserved1[3];	// 65
	int8_t cluster_per_index_record;	// 68
	uint8_t reserved2[3];	// 69
	uint8_t volume_serial[8];	// 72
	uint16_t checksum;	// 80
} PACKED;

struct vfat_super_block {
	uint8_t boot_jump[3];	// 0
	uint8_t sysid[8];	// 3
	uint16_t sector_size;	// 11
	uint8_t sectors_per_cluster;	// 13
	uint16_t reserved;	// 14
	uint8_t fats;		// 16
	uint16_t dir_entries;	// 17
	uint16_t sectors;	// 19
	uint8_t media;		// 21
	uint16_t fat_length;	// 22
	uint16_t secs_track;	// 24
	uint16_t heads;		// 26
	uint32_t hidden;	// 28
	uint32_t total_sect;	// 32
	union {
		struct fat_super_block {
			uint8_t unknown[3];	// 36
			uint8_t serno[4];	// 39
			uint8_t label[11];	// 43
			uint8_t magic[8];	// 54
			uint8_t dummy2[192];	// 62
			uint16_t pmagic;	// 254
		} PACKED fat;
		struct fat32_super_block {
			uint32_t fat32_length;	// 36
			uint16_t flags;	// 40
			uint8_t version[2];	// 42
			uint32_t root_cluster;	// 44
			uint16_t fsinfo_sector;	// 48
			uint16_t backup_boot;	// 50
			uint16_t reserved2[6];	// 52
			uint8_t unknown[3];	// 64
			uint8_t serno[4];	// 67
			uint8_t label[11];	// 71
			uint8_t magic[8];	// 82
			uint8_t dummy2[164];	// 90
			uint16_t pmagic;	// 254
		} PACKED fat32;
	} PACKED type;
} PACKED;

struct ext2_super_block {
	uint32_t s_inodes_count;
	uint32_t s_blocks_count;
	uint32_t s_r_blocks_count;
	uint32_t s_free_blocks_count;
	uint32_t s_free_inodes_count;
	uint32_t s_first_data_block;
	uint32_t s_log_block_size;
	uint32_t s_log_frag_size;
	uint32_t s_blocks_per_group;
	uint32_t s_frags_per_group;
	uint32_t s_inodes_per_group;
	uint32_t s_mtime;
	uint32_t s_wtime;
	uint16_t s_mnt_count;
	uint16_t s_max_mnt_count;
	uint16_t s_magic;
	uint16_t s_state;
	uint16_t s_errors;
	uint16_t s_minor_rev_level;
	uint32_t s_lastcheck;
	uint32_t s_checkinterval;
	uint32_t s_creator_os;
	uint32_t s_rev_level;
	uint16_t s_def_resuid;
	uint16_t s_def_resgid;
	uint32_t s_first_ino;
	uint16_t s_inode_size;
	uint16_t s_block_group_nr;
	uint32_t s_feature_compat;
	uint32_t s_feature_incompat;
	uint32_t s_feature_ro_compat;
	uint8_t s_uuid[16];
	uint8_t s_volume_name[16];
} PACKED;

/*
 * Static declarations
 */
static int sysfs_get_string(const char *path, const char *filename,
			    char *buf, size_t buf_size);
static long long int sysfs_get_int(const char *path, const char *filename,
				   int dfl);

static int read_sb(int fd, off_t offset, char *buf, ssize_t buf_size);

static int xdev_block_slice_format_uuid(char *buf, size_t buf_size,
					int uuid_type,
					const uint8_t * uuid_raw,
					size_t uuid_raw_size);
static int xdev_slice_header_ntfs(struct xdev_block_slice_t *slice, int fd);
static int xdev_slice_header_fat(struct xdev_block_slice_t *slice, int fd);
static int xdev_slice_header_ext(struct xdev_block_slice_t *slice, int fd);
static int xdev_slice_header_info(struct xdev_block_slice_t *slice);
static int xdev_slice_mnt_info(struct xdev_block_slice_t *slice);
static int xdev_list_block_slice(struct xdev_block_t *block);

static int follow_the_link(const char *base_path, const char *linkname,
			   char *buf, size_t buf_size);

/* static defs */
static const char *inktype_list[] = {
	"NotPresent",
	"Black",
	"Color",
	"Photo",
	"Cyan",
	"Magenta",
	"Yellow",
	"PhotoBlack",
	"PhotoCyan",
	"PhotoMagenta",
	"PhotoYellow",
	"Red",
	"Green",
	"Blue",
	"LightBlack",
	"LightCyan",
	"LightMagenta",
	"LightLightBlack",
	"MatteBlack",
	"GlossOptimizer",
	"Unknown",
	"KCM",			/* Black, Cyan, Magenta */
	"GGK",			/* 2x Grey and Black */
	"KCMY",			/* Black, Cyan, Magenta, Yellow */
	"LCLM",			/* Photocyan and Photomagenta */
	"YM",			/* Yellow and Magenta */
	"CK",			/* Cyan and Black */
	"LGPK",			/* Light Grey and Photoblack */
	"LG",			/* Light Grey */
	"G",			/* Medium Grey */
	"PG",			/* Photogrey */
	"White",		/* White */
};

static struct {
	int cd;
	char *str;
} xdev_fstype_ascii_map[] = {
	{
	.cd = FSTYPE_UNKNOWN,.str = FSTYPE_UNKNOWN_STR,}, {
	.cd = FSTYPE_NTFS,.str = "HPFS/NTFS",}, {
	.cd = FSTYPE_FAT,.str = "FAT",}, {
	.cd = FSTYPE_FAT12_16,.str = "FAT12/16",}, {
	.cd = FSTYPE_FAT32,.str = "FAT32",}, {
	.cd = FSTYPE_EXT2,.str = "EXT2",}, {
	.cd = FSTYPE_EXT3,.str = "EXT3",}, {
.cd = FSTYPE_EXT4,.str = "EXT4",},};

static struct {
	int (*fct) (struct xdev_block_slice_t *, int fd);
	int fstype_mask;
} xdev_hdr_map[] = {
	{
	.fct = xdev_slice_header_ntfs,.fstype_mask = FSTYPE_NTFS,}, {
	.fct = xdev_slice_header_fat,.fstype_mask =
		    FSTYPE_FAT | FSTYPE_FAT12_16 | FSTYPE_FAT32,}, {
.fct = xdev_slice_header_ext,.fstype_mask =
		    FSTYPE_EXT2 | FSTYPE_EXT3 | FSTYPE_EXT4,},};

static struct {
	char *name;
	int type;
} xdev_fstype_map[] = {
	{
	.name = "vfat",.type = FSTYPE_FAT,}, {
	.name = "ntfs",.type = FSTYPE_NTFS,}, {
	.name = "fuseblk",.type = FSTYPE_NTFS,}, {
	.name = "ntfs-3g",.type = FSTYPE_NTFS,}, {
	.name = "ext2",.type = FSTYPE_EXT2,}, {
	.name = "ext3",.type = FSTYPE_EXT3,}, {
.name = "ext4",.type = FSTYPE_EXT4,},};

/*
 * Static definitions
 */
static int sysfs_get_string(const char *path, const char *filename,
			    char *buf, size_t buf_size)
{
	int fd = -1;
	char path_full[PATH_MAX_SIZE];
	size_t size;

	snprintf(path_full, sizeof(path_full), "%s/%s", path, filename);

	fd = open(path_full, O_RDONLY);
	if (fd < 0) {
		err("Failed to open '%s': %m", path_full);
		return -1;
	}

	size = read(fd, buf, buf_size - 1);
	dbg("read %zu byte(s) from '%s' (fd '%d')", size, path_full, fd);

	close(fd);

	buf[size] = '\0';
	strrtrim(buf);

	return 0;
}

static long long int sysfs_get_int(const char *path, const char *filename,
				   int dfl)
{
	char stri[32];
	long long int ret;

	if (sysfs_get_string(path, filename, stri, sizeof(stri)) != 0) {
		return dfl;
	}

	errno = 0;
	ret = strtoll(stri, NULL, 10);

	return (errno == 0 ? ret : dfl);
}

static int read_sb(int fd, off_t offset, char *buf, ssize_t buf_size)
{
	ssize_t read_size;

	if (lseek(fd, offset, SEEK_SET) < 0) {
		err("lseek failed: %m");
		return -1;
	}

	read_size = read(fd, buf, buf_size);
	if (read_size == -1) {
		err("Error reading %zu bytes on fd %d", buf_size, fd);
		return -1;
	}

	if (read_size != buf_size) {
		err("Request %zu bytes, got only %zx bytes.",
		    buf_size, read_size);
		return -1;
	}

	return 0;
}

static int xdev_block_slice_format_uuid(char *buf, size_t buf_size,
					int uuid_type,
					const uint8_t * uuid_raw,
					size_t uuid_raw_size)
{
	unsigned int max_cp_size = min(uuid_raw_size, buf_size);
	unsigned int i = 0;

	if (uuid_type == UUID_DOS) {
		snprintf(buf, buf_size, "%02X%02X-%02X%02X",
			 uuid_raw[3], uuid_raw[2], uuid_raw[1], uuid_raw[0]);
	} else if (uuid_type == UUID_64BIT_LE) {
		snprintf(buf, buf_size, "%02X%02X%02X%02X%02X%02X%02X%02X",
			 uuid_raw[7], uuid_raw[6], uuid_raw[5], uuid_raw[4],
			 uuid_raw[3], uuid_raw[2], uuid_raw[1], uuid_raw[0]);
	} else if (uuid_type == UUID_64BIT_BE) {
		snprintf(buf, buf_size, "%02X%02X%02X%02X%02X%02X%02X%02X",
			 uuid_raw[0], uuid_raw[1], uuid_raw[2], uuid_raw[3],
			 uuid_raw[4], uuid_raw[5], uuid_raw[6], uuid_raw[7]);
	} else if (uuid_type == UUID_DCE) {
		snprintf(buf, buf_size,
			 "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-"
			 "%02x%02x%02x%02x%02x%02x",
			 uuid_raw[0], uuid_raw[1], uuid_raw[2], uuid_raw[3],
			 uuid_raw[4], uuid_raw[5],
			 uuid_raw[6], uuid_raw[7],
			 uuid_raw[8], uuid_raw[9],
			 uuid_raw[10], uuid_raw[11], uuid_raw[12],
			 uuid_raw[13], uuid_raw[14], uuid_raw[15]);
	} else if (uuid_type == UUID_HEX_STRING) {
		/* translate A..F to a..f */
		memcpy(buf, uuid_raw, max_cp_size);
		for (i = 0; i < max_cp_size; i++) {
			if (buf[i] >= 'A' && buf[i] <= 'F') {
				buf[i] = (buf[i] - 'A') + 'a';
			}
		}
		buf[max_cp_size] = '\0';
	} else if (uuid_type == UUID_STRING) {
		memcpy(buf, uuid_raw, max_cp_size);
		buf[max_cp_size] = '\0';
	} else if (uuid_type == UUID_FOURINT) {
		snprintf(buf, buf_size,
			 "%02x%02x%02x%02x:%02x%02x%02x%02x:"
			 "%02x%02x%02x%02x:%02x%02x%02x%02x",
			 uuid_raw[0], uuid_raw[1], uuid_raw[2], uuid_raw[3],
			 uuid_raw[4], uuid_raw[5], uuid_raw[6], uuid_raw[7],
			 uuid_raw[8], uuid_raw[9], uuid_raw[10], uuid_raw[11],
			 uuid_raw[12], uuid_raw[13],
			 uuid_raw[14], uuid_raw[15]);
	} else {
		err("Unknown UUID type %d !", uuid_type);
		return -1;
	}

	return 0;
}

static int xdev_slice_header_ntfs(struct xdev_block_slice_t *slice, int fd)
{
	char buf[512];
	struct ntfs_super_block *sb = NULL;
	int ret = 1;

	if (read_sb(fd, 0, buf, sizeof(buf)) != 0) {
		return -1;
	}

	sb = (struct ntfs_super_block *)buf;

	if (sb->oem_id[0] == 'N' && sb->oem_id[1] == 'T'
	    && sb->oem_id[2] == 'F' && sb->oem_id[3] == 'S') {
		/* I guess it's really a NTFS partition */

		/* fstype */
		slice->fstype = FSTYPE_NTFS;

		/* uuid */
		xdev_block_slice_format_uuid(slice->uuid,
					     sizeof(slice->uuid),
					     UUID_64BIT_LE,
					     sb->volume_serial,
					     sizeof(sb->volume_serial));
		ret = 0;
	}

	return ret;
}

static int xdev_slice_header_fat(struct xdev_block_slice_t *slice, int fd)
{
	char buf[1024];
	struct vfat_super_block *sb = NULL;
	int ret = 1;
	int fstype = FSTYPE_UNKNOWN;
	uint16_t fat_length = 0;
	uint32_t fat32_length = 0;

	unsigned long nsecs = 0;
	char label[sizeof(sb->type.fat.label)];
	uint8_t *serno = NULL;

	if (read_sb(fd, 0, buf, sizeof(buf)) != 0) {
		return -1;
	}

	sb = (struct vfat_super_block *)buf;

	if (sb->boot_jump[0] == 0xeb
	    && sb->boot_jump[2] == 0x90
	    && (sb->media == 0xf8 || sb->media == 0xfc)) {
		/* check DOSPTMAGIC at 0x510 */
		if (*((unsigned short *)buf + 255) != le16(DOSPTMAGIC)) {
			dbg("Not found DOS MAGIC signature in %s",
			    slice->devpath);
			return 1;
		}

		nsecs = le16(sb->sectors);
		if (nsecs == 0) {
			nsecs = le32(sb->total_sect);
		}
		if (nsecs == 0) {
			dbg("Invalid number of sectors in %s", slice->devpath);
			return 1;
		}

		dbg("buf[0x39] = %c, buf[0x3a] = %c", buf[0x39], buf[0x3a]);
		if ((buf[0x39] == '1' && buf[0x3a] == '2')
		    || (buf[0x39] == '1' && buf[0x3a] == '6')) {
			fstype = FSTYPE_FAT12_16;
		}

		fat_length = le16(sb->fat_length);
		fat32_length = le32(sb->type.fat32.fat32_length);
		dbg("fat_length = 0x%x (0x%x), fat32_length = 0x%x (0x%x)",
		    fat_length, sb->fat_length,
		    fat32_length, sb->type.fat32.fat32_length);
		if (fat_length == 0 && fat32_length != 0) {
			fstype = FSTYPE_FAT32;
		} else {
			/* trust usbkey, it's by default an old fat partition */
			fstype = FSTYPE_FAT12_16;
		}

		if (fstype == FSTYPE_FAT32 && *(buf + 0x26) == 0x29) {
			dbg("FAT32 && *(buf + 0x26) == 0x29");
			return 1;
		}

		if (fstype != FSTYPE_FAT32 && *(buf + 0x26) != 0x29) {
			dbg("!= FAT32 && *(buf + 0x26) != 0x29");
			return 1;
		}

		if (fstype != FSTYPE_FAT32 && fstype != FSTYPE_FAT12_16) {
			dbg("Not FAT partition type 0x%x (FAT32=0x%x, FAT1216=0x%x)", fstype, FSTYPE_FAT32, FSTYPE_FAT12_16);
			return 1;
		}

		/* I guess its really a FAT partition */
		if (fstype == FSTYPE_FAT32) {
			/* label */
			memcpy(label, sb->type.fat32.label, sizeof(label));

			/* uuid */
			serno = sb->type.fat32.serno;
		} else {
			/* label */
			memcpy(label, sb->type.fat.label, sizeof(label));

			/* uuid */
			serno = sb->type.fat.serno;
		}

		/* fstype */
		slice->fstype = fstype;

		/* label */
		snprintf(slice->label, sizeof(slice->label), "%s", label);

		/* uuid */
		xdev_block_slice_format_uuid(slice->uuid,
					     sizeof(slice->uuid),
					     UUID_DOS,
					     serno, sizeof(sb->type.fat.serno));

		ret = 0;
	}

	return ret;
}

static int xdev_slice_header_ext(struct xdev_block_slice_t *slice, int fd)
{
	char buf[512];
	struct ext2_super_block *sb = NULL;
	int ret = 1;

	if (read_sb(fd, EXT_SUPERBLOCK_OFFSET, buf, sizeof(buf)) != 0) {
		return -1;
	}

	sb = (struct ext2_super_block *)buf;

	if (sb->s_magic == le16(EXT_SUPER_MAGIC)) {
		if (sb->s_free_blocks_count >= sb->s_blocks_count) {
			dbg("sb->s_free_blocks_count >= sb->s_blocks_count");
			return 1;
		}

		if (sb->s_free_inodes_count >= sb->s_inodes_count) {
			dbg("sb->s_free_inodes_count >= sb->s_inodes_count");
			return 1;
		}

		if (sb->s_blocks_count == 0) {
			dbg("Block count is 0 !");
			return 1;
		}

		/* fstype */
		if (le32(sb->s_feature_compat)
		    & EXT3_FEATURE_COMPAT_HAS_JOURNAL) {
			/* not a ext2 */
			if ((le32(sb->s_feature_ro_compat)
			     & EXT4_FEATURE_RO_COMPAT_HUGE_FILE)
			    || (le32(sb->s_feature_ro_compat)
				& EXT4_FEATURE_RO_COMPAT_DIR_NLINK)
			    || (le32(sb->s_feature_incompat)
				& EXT4_FEATURE_INCOMPAT_EXTENTS)
			    || (le32(sb->s_feature_incompat)
				& EXT4_FEATURE_INCOMPAT_64BIT)) {
				slice->fstype = FSTYPE_EXT4;
			} else {
				slice->fstype = FSTYPE_EXT3;
			}
		} else {
			slice->fstype = FSTYPE_EXT2;
		}

		/* label */
		snprintf(slice->label, min(sizeof(slice->label), sizeof(sb->s_volume_name)), "%s", (char *)sb->s_volume_name);

		/* uuid */
		xdev_block_slice_format_uuid(slice->uuid,
					     sizeof(slice->uuid),
					     UUID_DCE,
					     sb->s_uuid, sizeof(sb->s_uuid));

		ret = 0;
	}

	return ret;
}

static int xdev_slice_header_info(struct xdev_block_slice_t *slice)
{
	int fd;
	int found = 0, ret = -1;
	unsigned int i = 0;

	fd = open(slice->devpath, O_RDONLY);
	if (fd < 0) {
		err("Error opening %s: %m", slice->devpath);
		return -1;
	}

	if (slice->fstype == FSTYPE_UNKNOWN) {
		/* need to guess slice type */
		for (i = 0; i < ARRAY_SIZE(xdev_hdr_map); i++) {
			if ((ret = xdev_hdr_map[i].fct(slice, fd)) == 0) {
				found = 1;
				break;
			}
		}

		if (!found) {
			err("No found fstype of %s", slice->devpath);
		}
	} else {
		for (i = 0; i < ARRAY_SIZE(xdev_hdr_map); i++) {
			/* dbg("0x%X & 0x%X = 0x%X",  */
			/*     slice->fstype, xdev_hdr_map[i].fstype_mask, */
			/*     slice->fstype & xdev_hdr_map[i].fstype_mask); */
			if (slice->fstype & xdev_hdr_map[i].fstype_mask) {
				ret = xdev_hdr_map[i].fct(slice, fd);

				found = 1;
				break;
			}
		}

		if (!found) {
			err("Invalid fstype of %s (fstype code=%d)",
			    slice->devpath, slice->fstype);
		} else if (ret != 0) {
			err("Error reading header of %s (fstype=%s)",
			    slice->devpath, xdev_fstype_ctoa(slice->fstype));
		}
	}

	close(fd);

	return (found && ret == 0 ? 0 : -1);
}

static int xdev_slice_mnt_info(struct xdev_block_slice_t *slice)
{
	FILE *mount_table = NULL;
	struct mntent *mount_entry = NULL;
	struct statfs dev_statfs;
	int ret = 1;

	unsigned int i = 0;

	mount_table = setmntent("/proc/mounts", "r");
	if (mount_table == NULL) {
		err("Error reading mount table: %m");
		return -1;
	}

	/* read mount table */
	while ((mount_entry = getmntent(mount_table)) != NULL) {
		if (strcmp(mount_entry->mnt_fsname, slice->devpath) == 0) {
			/* mounted */
			slice->mounted = 1;

			/* mnt path */
			snprintf(slice->mntpath, sizeof(slice->mntpath), "%s",
				mount_entry->mnt_dir);

			/* capacities */
			if (statfs(mount_entry->mnt_dir, &dev_statfs) == 0) {
				slice->capacity =
				    ((long long int)dev_statfs.f_blocks *
				     dev_statfs.f_bsize);
				slice->capacity_used =
				    ((long long int)dev_statfs.f_blocks -
				     dev_statfs.f_bfree) * dev_statfs.f_bsize;

				dbg("mnt slice %s (capacity: %lld, "
				    "capacity used: %lld)",
				    slice->mntpath,
				    slice->capacity, slice->capacity_used);
				dbg("mnt slice %s (dev_statfs.f_blocks = %ld, "
				    "dev_statfs.f_bfree = %ld, "
				    "dev_statfs.f_bsize = %ld)",
				    slice->mntpath,
				    dev_statfs.f_blocks,
				    dev_statfs.f_bfree, dev_statfs.f_bsize);
			}

			/* fstype */
			/*
			 * here, there a poor information about fstype. So
			 * use if we are no fstype yet.
			 */
			if (slice->fstype == FSTYPE_UNKNOWN) {
				for (i = 0;
				     i < ARRAY_SIZE(xdev_fstype_map); i++) {
					if (strcmp(xdev_fstype_map[i].name,
						   mount_entry->mnt_type) == 0)
					{
						slice->fstype =
						    xdev_fstype_map[i].type;

						break;
					}
				}
			}

			ret = 0;

			break;
		}
	}

	endmntent(mount_table);

	return ret;
}

static int xdev_list_block_slice(struct xdev_block_t *block)
{
	struct dirent *dir_read = NULL;
	DIR *dir = NULL;
	size_t len;
	struct stat sb;

	char syspath_buf[PATH_MAX_SIZE];

	struct xdev_block_slice_t *slice = NULL;

	INIT_LIST_HEAD(&(block->slice_list));

	dir = opendir(block->syspath);
	if (dir == NULL) {
		err("Unable to opendir ! %s", strerror(errno));
		return -1;
	}

	while ((dir_read = readdir(dir))) {
		if (matches(dir_read->d_name, ".")
		    || matches(dir_read->d_name, "..")) {
			continue;
		}

		/* try to find if its a partition */
		snprintf(syspath_buf, sizeof(syspath_buf),
			 "%s/%s", block->syspath, dir_read->d_name);

		if (stat(syspath_buf, &sb) != 0) {
			err("stat error ! %s", strerror(errno));
			continue;
		}

		/* not the good way */
		if (!S_ISDIR(sb.st_mode))
			continue;

		/* child partition directory ? */
		len = strlen(block->name);
		if ((len == 0) || (strncmp(dir_read->d_name, block->name, len) != 0))
			continue;

		/* snprintf(path_buf, sizeof(path_buf), */
		/*          "%s/partition", syspath_buf); */

		/* if (stat(path_buf, &sb) != 0)  */
		/* { */
		/*         dbg("Not a partition ! %s", strerror(errno)); */
		/*         continue; */
		/* } */

		/* Here, We have found a partition */
		slice = calloc(1, sizeof(struct xdev_block_slice_t));

		/* set name */
		snprintf(slice->name, sizeof(slice->name), "%s", dir_read->d_name);

		/* set syspath */
		snprintf(slice->syspath, sizeof(slice->syspath), "%s", syspath_buf);

		/* compute devpath */
		snprintf(slice->devpath, sizeof(slice->devpath),
			 "/dev/%s", dir_read->d_name);

		/* compute capacity (size value is exprimed to 512B sector) */
		slice->capacity = sysfs_get_int(syspath_buf, "size", 0) * 512;
		dbg("slice %s (capacity: %lld)", slice->name, slice->capacity);

		/* get mount information (if mounted)
		 * Call mnt_info fct before header_info because mount info 
		 * can give use some first informations about fstype 
		 */
		xdev_slice_mnt_info(slice);

		/* get informations in partition header */
		if (xdev_slice_header_info(slice) != 0) {
			err("Error getting header info on %s", slice->devpath);
			free(slice);
			continue;
		}

		/* add to list */
		list_add_tail(&(slice->node), &(block->slice_list));
	}

	closedir(dir);

	if (list_empty(&(block->slice_list))) {
		/* Some usbkeys haven't partitions table and store directly
		 * data at start. 
		 *
		 * Suppose we have this case and try to get informations.
		 */
		slice = calloc(1, sizeof(struct xdev_block_slice_t));

		/* set name */
		snprintf(slice->name, sizeof(slice->name), "%s", block->name);

		/* set syspath */
		snprintf(slice->syspath, sizeof(slice->syspath), "%s", block->syspath);

		/* set devpath */
		snprintf(slice->devpath, sizeof(slice->devpath), "%s",  block->devpath);

		/* set capacity */
		slice->capacity = block->capacity;

		/* get mount information about this slice 
		 * Call mnt_info fct before header_info because mount info 
		 * can give use some first informations about fstype 
		 */
		xdev_slice_mnt_info(slice);

		/* get informations in partition super block */
		if (xdev_slice_header_info(slice) == 0) {
			/* it's seems there one partition on this block !
			 * add to list 
			 */
			list_add_tail(&(slice->node), &(block->slice_list));
		} else {
			free(slice);
		}
	}

	return 0;
}

static int follow_the_link(const char *base_path, const char *linkname,
			   char *buf, size_t buf_size)
{
	char path_buf[PATH_MAX_SIZE];
	char fullpath_buf[PATH_MAX_SIZE];
	ssize_t len;
	struct stat sb;

	snprintf(path_buf, sizeof(path_buf), "%s/%s", base_path, linkname);

	if (lstat(path_buf, &sb) != 0) {
		err("stat error on '%s' ! %s", path_buf, strerror(errno));
		return -1;
	}

	if (S_ISLNK(sb.st_mode)) {
		len = readlink(path_buf,
			       fullpath_buf, sizeof(fullpath_buf) - 1);
		if (len <= 0) {
			err("Error readlink '%s': %m", path_buf);
			return -1;
		}
		fullpath_buf[len] = '\0';

		snprintf(buf, buf_size, "%s/%s", base_path, fullpath_buf);
	} else {
		err("It must be a symbolic link %s !", path_buf);
		return -1;
	}

	return 0;
}

/*
 * Public definitions
 */
int xdev_list_block(struct list_head *block_list)
{
	struct dirent *dir_read = NULL;
	DIR *dir = NULL;

	char syspath_buf[PATH_MAX_SIZE];
	char buf[64];

	struct xdev_block_t *block = NULL;

	INIT_LIST_HEAD(block_list);

	dir = opendir(SYSFS_BLOCK_PATH);
	if (dir == NULL) {
		err("Unable to opendir ! %s", strerror(errno));
		return -1;
	}

	while ((dir_read = readdir(dir))) {
		if (matches(dir_read->d_name, ".")
		    || matches(dir_read->d_name, ".."))
			continue;

		/* only deal with sd* block */
		if (strncmp(dir_read->d_name, "sd", 2) != 0)
			continue;

		/* compute syspath */
#ifdef NB6
		if (follow_the_link(SYSFS_BLOCK_PATH, dir_read->d_name,
				    syspath_buf, sizeof(syspath_buf)) != 0)
			continue;
#else
		char path_buf[PATH_MAX_SIZE];
		char full_path_buf[PATH_MAX_SIZE];
		snprintf(path_buf, sizeof(path_buf),
			 "%s/%s", SYSFS_BLOCK_PATH, dir_read->d_name);
		if (follow_the_link(path_buf, "device",
				    full_path_buf, sizeof(full_path_buf)) != 0)
			continue;
		snprintf(syspath_buf, sizeof(syspath_buf),
			 "%s/block:%s", full_path_buf, dir_read->d_name);
#endif

		block = calloc(1, sizeof(struct xdev_block_t));

		/* set name */
		snprintf(block->name, sizeof(block->name), "%s", dir_read->d_name);

		/* set syspath */
		snprintf(block->syspath, sizeof(block->syspath), "%s", syspath_buf);

		/* compute devpath */
		snprintf(block->devpath, sizeof(block->devpath),
			 "/dev/%s", dir_read->d_name);

		/* set vendor */
		if (sysfs_get_string(syspath_buf, "device/vendor",
				     buf, sizeof(buf)) == 0) {
			snprintf(block->vendor, sizeof(block->vendor), "%s", buf);
		}

		/* set model */
		if (sysfs_get_string(syspath_buf, "device/model",
				     buf, sizeof(buf)) == 0) {
			snprintf(block->model, sizeof(block->model), "%s", buf);
		}

		/* compute capacity (size value is exprimed to 512B sector) */
		block->capacity = sysfs_get_int(syspath_buf, "size", 0) * 512;

		/* set slices */
		xdev_list_block_slice(block);

		/* add to list */
		list_add_tail(&(block->node), block_list);
	}

	closedir(dir);

	return 0;
}

void xdev_free_list_block(struct list_head *block_list)
{
	struct xdev_block_t *block = NULL, *safe_block = NULL;
	struct xdev_block_slice_t *slice = NULL, *safe_slice = NULL;

	list_for_each_entry_safe(block, safe_block, block_list, node) {
		list_for_each_entry_safe(slice, safe_slice,
					 &(block->slice_list), node) {
			list_del(&(slice->node));
			free(slice);
		}

		list_del(&(block->node));
		free(block);
	}
}

const char *xdev_fstype_ctoa(int fstype)
{
	unsigned int i = 0;

	for (i = 0; i < ARRAY_SIZE(xdev_fstype_ascii_map); i++) {
		if (xdev_fstype_ascii_map[i].cd == fstype)
			return xdev_fstype_ascii_map[i].str;
	}

	return FSTYPE_UNKNOWN_STR;
}

static int xdev_printer_ink_level(struct xdev_lp_t *lp)
{
	unsigned int j;
	struct stat sb;
	struct ink_level *level = NULL;
	int result;
	unsigned int inktype_index;
	struct xdev_lp_cartridge_t *lpcr = NULL;

	INIT_LIST_HEAD(&(lp->cartridge_list));

	if (stat(lp->devpath, &sb) != 0) {
		err("stat error on %s ! %s", lp->devpath, strerror(errno));
		return -1;
	}

	if (!S_ISCHR(sb.st_mode)) {
		err("%s not seems to be a caracter device...", lp->devpath);
		return -1;
	}

	level = calloc(1, sizeof(struct ink_level));

	result = get_ink_level(CUSTOM_USB, lp->devpath, 0, level);
	if (result == OK && level->status == RESPONSE_VALID) {
		for (j = 0; j < MAX_CARTRIDGE_TYPES; j++) {
			inktype_index = level->levels[j][INDEX_TYPE];
			if (inktype_index == CARTRIDGE_NOT_PRESENT) {
				break;
			}

			if (inktype_index < ARRAY_SIZE(inktype_list)) {
				lpcr = calloc(1, sizeof(*lpcr));

				/* set name */
				snprintf(lpcr->name, sizeof(lpcr->name), "%s",
					inktype_list[inktype_index]);

				/* set level */
				lpcr->level = level->levels[j][INDEX_LEVEL];

				list_add_tail(&(lpcr->node),
					      &(lp->cartridge_list));
			} else {
				err("Unknown index ink type '%d'.",
				    inktype_index);
			}
		}
	} else {
		err("Unable to get ink level " "(inkerrno='%d')", result);
	}

	free(level);

	return 0;
}

int xdev_list_printer(struct list_head *lp_list)
{
	struct dirent *dir_read = NULL;
	DIR *dir = NULL;
	struct stat sb;
	ssize_t len;

	char syspath_buf[PATH_MAX_SIZE];
	char path_buf[PATH_MAX_SIZE];
	char buf[64];

	struct xdev_lp_t *lp = NULL;

	INIT_LIST_HEAD(lp_list);

	dir = opendir(SYSFS_LP_PATH);
	if (dir == NULL) {
		dbg("%s(%s)", "opendir", SYSFS_LP_PATH);
		return 0;
	}

	while ((dir_read = readdir(dir))) {
		if (matches(dir_read->d_name, ".")
		    || matches(dir_read->d_name, "..")) {
			continue;
		}

		snprintf(syspath_buf, sizeof(syspath_buf),
			 "%s/%s", SYSFS_LP_PATH, dir_read->d_name);

		/* only deal with lp* usb class */
		if (strncmp(dir_read->d_name, "lp", 2) != 0) {
			dbg("No deal with %s", dir_read->d_name);
			continue;
		}

		if (lstat(syspath_buf, &sb) != 0) {
			err("stat error on '%s' ! %s",
			    syspath_buf, strerror(errno));
			continue;
		}

		if (S_ISLNK(sb.st_mode)) {
			len = readlink(syspath_buf,
				       path_buf, sizeof(path_buf) - 1);
			if (len <= 0) {
				err("Error readlink '%s': %m", syspath_buf);
				continue;
			}
			path_buf[len] = '\0';

			snprintf(syspath_buf, sizeof(syspath_buf),
				 "%s/%s", SYSFS_LP_PATH, path_buf);
		}

		lp = calloc(1, sizeof(struct xdev_lp_t));

		/* set name */
		snprintf(lp->name, sizeof(lp->name), "%s", dir_read->d_name);

		/* set syspath */
		snprintf(lp->syspath, sizeof(lp->syspath), "%s", syspath_buf);

		/* set devpath */
		snprintf(lp->devpath, sizeof(lp->devpath),
			 "/dev/usb/%s", dir_read->d_name);

		/* set vendor */
		if (sysfs_get_string(syspath_buf, "device/../manufacturer",
				     buf, sizeof(buf)) == 0)
			snprintf(lp->vendor, sizeof(lp->vendor), "%s", buf);

		/* set model */
		if (sysfs_get_string(syspath_buf, "device/../product",
				     buf, sizeof(buf)) == 0)
			snprintf(lp->model, sizeof(lp->model), "%s", buf);

		/* set version */
		if (sysfs_get_string(syspath_buf, "device/../version",
				     buf, sizeof(buf)) == 0)
			snprintf(lp->version, sizeof(lp->version), "%s", buf);

		/* get ink level */
		xdev_printer_ink_level(lp);

		/* add to list */
		list_add_tail(&(lp->node), lp_list);
	}

	closedir(dir);

	return 0;

}

void xdev_free_list_printer(struct list_head *lp_list)
{
	struct xdev_lp_t *lp = NULL, *safe_lp = NULL;
	struct xdev_lp_cartridge_t *cartridge = NULL, *safe_cartridge = NULL;

	list_for_each_entry_safe(lp, safe_lp, lp_list, node) {
		list_for_each_entry_safe(cartridge, safe_cartridge,
					 &(lp->cartridge_list), node) {
			list_del(&(cartridge->node));
			free(cartridge);
		}

		list_del(&(lp->node));
		free(lp);
	}
}
