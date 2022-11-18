#include "_record.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <alarm.h>
#include <callback.h>
#include <dbAccess.h>
#include <dbCommon.h>
#include <dbScan.h>
#include <recGbl.h>
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

void fer_epics_record_init(dbCommon *rec, FerEpicsRecordInfo info, FerEpicsVar *var) {
    FerEpicsRecordDpvt *dpvt = (FerEpicsRecordDpvt *)malloc(sizeof(FerEpicsRecordDpvt));
    fer_epics_assert(dpvt != NULL);

    dpvt->process = fer_epics_proc_req_create(rec);
    dpvt->ioscan_list = NULL;
    dpvt->info = info;
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

    process.status = FER_VAR_STATUS_ERROR;

    return process;
}

IOSCANPVT fer_epics_record_ioscan_create(dbCommon *rec) {
    FerEpicsRecordDpvt *dpvt = fer_epics_record_dpvt(rec);

    dpvt->var->info.perm |= FER_VAR_PERM_REQUEST;

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

long fer_epics_record_process(dbCommon *rec) {
    FerEpicsRecordDpvt *dpvt = fer_epics_record_dpvt(rec);
    if (!rec->pact) {
        rec->pact = true;

        // Store value from record to buffer.
        dpvt->info.store(rec);

        fer_var_proc_begin((FerVar *)rec);
        return 0;
    } else {
        rec->pact = false;

        // Unless a new value been put during processing:
        if (!(rec->putf && rec->rpro)) {
            // Load value from buffer to record.
            dpvt->info.load(rec);
            rec->udf = 0;
        }

        fer_var_proc_end((FerVar *)rec);

        switch (dpvt->process.status) {
        case FER_VAR_STATUS_OK:
            recGblSetSevr(rec, NO_ALARM, NO_ALARM);
            return 0;
        case FER_VAR_STATUS_ERROR:
            recGblSetSevrMsg(rec, COMM_ALARM, INVALID_ALARM, dpvt->process.message);
            return 1;
        }
        fer_epics_unreachable();
        return 1;
    }
}

void fer_epics_record_complete_proc(dbCommon *rec, FerVarStatus st, const char *msg, size_t msg_len) {
    FerEpicsProcReq *process = &fer_epics_record_dpvt(rec)->process;
    process->status = st;

    if (msg != NULL) {
        size_t len = msg_len < FER_EPICS_STRING_MAX_LEN ? msg_len : FER_EPICS_STRING_MAX_LEN;
        memcpy(process->message, msg, len);
        process->message[len] = '\0';
    } else {
        process->message[0] = '\0';
    }

    callbackRequest(&process->callback);
}
