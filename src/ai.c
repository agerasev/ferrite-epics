#include <stdlib.h>

#include <aiRecord.h>
#include <devSup.h>
#include <epicsExport.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

SCALAR_STORE(_store, aiRecord)
SCALAR_LOAD(_load, aiRecord)

static long init(aiRecord *rec) {
    fer_epics_record_init(
        (dbCommon *)rec,
        (FerEpicsRecordInfo){
            .dir = FER_EPICS_RECORD_DIR_INPUT,
            .store = (FerEpicsRecordStoreFunc)_store,
            .load = (FerEpicsRecordLoadFunc)_load,
        },
        fer_epics_var_create((FerVarInfo){
            .perm = FER_VAR_PERM_WRITE,
            .type = FER_VAR_TYPE_F64,
            .max_len = 0,
        }));
    return 0;
}

GET_IOINT_INFO(aiRecord)

static long read(aiRecord *rec) {
    long st = fer_epics_record_process((dbCommon *)rec);
    if (st == 0) {
        return 2;
    } else {
        return st;
    }
}

static long linconv(aiRecord *rec, int after) {
    return 0;
}

struct AiRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read;
    DEVSUPFUN special_linconv;
};

struct AiRecordCallbacks ai_record_handler = {
    6,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)read,
    (DEVSUPFUN)linconv,
};

epicsExportAddress(dset, ai_record_handler);
