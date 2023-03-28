#include <stdlib.h>

#include <devSup.h>
#include <epicsExport.h>
#include <longinRecord.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

SCALAR_STORE(_store, longinRecord)
SCALAR_LOAD(_load, longinRecord)

static long init(longinRecord *rec) {
    fer_epics_record_init(
        (dbCommon *)rec,
        (FerEpicsRecordInfo){
            .dir = FER_EPICS_RECORD_DIR_INPUT,
            .store = (FerEpicsRecordStoreFunc)_store,
            .load = (FerEpicsRecordLoadFunc)_load,
        },
        fer_epics_var_create((FerVarInfo){
            .type = FER_VAR_TYPE_I32,
            .max_len = 0,
        }));
    return 0;
}

GET_IOINT_INFO(longinRecord)

static long read(longinRecord *rec) {
    return fer_epics_record_process((dbCommon *)rec);
}

struct LonginRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read;
};

struct LonginRecordCallbacks longin_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)read,
};

epicsExportAddress(dset, longin_record_handler);
