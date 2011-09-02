#ifndef _DBL_H_
#define _DBL_H_

#include <ewf/ewf_hdf.h>

#include <nbd/nvram.h>

#include <nbu/file.h>
#include <nbu/log.h>

struct trx {
        int id;
        short int sum_set_stat; /* 0 => ok, -1 => a set error has occured */
};

/*
 * init trx structure
 * -> create a cursor and set trx flag
 * -> if failed, set Form.Error = "system" and replay query vars
 * @return 0 if success, -1 otherwise
 */
static inline int __nv_trx_init(struct trx *trx)
{
        int ret = 0;

        trx->id = nbd_user_config_cursor_new();
        trx->sum_set_stat = 0;
        if(trx->id <= 0)
        {
                nbu_log_error("Unable to get valid session");
                ewf_hdf_query_error("system");
                ret = -1;
        }

        return ret;
}

/*
 * Set a nv var throw trx
 * -> call nbd_user_config_cset
 * -> if NV_SET_INVALID_VALUE, var_error(value) hdf var, set trx flag to -1
 *      and replay query vars
 * -> if failed, set Form.Error = "system" and replay query vars
 * @return NV_SET_SUCCESS if success, NV_SET_INVALID_VALUE if invalid value
 *         NV_SET_ERROR otherwise
 */
static inline int __nv_set_with_trx(struct trx *trx,
                                    const char *nvname,
                                    const char *hdfname,
                                    const char *valuevar)
{
        int ret;

        ret = nbd_user_config_cset(trx->id, nvname, valuevar);
        if(ret == NV_SET_SUCCESS || ret == NV_SET_UNCHANGED)
        {
                nbu_log_debug("Set nv %s = %s OK",
                              nvname, valuevar);
                ret = NV_SET_SUCCESS; /* force to NV_SET_SUCCESS,
                                       * don't care of NV_SET_UNCHANGED */
        }
        else if(ret == NV_SET_INVALID_VALUE)
        {
                nbu_log_notice("Set failed %s = %s (invalid value)",
                               nvname, valuevar);
                ewf_hdf_query_var_error(hdfname, "value");
                trx->sum_set_stat = -1;
        }
        else
        {
                /* critical failed */
                nbu_log_error("Set failed - %s = %s (ret=%d)",
                              nvname, valuevar, ret);
                ewf_hdf_query_error("system");
                ret = NV_SET_ERROR; /* force to NV_SET_ERROR */
        }

        return ret;
}

static inline int __nv_trx_commit(struct trx *trx)
{
        if(trx->sum_set_stat == -1)
        {
                nbu_log_notice("Don't commit dirty trx id %d", trx->id);
                return NV_COMMIT_UNCHANGED;
        }

        return nbd_user_config_cursor_commit(trx->id);
}

static inline int __nv_trx_close(struct trx *trx)
{
        int ret;
        ret = nbd_user_config_cursor_close(trx->id);

        if(ret != 0)
        {
                nbu_log_error("Error closing cursor id %d", trx->id);
        }

        return ret;
}

#define NV_TRX_INIT(trxvar)                                             \
        do {                                                            \
                if(__nv_trx_init(&trxvar))                              \
                {                                                       \
                        return;                                         \
                }                                                       \
        } while(0)

#define NV_SET_WTRX(trxvar, nvname, valuevar)                           \
        do {                                                            \
                if(__nv_set_with_trx(&trxvar,                           \
                                     nvname,                            \
                                     #valuevar,                         \
                                     valuevar) == NV_SET_ERROR)         \
                {                                                       \
                        nbd_user_config_cursor_close(trxvar.id);                 \
                        return;                                         \
                }                                                       \
        } while(0);

#define NV_TRX_XCOMMIT(trxvar)                                          \
        ({                                                              \
                int __dummy_ret = -1;                                   \
                __dummy_ret = __nv_trx_commit(&trxvar);                 \
                __nv_trx_close(&trxvar);                                \
                nbu_log_debug("Commit ret = %d", __dummy_ret);          \
                __dummy_ret;                                            \
        })

#define NV_TRX_CLOSE(trxvar)                                            \
        ({                                                              \
                __nv_trx_close(&trxvar);                                \
        })

#define NV_GET(nvvar)                                                   \
        do {                                                            \
                if(nbd_user_config_get(#nvvar, nvvar, sizeof(nvvar)) != 0)       \
                {                                                       \
                        nbu_log_error("Unable to get nv %s !", #nvvar); \
                        return;                                         \
                }                                                       \
        } while(0)

#define STATUS_GET(stvar)                                               \
        do {                                                            \
                if(nbd_status_get(#stvar, stvar, sizeof(stvar)) != 0)   \
                {                                                       \
                        nbu_log_error("Unable to get status %s !", #stvar); \
                        return;                                         \
                }                                                       \
        } while(0)

/*
 * 0) Check arguments
 * 1) nbd_user_config_get
 *    --> OK
 *             1a.1) ewf_hdf_set
 *    --> FAIL
 *             1b.2) log_error
 *
 * @return EWF_ERROR if fail, EWF_SUCCESS otherwise
 */
#define NV_TO_HDF(nvvar) nv_to_hdf(nvvar, nvvar)
int nv_to_hdf(const char *const nv_name, const char *const hdf_name);

/*
 * 0) Check arguments
 * 1) nbd_status_get
 *    --> OK
 *             1a.1) ewf_hdf_set
 *    --> FAIL
 *             1b.2) log_error
 *
 * @return EWF_ERROR if fail, EWF_SUCCESS otherwise
 */
#define STATUS_TO_HDF(stvar) status_to_hdf(stvar, stvar)
int status_to_hdf(const char *const nv_name, const char *const hdf_name);

/*
 * @return NBD_NV_SUCCESS if set has been completed
 */
#define HDF_TO_NV(hdvar) hdf_to_nv(hdvar, hdvar)
int hdf_to_nv(const char *hdf_name, const char *nvram_name);

#endif
