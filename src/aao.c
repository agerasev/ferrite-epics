#include <stdlib.h>

#include <aaoRecord.h>
#include <devSup.h>
#include <epicsExport.h>
#include <epicsTypes.h>
#include <recGbl.h>

#include "_macros.h"
#include "_record.h"

ARRAY_STORE(_store, aaoRecord)
ARRAY_LOAD(_load, aaoRecord)

static long init(aaoRecord *rec) {
    fer_epics_record_init(
        (dbCommon *)rec,
        (FerEpicsRecordInfo){
            .dir = FER_EPICS_RECORD_DIR_OUTPUT,
            .store = (FerEpicsRecordStoreFunc)_store,
            .load = (FerEpicsRecordLoadFunc)_load,
        },
        fer_epics_var_create((FerVarInfo){
            .type = fer_epics_convert_type((menuFtype)rec->ftvl),
            .max_len = rec->nelm,
        }));
    return 0;
}

GET_IOINT_INFO(aaoRecord)

static long write(aaoRecord *rec) {
    return fer_epics_record_process((dbCommon *)rec);
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
