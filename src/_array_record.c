#include "_array_record.h"

#include <stdlib.h>
#include <string.h>

#include "_assert.h"

FerEpicsVarArray *fer_epics_record_var_array_info(dbCommon *rec) {
    FerEpicsVar *var_info = fer_epics_record_var_info(rec);
    fer_epics_assert(var_info->type.kind == FER_VAR_KIND_ARRAY);
    return (FerEpicsVarArray *)var_info;
}

FerVarScalarType fer_epics_convert_scalar_type(menuFtype ftype) {
    switch (ftype) {
    case menuFtypeCHAR:
        return FER_VAR_SCALAR_TYPE_I8;
    case menuFtypeUCHAR:
        return FER_VAR_SCALAR_TYPE_U8;
    case menuFtypeSHORT:
        return FER_VAR_SCALAR_TYPE_I16;
    case menuFtypeUSHORT:
        return FER_VAR_SCALAR_TYPE_U16;
    case menuFtypeLONG:
        return FER_VAR_SCALAR_TYPE_I32;
    case menuFtypeULONG:
        return FER_VAR_SCALAR_TYPE_U32;
    case menuFtypeINT64:
        return FER_VAR_SCALAR_TYPE_I64;
    case menuFtypeUINT64:
        return FER_VAR_SCALAR_TYPE_U64;
    case menuFtypeFLOAT:
        return FER_VAR_SCALAR_TYPE_F32;
    case menuFtypeDOUBLE:
        return FER_VAR_SCALAR_TYPE_F64;
    default:
        return FER_VAR_SCALAR_TYPE_NONE;
    }
}

size_t fer_epics_scalar_type_size(menuFtype ftype) {
    switch (ftype) {
    case menuFtypeCHAR:
    case menuFtypeUCHAR:
        return 1;
    case menuFtypeSHORT:
    case menuFtypeUSHORT:
        return 2;
    case menuFtypeLONG:
    case menuFtypeULONG:
        return 4;
    case menuFtypeINT64:
    case menuFtypeUINT64:
        return 8;
    case menuFtypeFLOAT:
        return 4;
    case menuFtypeDOUBLE:
        return 8;
    default:
        return 0;
    }
}

void fer_epics_record_array_init(dbCommon *rec, FerEpicsRecordType type, FerEpicsVarArray *var_info) {
    // Create additional buffer to store copy of data.
    // See note in `fer_epics_record_array_copy_data` function below.
    var_info->locked_data = malloc(var_info->base.type.array_max_len * var_info->item_size);
    var_info->locked_len = 0;

    var_info->base.data = var_info->locked_data;
    var_info->len_ptr = &var_info->locked_len;

    fer_epics_record_init(rec, type, (FerEpicsVar *)var_info);
}

void fer_epics_record_array_deinit(dbCommon *rec) {
    FerEpicsVarArray *var_info = fer_epics_record_var_array_info(rec);

    if (var_info->locked_data != NULL) {
        free(var_info->locked_data);
    }

    fer_epics_record_deinit(rec);
}

void fer_epics_record_array_copy_data(dbCommon *rec, void *data, epicsUInt32 *len_ptr) {
    FerEpicsVarArray *var_info = fer_epics_record_var_array_info(rec);

    // Array record contents can be updated even if record is processing (`PACT` is true).
    // To avoid data race we provide to user not an original data but a copy of it.
    if (!rec->pact) {
        const epicsUInt32 len = *len_ptr;
        fer_epics_assert(len <= var_info->base.type.array_max_len);
        memcpy(var_info->locked_data, data, len * var_info->item_size);
        var_info->locked_len = len;
    } else {
        const epicsUInt32 len = var_info->locked_len;
        fer_epics_assert(len <= var_info->base.type.array_max_len);
        memcpy(data, var_info->locked_data, len * var_info->item_size);
        *len_ptr = len;
    }
}
