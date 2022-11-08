#include "_record.h"

#include <stdlib.h>
#include <string.h>

#include "_assert.h"

FerEpicsVar *fer_epics_var_create(FerVarInfo info) {
    FerEpicsVar *var = (FerEpicsVar *)malloc(sizeof(FerEpicsVar));

    var->info = info;

    // Create additional buffer to store copy of data.
    // Array record contents can be updated even if record is processing (`PACT` is true).
    // To avoid data race we provide to user not an original data but a copy of it.
    size_t len = info.max_len == 0 ? 1 : info.max_len;
    var->value.data = malloc(len * fer_epics_type_size(info.type));
    var->value.len = 0;

    return var;
}

void fer_epics_var_destroy(FerEpicsVar *var) {
    free(var->value.data);
    free((void *)var);
}

void fer_epics_var_store_data(FerEpicsVar *var, void *data, size_t len) {
    if (var->info.max_len != 0) {
        fer_epics_assert(len <= var->info.max_len);
    } else {
        fer_epics_assert(len == 1);
    }
    memcpy(var->value.data, data, len * fer_epics_type_size(var->info.type));
    var->value.len = len;
}

size_t fer_epics_var_load_data(FerEpicsVar *var, void *data) {
    epicsUInt32 len;
    if (var->info.max_len == 0) {
        len = 1;
    } else {
        fer_epics_assert(len <= var->info.max_len);
        len = var->value.len;
    }
    memcpy(data, var->value.data, len * fer_epics_type_size(var->info.type));
    return len;
}

size_t fer_epics_type_size(FerVarType type) {
    switch (type) {
    case FER_VAR_TYPE_U8:
    case FER_VAR_TYPE_I8:
        return 1;
    case FER_VAR_TYPE_U16:
    case FER_VAR_TYPE_I16:
        return 2;
    case FER_VAR_TYPE_U32:
    case FER_VAR_TYPE_I32:
        return 4;
    case FER_VAR_TYPE_U64:
    case FER_VAR_TYPE_I64:
        return 8;
    case FER_VAR_TYPE_F32:
        return 4;
    case FER_VAR_TYPE_F64:
        return 8;
    }
    return 0;
}

FerVarType fer_epics_convert_type(menuFtype ftype) {
    switch (ftype) {
    case menuFtypeCHAR:
        return FER_VAR_TYPE_I8;
    case menuFtypeUCHAR:
        return FER_VAR_TYPE_U8;
    case menuFtypeSHORT:
        return FER_VAR_TYPE_I16;
    case menuFtypeUSHORT:
        return FER_VAR_TYPE_U16;
    case menuFtypeLONG:
        return FER_VAR_TYPE_I32;
    case menuFtypeULONG:
        return FER_VAR_TYPE_U32;
    case menuFtypeINT64:
        return FER_VAR_TYPE_I64;
    case menuFtypeUINT64:
        return FER_VAR_TYPE_U64;
    case menuFtypeFLOAT:
        return FER_VAR_TYPE_F32;
    case menuFtypeDOUBLE:
        return FER_VAR_TYPE_F64;
    default:
        fer_epics_unreachable();
    }
    return (FerVarType)0;
}
