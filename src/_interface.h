#pragma once

#include <stddef.h>
#include <stdint.h>

/// Opaque variable.
typedef struct FerVar FerVar;

/// Variable processing status.
typedef enum FerVarAction {
    FER_VAR_ACTION_DISCARD = 0,
    FER_VAR_ACTION_READ,
    FER_VAR_ACTION_WRITE,
} FerVarAction;

/// Permissions the variable.
typedef uint32_t FerVarPerm;
#define FER_VAR_PERM_READ ((FerVarPerm)1)
#define FER_VAR_PERM_WRITE ((FerVarPerm)2)
#define FER_VAR_PERM_REQUEST ((FerVarPerm)4)

/// Variable value type.
typedef enum FerVarType {
    FER_VAR_TYPE_U8 = 0,
    FER_VAR_TYPE_I8,
    FER_VAR_TYPE_U16,
    FER_VAR_TYPE_I16,
    FER_VAR_TYPE_U32,
    FER_VAR_TYPE_I32,
    FER_VAR_TYPE_U64,
    FER_VAR_TYPE_I64,
    FER_VAR_TYPE_F32,
    FER_VAR_TYPE_F64,
} FerVarType;

/// Information about variable.
typedef struct FerVarInfo {
    /// Permissions of the variable.
    FerVarPerm perm;
    /// Type of variable itself or its items.
    FerVarType type;
    /// Maximum number of items in the variable. 0 if variable is scalar.
    size_t max_len;
} FerVarInfo;

/// Variable value.
/// Contents (where T is the type of items):
/// + Scalar: { value: T }
/// + Vector: { len: usize, data: [T] }
/// Items of T are properly aligned.
typedef struct FerVarValue FerVarValue;

/// Initialize application.
extern void fer_app_init();
/// All initialization complete, safe to start operating.
extern void fer_app_start();
/// Termination request from application.
void fer_app_exit(int code);

/// Initialize variable.
extern void fer_var_init(FerVar *var);
/// Request variable processing (read or write).
void fer_var_request(FerVar *var);
/// Asynchronous variable processing begin.
/// NOTE: Variable passed to this function is automatically locked during the call.
extern void fer_var_proc_begin(FerVar *var);
/// Notify that variable asynchronous processing complete.
void fer_var_commit(FerVar *var, FerVarAction action);
/// Asynchronous variable processing end.
/// NOTE: Variable passed to this function is automatically locked during the call.
extern void fer_var_proc_end(FerVar *var);

/// Lock variable.
void fer_var_lock(FerVar *var);
/// Unlock variable.
void fer_var_unlock(FerVar *var);

/// Variable name.
const char *fer_var_name(FerVar *var);
/// Variable permissions.
FerVarInfo fer_var_info(FerVar *var);
/// Get pointer to variable value.
/// *Requires variable to be locked.*
FerVarValue *fer_var_value(FerVar *var);

/// Current number of items in `data`.
/// + For scalars NULL is returned.
/// + For arrays must be less or equal to `FerVarType::max_len`. Must be set on write.
size_t *fer_var_value_len(FerVar *var);
/// Variable data that must be interpreted according to variable type (T) and length.
/// + For scalars it is simply T.
/// + For arrays it is [T; len].
void *fer_var_value_data(FerVar *var);

/// Get user data.
void *fer_var_user_data(FerVar *var);
/// Set user data.
void fer_var_set_user_data(FerVar *var, void *user_data);
