#include <stdlib.h>
#include <string.h>

#include <devSup.h>
#include <epicsExport.h>
#include <epicsTypes.h>
#include <recGbl.h>
#include <stringoutRecord.h>

#include "_macros.h"
#include "_record.h"

#define VAL_LEN 40

static long init(stringoutRecord *rec) {
    FerEpicsVar *var = fer_epics_var_create((FerVarInfo){
        .perm = FER_VAR_PERM_READ | FER_VAR_PERM_WRITE,
        .type = FER_VAR_TYPE_U8,
        .max_len = STRING_LEN - 1,
    });

    fer_epics_record_init((dbCommon *)rec, var);
    return 0;
}

GET_IOINT_INFO(stringoutRecord)

STRING_STORE(_load, stringoutRecord)
STRING_LOAD(_store, stringoutRecord)

static long write(stringoutRecord *rec) {
    static const FerEpicsRecordInfo info = {
        .dir = FER_EPICS_RECORD_DIR_OUTPUT,
        .load = (FerEpicsRecordLoadFunc)_load,
        .store = (FerEpicsRecordStoreFunc)_store,
    };

    return fer_epics_record_process((dbCommon *)rec, &info);
}

struct StringoutRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write;
};

struct StringoutRecordCallbacks stringout_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)write,
};

epicsExportAddress(dset, stringout_record_handler);
