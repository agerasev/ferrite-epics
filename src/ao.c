#include <stdlib.h>

#include <aoRecord.h>
#include <devSup.h>
#include <epicsExport.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

SCALAR_STORE(_store, aoRecord)
SCALAR_LOAD(_load, aoRecord)

static long init(aoRecord *rec) {
    fer_epics_record_init(
        (dbCommon *)rec,
        (FerEpicsRecordInfo){
            .dir = FER_EPICS_RECORD_DIR_OUTPUT,
            .store = (FerEpicsRecordStoreFunc)_store,
            .load = (FerEpicsRecordLoadFunc)_load,
        },
        fer_epics_var_create((FerVarInfo){
            .type = FER_VAR_TYPE_F64,
            .max_len = 0,
        }));
    return 0;
}

GET_IOINT_INFO(aoRecord)

static long write(aoRecord *rec) {
    long st = fer_epics_record_process((dbCommon *)rec);
    if (st == 0) {
        return 2;
    } else {
        return st;
    }
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
