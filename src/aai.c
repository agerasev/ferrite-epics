#include <stdlib.h>

#include <aaiRecord.h>
#include <devSup.h>
#include <epicsExport.h>
#include <epicsTypes.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

ARRAY_STORE(_store, aaiRecord)
ARRAY_LOAD(_load, aaiRecord)

static long init(aaiRecord *rec) {
    fer_epics_record_init(
        (dbCommon *)rec,
        (FerEpicsRecordInfo){
            .dir = FER_EPICS_RECORD_DIR_INPUT,
            .store = (FerEpicsRecordStoreFunc)_store,
            .load = (FerEpicsRecordLoadFunc)_load,
        },
        fer_epics_var_create((FerVarInfo){
            .perm = FER_VAR_PERM_WRITE,
            .type = fer_epics_convert_type((menuFtype)rec->ftvl),
            .max_len = rec->nelm,
        }));
    return 0;
}

GET_IOINT_INFO(aaiRecord)

static long read(aaiRecord *rec) {
    return fer_epics_record_process((dbCommon *)rec);
}

struct AaiRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read;
};

struct AaiRecordCallbacks aai_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)read,
};

epicsExportAddress(dset, aai_record_handler);
