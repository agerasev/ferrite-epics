#include <stdlib.h>

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

    fer_epics_record_array_init((dbCommon *)rec, FER_EPICS_RECORD_TYPE_AAI, var_info);
    return 0;
}

static long get_ioint_info(int cmd, aaiRecord *rec, IOSCANPVT *ppvt) {
    *ppvt = fer_epics_record_ioscan_create((dbCommon *)rec);
    return 0;
}

static long read(aaiRecord *rec) {
    if (rec->pact) {
        fer_epics_record_array_copy_data((dbCommon *)rec, rec->bptr, &rec->nord);
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
