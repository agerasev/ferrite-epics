#include <stdlib.h>
#include <string.h>

#include <devSup.h>
#include <epicsExport.h>
#include <epicsTypes.h>
#include <recGbl.h>
#include <stringoutRecord.h>

#include "_array_record.h"
#include "_assert.h"
#include "_record.h"

#define VAL_LEN 40

static long init(stringoutRecord *rec) {
    FerEpicsVarArray *var_info = (FerEpicsVarArray *)malloc(sizeof(FerEpicsVarArray));
    var_info->base.type = (FerVarType){
        FER_VAR_KIND_ARRAY,
        FER_VAR_DIR_READ,
        FER_VAR_SCALAR_TYPE_U8,
        VAL_LEN - 1,
    };
    var_info->item_size = 1;

    fer_epics_record_array_init((dbCommon *)rec, FER_EPICS_RECORD_TYPE_STRINGOUT, var_info);
    return 0;
}

static long get_ioint_info(int cmd, stringoutRecord *rec, IOSCANPVT *ppvt) {
    *ppvt = fer_epics_record_ioscan_create((dbCommon *)rec);
    return 0;
}

static long write(stringoutRecord *rec) {
    if (!rec->pact) {
        epicsUInt32 len = strnlen((char *)rec->val, VAL_LEN - 1);
        fer_epics_record_array_copy_data((dbCommon *)rec, rec->val, &len);
    }

    fer_epics_record_process((dbCommon *)rec);
    return 0;
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
