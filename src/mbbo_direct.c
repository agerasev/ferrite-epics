#include <stdlib.h>

#include <devSup.h>
#include <epicsExport.h>
#include <mbboDirectRecord.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

SCALAR_STORE(_store, mbboDirectRecord)
SCALAR_LOAD(_load, mbboDirectRecord)

static long init(mbboDirectRecord *rec) {
    fer_epics_record_init(
        (dbCommon *)rec,
        (FerEpicsRecordInfo){
            .dir = FER_EPICS_RECORD_DIR_OUTPUT,
            .store = (FerEpicsRecordStoreFunc)_store,
            .load = (FerEpicsRecordLoadFunc)_load,
        },
        fer_epics_var_create((FerVarInfo){
            .perm = FER_VAR_PERM_READ | FER_VAR_PERM_WRITE,
            .type = FER_VAR_TYPE_U32,
            .max_len = 0,
        }));
    return 0;
}

GET_IOINT_INFO(mbboDirectRecord)

static long write(mbboDirectRecord *rec) {
    long st = fer_epics_record_process((dbCommon *)rec);
    if (st == 0) {
        return 2;
    } else {
        return st;
    }
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
