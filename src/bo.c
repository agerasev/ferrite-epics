#include <stdlib.h>

#include <boRecord.h>
#include <devSup.h>
#include <epicsExport.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

SCALAR_STORE(_store, boRecord)
SCALAR_LOAD(_load, boRecord)

static long init(boRecord *rec) {
    fer_epics_record_init(
        (dbCommon *)rec,
        (FerEpicsRecordInfo){
            .dir = FER_EPICS_RECORD_DIR_OUTPUT,
            .load = (FerEpicsRecordLoadFunc)_load,
            .store = (FerEpicsRecordStoreFunc)_store,
        },
        fer_epics_var_create((FerVarInfo){
            .perm = FER_VAR_PERM_READ | FER_VAR_PERM_WRITE,
            .type = FER_VAR_TYPE_U16,
            .max_len = 0,
        }));
    return 0;
}

GET_IOINT_INFO(boRecord)

static long write(boRecord *rec) {
    long st = fer_epics_record_process((dbCommon *)rec);
    if (st == 0) {
        return 2;
    } else {
        return st;
    }
}

struct BoRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write;
};

struct BoRecordCallbacks bo_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)write,
};

epicsExportAddress(dset, bo_record_handler);
