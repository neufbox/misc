#ifndef _NBU_FILE_H_
#define _NBU_FILE_H_

/*! Wrapper for basename.
 * \param path null-terminated pathname string
 * \return the component following the final '/'
 */
const char *nbu_file_basename(const char *path);

#endif /* _NBU_FILE_H_ */
