#include <stdlib.h>

#include <devSup.h>
#include <epicsExport.h>
#include <epicsTypes.h>
#include <lsoRecord.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

GENERIC_ARRAY_STORE(_store, lsoRecord, val, len)
GENERIC_ARRAY_LOAD(_load, lsoRecord, val, len)

static long init(lsoRecord *rec) {
    fer_epics_record_init(
        (dbCommon *)rec,
        (FerEpicsRecordInfo){
            .dir = FER_EPICS_RECORD_DIR_OUTPUT,
            .store = (FerEpicsRecordStoreFunc)_store,
            .load = (FerEpicsRecordLoadFunc)_load,
        },
        fer_epics_var_create((FerVarInfo){
            .type = FER_VAR_TYPE_U8,
            .max_len = rec->sizv,
        }));
    return 0;
}

GET_IOINT_INFO(lsoRecord)

static long write(lsoRecord *rec) {
    return fer_epics_record_process((dbCommon *)rec);
}

struct LsoRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write;
};

struct LsoRecordCallbacks lso_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)write,
};

epicsExportAddress(dset, lso_record_handler);
