#include <stdlib.h>

#include <devSup.h>
#include <epicsExport.h>
#include <mbbiDirectRecord.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

static long init(mbbiDirectRecord *rec) {
    FerEpicsVar *var = fer_epics_var_create((FerVarInfo){
        .perm = FER_VAR_PERM_WRITE,
        .type = FER_VAR_TYPE_U32,
        .max_len = 0,
    });

    fer_epics_record_init((dbCommon *)rec, var);
    return 0;
}

GET_IOINT_INFO(mbbiDirectRecord)

SCALAR_STORE(_store, mbbiDirectRecord)
SCALAR_LOAD(_load, mbbiDirectRecord)

static long read(mbbiDirectRecord *rec) {
    static const FerEpicsRecordInfo info = {
        .dir = FER_EPICS_RECORD_DIR_INPUT,
        .load = (FerEpicsRecordLoadFunc)_load,
        .store = (FerEpicsRecordStoreFunc)_store,
    };

    long st = fer_epics_record_process((dbCommon *)rec, &info);
    if (st == 0) {
        return 2;
    } else {
        return st;
    }
}

struct MbbiDirectRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_ai;
};

struct MbbiDirectRecordCallbacks mbbi_direct_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)read,
};

epicsExportAddress(dset, mbbi_direct_record_handler);
