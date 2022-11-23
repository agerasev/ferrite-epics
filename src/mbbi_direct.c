#include <stdlib.h>

#include <devSup.h>
#include <epicsExport.h>
#include <mbbiDirectRecord.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

SCALAR_STORE(_store, mbbiDirectRecord)
SCALAR_LOAD(_load, mbbiDirectRecord)

static long init(mbbiDirectRecord *rec) {
    fer_epics_record_init(
        (dbCommon *)rec,
        (FerEpicsRecordInfo){
            .dir = FER_EPICS_RECORD_DIR_INPUT,
            .store = (FerEpicsRecordStoreFunc)_store,
            .load = (FerEpicsRecordLoadFunc)_load,
        },
        fer_epics_var_create((FerVarInfo){
            .type = FER_VAR_TYPE_U32,
            .max_len = 0,
        }));
    return 0;
}

GET_IOINT_INFO(mbbiDirectRecord)

static long read(mbbiDirectRecord *rec) {
    long st = fer_epics_record_process((dbCommon *)rec);
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
    DEVSUPFUN read;
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
