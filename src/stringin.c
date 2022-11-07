#include <stdlib.h>

#include <devSup.h>
#include <epicsExport.h>
#include <epicsTypes.h>
#include <recGbl.h>
#include <stringinRecord.h>

#include "_array_record.h"
#include "_assert.h"
#include "_record.h"

#define VAL_LEN 40

static long init(stringinRecord *rec) {
    FerEpicsVarArray *var_info = (FerEpicsVarArray *)malloc(sizeof(FerEpicsVarArray));
    var_info->base.type = (FerVarType){
        FER_VAR_KIND_ARRAY,
        FER_VAR_DIR_WRITE,
        FER_VAR_SCALAR_TYPE_U8,
        VAL_LEN - 1,
    };
    var_info->item_size = 1;

    fer_epics_record_array_init((dbCommon *)rec, FER_EPICS_RECORD_TYPE_STRINGIN, var_info);
    return 0;
}

static long get_ioint_info(int cmd, stringinRecord *rec, IOSCANPVT *ppvt) {
    *ppvt = fer_epics_record_ioscan_create((dbCommon *)rec);
    return 0;
}

static long read(stringinRecord *rec) {
    FerEpicsVarArray *var_info = (FerEpicsVarArray *)fer_epics_record_var_info((dbCommon *)rec);

    if (rec->pact) {
        epicsUInt32 len = var_info->locked_len;
        fer_epics_record_array_copy_data((dbCommon *)rec, rec->val, &len);
        ((char *)rec->val)[len] = '\0';
    }

    fer_epics_record_process((dbCommon *)rec);
    return 0;
}

struct StringinRecordCallbacks {
    long number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read;
};

struct StringinRecordCallbacks stringin_record_handler = {
    5,
    NULL,
    NULL,
    (DEVSUPFUN)init,
    (DEVSUPFUN)get_ioint_info,
    (DEVSUPFUN)read,
};

epicsExportAddress(dset, stringin_record_handler);
