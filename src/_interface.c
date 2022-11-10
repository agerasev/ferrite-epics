#include "_interface.h"

#include <dbCommon.h>
#include <dbLock.h>
#include <epicsExit.h>

#include "_assert.h"
#include "_record.h"

void fer_app_exit(int code) {
    epicsExit(code);
}

void fer_var_request(FerVar *var) {
    fer_epics_record_request_proc((dbCommon *)var);
}

void fer_var_commit(FerVar *var, FerVarAction action) {
    fer_epics_record_complete_proc((dbCommon *)var, action);
}

void fer_var_lock(FerVar *var) {
    dbScanLock((dbCommon *)var);
}

void fer_var_unlock(FerVar *var) {
    dbScanUnlock((dbCommon *)var);
}

const char *fer_var_name(FerVar *var) {
    return ((dbCommon *)var)->name;
}

FerVarInfo fer_var_info(FerVar *var) {
    return fer_epics_record_var((dbCommon *)var)->info;
}

FerVarValue *fer_var_value(FerVar *var) {
    return &fer_epics_record_var((dbCommon *)var)->value;
}

void *fer_var_user_data(FerVar *var) {
    return fer_epics_record_dpvt((dbCommon *)var)->user_data;
}

void fer_var_set_user_data(FerVar *var, void *user_data) {
    fer_epics_record_dpvt((dbCommon *)var)->user_data = user_data;
}
