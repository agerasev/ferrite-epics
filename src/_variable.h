#pragma once

#include <menuFtype.h>

#include "_interface.h"

typedef struct FerEpicsVar {
    FerVarInfo info;
    FerVarValue *value;
} FerEpicsVar;

/// Create interface variable.
FerEpicsVar *fer_epics_var_create(FerVarInfo info);
/// Destroy interface variable.
void fer_epics_var_destroy(FerEpicsVar *var);

/// Variable length.
size_t *fer_epics_var_value_len(FerEpicsVar *var);
/// Variable data.
void *fer_epics_var_value_data(FerEpicsVar *var);
/// Store data in variable.
void fer_epics_var_store_data(FerEpicsVar *var, void *data, size_t len);
/// Load data from variable.
size_t fer_epics_var_load_data(FerEpicsVar *var, void *data);

/// Size of item of specific type.
size_t fer_epics_type_size(FerVarType type);
/// Align of item of specific type.
size_t fer_epics_type_align(FerVarType type);
/// Convert `menuFtype` to `FerVarType`.
FerVarType fer_epics_convert_type(menuFtype ftype);
