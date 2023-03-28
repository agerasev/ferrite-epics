#include <stdlib.h>

#include <devSup.h>
#include <epicsExport.h>
#include <longoutRecord.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

SCALAR_STORE(_store, longoutRecord)
SCALAR_LOAD(_load, longoutRecord)

static long init(longoutRecord *rec) {
    fer_epics_record_init(
        (dbCommon *)rec,
        (FerEpicsRecordInfo){
            .dir = FER_EPICS_RECORD_DIR_OUTPUT,
            .load = (FerEpicsRecordLoadFunc)_load,
            .store = (FerEpicsRecordStoreFunc)_store,
        },
        fer_epics_var_create((FerVarInfo){
            .type = FER_VAR_TYPE_I32,
            .max_len = 0,
        }));
    return 0;
}

GET_IOINT_INFO(longoutRecord)

static long write(longoutRecord *rec) {
    return fer_epics_record_process((dbCommon *)rec);
}

struct LongoutRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write;
};

struct LongoutRecordCallbacks longout_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)write,
};

epicsExportAddress(dset, longout_record_handler);
