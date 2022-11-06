#include <stdlib.h>
#include <string.h>

#include <aaiRecord.h>
#include <devSup.h>
#include <epicsExport.h>
#include <epicsTypes.h>
#include <recGbl.h>

#include "_array_record.h"
#include "_record.h"

static long init(aaiRecord *rec) {
    FerEpicsVarArray *var_info = (FerEpicsVarArray *)malloc(sizeof(FerEpicsVarArray));
    var_info->base.type = (FerVarType){
        FER_VAR_KIND_ARRAY,
        FER_VAR_DIR_WRITE,
        fer_epics_convert_scalar_type((menuFtype)rec->ftvl),
        rec->nelm,
    };
    var_info->item_size = fer_epics_scalar_type_size((menuFtype)rec->ftvl);

    // Create additional buffer to store copy of data.
    // See note in `read` function below.
    var_info->locked_data = malloc(rec->nelm * var_info->item_size);
    var_info->locked_len = 0;

    var_info->base.data = var_info->locked_data;
    var_info->len_ptr = &var_info->locked_len;

    fer_epics_record_init((dbCommon *)rec, FER_EPICS_RECORD_TYPE_AAI, (FerEpicsVar *)var_info);
    return 0;
}

static long get_ioint_info(int cmd, aaiRecord *rec, IOSCANPVT *ppvt) {
    *ppvt = fer_epics_record_ioscan_create((dbCommon *)rec);
    return 0;
}

static long read(aaiRecord *rec) {
    FerEpicsVarArray *var_info = (FerEpicsVarArray *)fer_epics_record_var_info((dbCommon *)rec);

    // `aaoRecord->(bptr/nord)` can be read even if record is processing (`PACT` is true).
    // To mitigate this issue we copy data and length only on processing completion.
    if (rec->pact) {
        const epicsUInt32 len = var_info->locked_len;
        memcpy(rec->bptr, var_info->locked_data, len * var_info->item_size);
        rec->nord = len;
    }

    fer_epics_record_process((dbCommon *)rec);
    return 0;
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
