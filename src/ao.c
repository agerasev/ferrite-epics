#include <stdlib.h>

#include <aoRecord.h>
#include <devSup.h>
#include <epicsExport.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

static long init(aoRecord *rec) {
    FerEpicsVar *var = fer_epics_var_create((FerVarInfo){
        .perm = FER_VAR_PERM_READ | FER_VAR_PERM_WRITE,
        .type = FER_VAR_TYPE_F64,
        .max_len = 0,
    });

    fer_epics_record_init((dbCommon *)rec, var);
    return 0;
}

GET_IOINT_INFO(aoRecord)

SCALAR_STORE(_store, aoRecord)
SCALAR_LOAD(_load, aoRecord)

static long write(aoRecord *rec) {
    static const FerEpicsRecordInfo info = {
        .dir = FER_EPICS_RECORD_DIR_OUTPUT,
        .load = (FerEpicsRecordLoadFunc)_load,
        .store = (FerEpicsRecordStoreFunc)_store,
    };

    return fer_epics_record_process((dbCommon *)rec, &info);
}

static long linconv(aoRecord *rec, int after) {
    return 0;
}

struct AoRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write;
    DEVSUPFUN special_linconv;
};

struct AoRecordCallbacks ao_record_handler = {
    6,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)write,
    (DEVSUPFUN)linconv,
};

epicsExportAddress(dset, ao_record_handler);
