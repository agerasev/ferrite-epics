#pragma once

#include <string.h>

#include <dbCommon.h>

#include "_assert.h"
#include "_record.h"
#include "_variable.h"

#define GET_IOINT_INFO(rec_type) \
    static long get_ioint_info(int cmd, rec_type *rec, IOSCANPVT *ppvt) { \
        *ppvt = fer_epics_record_ioscan_create((dbCommon *)rec); \
        return 0; \
    }

#define _GET_VAR(var, rec) FerEpicsVar *var = fer_epics_record_var((dbCommon *)rec)

#define SCALAR_STORE(ident, rec_type) \
    static void ident(rec_type *rec) { \
        _GET_VAR(var, rec); \
        fer_epics_var_store_data(var, (void *)&rec->val, 1); \
    }

#define SCALAR_LOAD(ident, rec_type) \
    static void ident(rec_type *rec) { \
        _GET_VAR(var, rec); \
        fer_epics_assert(fer_epics_var_load_data(var, (void *)&rec->val) == 1); \
    }

#define ARRAY_STORE(ident, rec_type) \
    static void ident(rec_type *rec) { \
        _GET_VAR(var, rec); \
        fer_epics_var_store_data(var, rec->bptr, rec->nord); \
    }

#define ARRAY_LOAD(ident, rec_type) \
    static void ident(rec_type *rec) { \
        _GET_VAR(var, rec); \
        rec->nord = fer_epics_var_load_data(var, rec->bptr); \
    }

#define STRING_STORE(ident, rec_type) \
    static void ident(rec_type *rec) { \
        _GET_VAR(var, rec); \
        size_t len = strnlen((char *)rec->val, FER_EPICS_STRING_MAX_LEN); \
        fer_epics_var_store_data(var, (void *)rec->val, len); \
    }

#define STRING_LOAD(ident, rec_type) \
    static void ident(rec_type *rec) { \
        _GET_VAR(var, rec); \
        size_t len = fer_epics_var_load_data(var, rec->val); \
        ((char *)rec->val)[len] = '\0'; \
    }
