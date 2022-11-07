#pragma once

#include <epicsTypes.h>
#include <menuFtype.h>

#include "_interface.h"
#include "_record.h"

typedef struct FerEpicsVarArray {
    FerEpicsVar base;
    size_t item_size;
    epicsUInt32 *len_ptr;
    epicsUInt32 locked_len;
    void *locked_data;

} FerEpicsVarArray;

FerEpicsVarArray *fer_epics_record_var_array_info(dbCommon *rec);

FerVarScalarType fer_epics_convert_scalar_type(menuFtype ftype);

size_t fer_epics_scalar_type_size(menuFtype ftype);

void fer_epics_record_array_init(dbCommon *rec, FerEpicsRecordType type, FerEpicsVarArray *var_info);
void fer_epics_record_array_deinit(dbCommon *rec);

void fer_epics_record_array_copy_data(dbCommon *rec, void *data, epicsUInt32 *len_ptr);
