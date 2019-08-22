/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <errno.h>
#include "ls_osa.h"
#include "ls_hal.h"
#include "sst_dbg.h"
#include "sst.h"
#include "sst_wrapper.h"

#define MAX_DATA_LEN 512
#define ITEM_NAME_LEN 65 //2 * SHA256_HASH_SIZE + 1

static uint32_t _kv_to_sst_res(int32_t err)
{
    uint32_t ret;

    switch (err) {
        case 0:
            ret = SST_SUCCESS;
            break;
        case SST_HAL_ERROR_ITEM_NOT_FOUND:
            ret = SST_ERROR_ITEM_NOT_FOUND;
            break;
        case SST_HAL_ERROR_STORAGE_NO_SPACE:
            ret = SST_ERROR_STORAGE_NO_SPACE;
            break;
        case SST_HAL_ERROR_BAD_PARAMETERS:
            ret = SST_ERROR_BAD_PARAMETERS;
            break;
        case SST_HAL_ERROR_OUT_OF_MEMORY:
            ret = SST_ERROR_OUT_OF_MEMORY;
            break;
        default:
            ret = SST_ERROR_GENERIC;
            break;
    }

    return ret;
}

uint64_t sst_current_raw_time(void)
{
    return ls_osa_get_time_ms();
}

uint32_t sst_store_obj(const char *name, void *file_data, uint32_t file_len, uint32_t flag)
{
    uint32_t ret = 0;
    int res = 0;
    char item_name[ITEM_NAME_LEN];

    sst_memset(item_name, 0, ITEM_NAME_LEN);
    ret = sst_imp_set_obj_name(name, item_name);
    if (ret) {
        SST_ERR("set obj name failed\n");
        return ret;
    }

    res = ls_hal_kv_set(item_name, file_data, file_len, 0);
    ret = _kv_to_sst_res(res);

    return ret;
}

uint32_t sst_get_obj(const char *name, void **pp_data, uint32_t *p_file_len)
{
    uint32_t ret = 0;
    uint8_t *file_data = NULL;
    uint32_t file_len = MAX_DATA_LEN;
    int res;
    char item_name[ITEM_NAME_LEN];

    sst_memset(item_name, 0, ITEM_NAME_LEN);
    ret = sst_imp_set_obj_name(name, item_name);
    if (ret) {
        SST_ERR("set obj name failed\n");
        return ret;
    }

    file_data = ls_osa_malloc(MAX_DATA_LEN);
    if (!file_data) {
        SST_ERR("fs malloc error\n");
        *pp_data = NULL;
        return SST_ERROR_OUT_OF_MEMORY;
    }
    sst_memset(file_data, 0, MAX_DATA_LEN);

    res = ls_hal_kv_get(item_name, file_data, (int *)&file_len);
    ret = _kv_to_sst_res(res);
    if (SST_SUCCESS != ret) {
        SST_ERR("fs get file error %x\n", (unsigned int)ret);
        ls_osa_free(file_data);
        *pp_data = NULL;
        goto _err;
    }
    *p_file_len = file_len;
    *pp_data = file_data;

_err:
    return ret;
}

uint32_t sst_delete_obj(const char *name)
{
    int res = 0;
    uint32_t ret = 0;
    char item_name[ITEM_NAME_LEN];

    sst_memset(item_name, 0, ITEM_NAME_LEN);
    ret = sst_imp_set_obj_name(name, item_name);
    if (ret) {
        SST_ERR("set obj name failed\n");
        return ret;
    }

    res = ls_hal_kv_del(item_name);
    ret = _kv_to_sst_res(res);
    if (ret) {
        SST_ERR("kv del failed res %d ret %x\n", res, (unsigned int)ret);
    }

    return ret;
}

