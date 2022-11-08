#include <stdlib.h>

#include <biRecord.h>
#include <devSup.h>
#include <epicsExport.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

static long init(biRecord *rec) {
    FerEpicsVar *var = fer_epics_var_create((FerVarInfo){
        .perm = FER_VAR_PERM_WRITE,
        .type = FER_VAR_TYPE_U16,
        .max_len = 0,
    });

    fer_epics_record_init((dbCommon *)rec, var);
    return 0;
}

GET_IOINT_INFO(biRecord)

SCALAR_STORE(_store, biRecord)
SCALAR_LOAD(_load, biRecord)

static long read(biRecord *rec) {
    static const FerEpicsRecordInfo info = {
        .dir = FER_EPICS_RECORD_DIR_INPUT,
        .load = (FerEpicsRecordLoadFunc)_load,
        .store = (FerEpicsRecordStoreFunc)_store,
    };

    return fer_epics_record_process((dbCommon *)rec, &info);
}

struct BiRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_ai;
};

struct BiRecordCallbacks bi_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)read,
};

epicsExportAddress(dset, bi_record_handler);
