#include <stdlib.h>

#include <devSup.h>
#include <epicsExport.h>
#include <mbboDirectRecord.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

static long init(mbboDirectRecord *rec) {
    FerEpicsVar *var = fer_epics_var_create((FerVarInfo){
        .perm = FER_VAR_PERM_READ | FER_VAR_PERM_WRITE,
        .type = FER_VAR_TYPE_U32,
        .max_len = 0,
    });

    fer_epics_record_init((dbCommon *)rec, var);
    return 0;
}

GET_IOINT_INFO(mbboDirectRecord)

SCALAR_STORE(_store, mbboDirectRecord)
SCALAR_LOAD(_load, mbboDirectRecord)

static long write(mbboDirectRecord *rec) {
    static const FerEpicsRecordInfo info = {
        .dir = FER_EPICS_RECORD_DIR_OUTPUT,
        .load = (FerEpicsRecordLoadFunc)_load,
        .store = (FerEpicsRecordStoreFunc)_store,
    };

    return fer_epics_record_process((dbCommon *)rec, &info);
}

struct MbboDirectRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write;
};

struct MbboDirectRecordCallbacks mbbo_direct_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)write,
};

epicsExportAddress(dset, mbbo_direct_record_handler);
