#ifndef _NBL_UTILS_H_
#define _NBL_UTILS_H_

typedef struct nbl_utils_ltime {
	int sec;
	int min;
	int hour;
	int day;
} nbl_utils_ltime_t;

/*
 * box is upgrading ? NBU_SUCCESS if yes, otherwise NBU_ERROR
 */
extern int nbl_utils_box_upgrading(void);

/*
 * reboot the box please
 */
extern int nbl_utils_box_reboot(void);

/*
 * transform a timestamp to ltime_t structure
 */
extern int nbl_utils_timestamp2ltime(int timestamp, nbl_utils_ltime_t* ltm);

/*
 * cut string in 2 parts
 */
extern int nbl_utils_strcut(const char *str,
                            char cr,
                            char *buf1, size_t buf1_size,
                            char *buf2, size_t buf2_size);

#endif
