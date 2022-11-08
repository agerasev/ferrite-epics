#include "_record.h"

#include <stdbool.h>
#include <stdlib.h>

#include <callback.h>
#include <dbAccess.h>
#include <dbCommon.h>
#include <dbScan.h>
#include <recSup.h>

#include "_assert.h"
#include "_interface.h"

static void process_callback(epicsCallback *callback) {
    dbCommon *rec = (dbCommon *)callback->user;
    dbScanLock(rec);
    struct typed_rset *rset = (struct typed_rset *)rec->rset;
    rset->process(rec);
    dbScanUnlock(rec);
}

void fer_epics_record_init(dbCommon *rec, FerEpicsVar *var) {
    FerEpicsRecordDpvt *dpvt = (FerEpicsRecordDpvt *)malloc(sizeof(FerEpicsRecordDpvt));
    fer_epics_assert(dpvt != NULL);

    dpvt->process = fer_epics_proc_req_create(rec);

    dpvt->ioscan_list = NULL;
    dpvt->var = var;
    dpvt->user_data = NULL;

    fer_epics_assert(rec->dpvt == NULL);
    rec->dpvt = dpvt;

    fer_var_init((FerVar *)rec);
}

void fer_epics_record_deinit(dbCommon *rec) {
    if (rec->dpvt != NULL) {
        FerEpicsRecordDpvt *dpvt = fer_epics_record_dpvt(rec);
        if (dpvt->var != NULL) {
            fer_epics_var_destroy(dpvt->var);
        }
        free((void *)rec->dpvt);
        rec->dpvt = NULL;
    }
}

FerEpicsRecordDpvt *fer_epics_record_dpvt(dbCommon *rec) {
    FerEpicsRecordDpvt *dpvt = (FerEpicsRecordDpvt *)rec->dpvt;
    fer_epics_assert(dpvt != NULL);
    return dpvt;
}

FerEpicsVar *fer_epics_record_var(dbCommon *rec) {
    FerEpicsVar *var = fer_epics_record_dpvt(rec)->var;
    fer_epics_assert(var != NULL);
    return var;
}

FerEpicsProcReq fer_epics_proc_req_create(dbCommon *rec) {
    FerEpicsProcReq process;

    callbackSetCallback(process_callback, &process.callback);
    callbackSetUser((void *)rec, &process.callback);
    callbackSetPriority(priorityMedium, &process.callback);

    process.op = FER_EPICS_RECORD_OP_READ;
    process.status = 0;

    return process;
}

IOSCANPVT fer_epics_record_ioscan_create(dbCommon *rec) {
    FerEpicsRecordDpvt *dpvt = fer_epics_record_dpvt(rec);

    dpvt->var->info.perm |= FER_VAR_PERM_NOTIFY;

    IOSCANPVT ioscan_list;
    scanIoInit(&ioscan_list);
    dpvt->ioscan_list = ioscan_list;
    return ioscan_list;
}

void fer_epics_record_request_proc(dbCommon *rec) {
    FerEpicsRecordDpvt *dpvt = fer_epics_record_dpvt(rec);
    if (dpvt->ioscan_list != NULL) {
        scanIoRequest(dpvt->ioscan_list);
    }
}

long fer_epics_record_process(dbCommon *rec, const FerEpicsRecordInfo *info) {
    FerEpicsRecordDpvt *dpvt = fer_epics_record_dpvt(rec);
    if (!rec->pact) {
        rec->pact = true;
        if (info->dir == FER_EPICS_RECORD_DIR_OUTPUT) {
            // Store value to buffer.
            info->store(rec);
        }
        fer_var_proc_begin((FerVar *)rec);
        return 0;
    } else {
        if (info->dir == FER_EPICS_RECORD_DIR_INPUT || dpvt->process.op == FER_EPICS_RECORD_OP_WRITE) {
            // Load value from buffer.
            info->load(rec);
        }
        rec->pact = false;
        fer_var_proc_end((FerVar *)rec);
        return dpvt->process.status;
    }
}

void fer_epics_record_complete_proc(dbCommon *rec, FerEpicsRecordOp op, long status) {
    FerEpicsProcReq *process = &fer_epics_record_dpvt(rec)->process;
    process->op = op;
    process->status = status;
    callbackRequest(&process->callback);
}
