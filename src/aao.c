#include <stdlib.h>

#include <aaoRecord.h>
#include <devSup.h>
#include <epicsExport.h>
#include <epicsTypes.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

static long init(aaoRecord *rec) {
    FerEpicsVar *var = fer_epics_var_create((FerVarInfo){
        .perm = FER_VAR_PERM_READ | FER_VAR_PERM_WRITE,
        .type = fer_epics_convert_type((menuFtype)rec->ftvl),
        .max_len = rec->nelm,
    });

    fer_epics_record_init((dbCommon *)rec, var);
    return 0;
}

GET_IOINT_INFO(aaoRecord)

ARRAY_STORE(_store, aaoRecord)
ARRAY_LOAD(_load, aaoRecord)

static long write(aaoRecord *rec) {
    static const FerEpicsRecordInfo info = {
        .dir = FER_EPICS_RECORD_DIR_OUTPUT,
        .load = (FerEpicsRecordLoadFunc)_load,
        .store = (FerEpicsRecordStoreFunc)_store,
    };

    return fer_epics_record_process((dbCommon *)rec, &info);
}

struct AaoRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN write;
};

struct AaoRecordCallbacks aao_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)write,
};

epicsExportAddress(dset, aao_record_handler);
