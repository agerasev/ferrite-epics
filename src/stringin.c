#include <stdlib.h>

#include <devSup.h>
#include <epicsExport.h>
#include <epicsTypes.h>
#include <recGbl.h>
#include <stringinRecord.h>

#include "_macros.h"
#include "_record.h"

STRING_STORE(_store, stringinRecord)
STRING_LOAD(_load, stringinRecord)

static long init(stringinRecord *rec) {
    fer_epics_record_init(
        (dbCommon *)rec,
        (FerEpicsRecordInfo){
            .dir = FER_EPICS_RECORD_DIR_INPUT,
            .store = (FerEpicsRecordStoreFunc)_store,
            .load = (FerEpicsRecordLoadFunc)_load,
        },
        fer_epics_var_create((FerVarInfo){
            .type = FER_VAR_TYPE_U8,
            .max_len = FER_EPICS_STRING_MAX_LEN,
        }));
    return 0;
}

GET_IOINT_INFO(stringinRecord)

static long read(stringinRecord *rec) {
    return fer_epics_record_process((dbCommon *)rec);
}

struct StringinRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read;
};

struct StringinRecordCallbacks stringin_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)read,
};

epicsExportAddress(dset, stringin_record_handler);
