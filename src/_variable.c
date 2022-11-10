#include "_record.h"

#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

#include <epicsTypes.h>

#include "_assert.h"

static size_t _data_offset(FerVarType type) {
    if (sizeof(size_t) % fer_epics_type_align(type) == 0) {
        return sizeof(size_t);
    } else {
        return fer_epics_type_align(type);
    }
}

FerEpicsVar *fer_epics_var_create(FerVarInfo info) {
    FerEpicsVar *var = (FerEpicsVar *)malloc(sizeof(FerEpicsVar));

    var->info = info;

    // Create additional buffer to store copy of data.
    // Array record contents can be updated even if record is processing (`PACT` is true).
    // To avoid data race we provide to user not an original data but a copy of it.
    if (info.max_len == 0) {
        // Scalar
        var->value = (FerVarValue *)malloc(fer_epics_type_size(info.type));
    } else {
        // Vector
        var->value = (FerVarValue *)malloc(_data_offset(info.type) + info.max_len * fer_epics_type_size(info.type));
        *(size_t *)var->value = 0;
    }

    return var;
}

void fer_epics_var_destroy(FerEpicsVar *var) {
    free((void *)var->value);
    free((void *)var);
}

size_t *fer_epics_var_value_len(FerEpicsVar *var) {
    if (var->info.max_len == 0) {
        return NULL;
    } else {
        return (size_t *)var->value;
    }
}

void *fer_epics_var_value_data(FerEpicsVar *var) {
    if (var->info.max_len == 0) {
        return (void *)var->value;
    } else {
        return (void *)var->value + _data_offset(var->info.type);
    }
}

void fer_epics_var_store_data(FerEpicsVar *var, void *data, size_t len) {
    if (var->info.max_len == 0) {
        fer_epics_assert(len == 1);
    } else {
        fer_epics_assert(len <= var->info.max_len);
        *fer_epics_var_value_len(var) = len;
    }
    memcpy(fer_epics_var_value_data(var), data, len * fer_epics_type_size(var->info.type));
}

size_t fer_epics_var_load_data(FerEpicsVar *var, void *data) {
    size_t len;
    if (var->info.max_len == 0) {
        len = 1;
    } else {
        len = *fer_epics_var_value_len(var);
        fer_epics_assert(len <= var->info.max_len);
    }
    memcpy(data, fer_epics_var_value_data(var), len * fer_epics_type_size(var->info.type));
    return len;
}

size_t fer_epics_type_size(FerVarType type) {
    switch (type) {
    case FER_VAR_TYPE_U8:
        return sizeof(epicsUInt8);
    case FER_VAR_TYPE_I8:
        return sizeof(epicsInt8);
    case FER_VAR_TYPE_U16:
        return sizeof(epicsUInt16);
    case FER_VAR_TYPE_I16:
        return sizeof(epicsInt16);
    case FER_VAR_TYPE_U32:
        return sizeof(epicsUInt32);
    case FER_VAR_TYPE_I32:
        return sizeof(epicsInt32);
    case FER_VAR_TYPE_U64:
        return sizeof(epicsUInt64);
    case FER_VAR_TYPE_I64:
        return sizeof(epicsInt64);
    case FER_VAR_TYPE_F32:
        return sizeof(float);
    case FER_VAR_TYPE_F64:
        return sizeof(double);
    }
    return 0;
}

size_t fer_epics_type_align(FerVarType type) {
    switch (type) {
    case FER_VAR_TYPE_U8:
        return alignof(epicsUInt8);
    case FER_VAR_TYPE_I8:
        return alignof(epicsInt8);
    case FER_VAR_TYPE_U16:
        return alignof(epicsUInt16);
    case FER_VAR_TYPE_I16:
        return alignof(epicsInt16);
    case FER_VAR_TYPE_U32:
        return alignof(epicsUInt32);
    case FER_VAR_TYPE_I32:
        return alignof(epicsInt32);
    case FER_VAR_TYPE_U64:
        return alignof(epicsUInt64);
    case FER_VAR_TYPE_I64:
        return alignof(epicsInt64);
    case FER_VAR_TYPE_F32:
        return alignof(float);
    case FER_VAR_TYPE_F64:
        return alignof(double);
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
