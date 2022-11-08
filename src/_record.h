#pragma once

#include <callback.h>
#include <dbCommon.h>
#include <dbScan.h>

#include "_interface.h"
#include "_variable.h"

/// Record direction.
typedef enum FerEpicsRecordDir {
    FER_EPICS_RECORD_DIR_INPUT,
    FER_EPICS_RECORD_DIR_OUTPUT,
} FerEpicsRecordDir;

typedef void (*FerEpicsRecordStoreFunc)(dbCommon *);
typedef void (*FerEpicsRecordLoadFunc)(dbCommon *);

/// Global record info (one per record type).
typedef struct FerEpicsRecordInfo {
    FerEpicsRecordDir dir;
    FerEpicsRecordStoreFunc store;
    FerEpicsRecordStoreFunc load;
} FerEpicsRecordInfo;

/// Record operation.
/// Whether record value was read or written.
typedef enum FerEpicsRecordOp {
    FER_EPICS_RECORD_OP_READ = 0,
    FER_EPICS_RECORD_OP_WRITE = 1,
} FerEpicsRecordOp;

/// Record processing request information.
typedef struct FerEpicsProcReq {
    epicsCallback callback;
    FerEpicsRecordOp op;
    long status;
} FerEpicsProcReq;

/// Private data to store in a record.
typedef struct FerEpicsRecordDpvt {
    /// Callback to call when async processing is done.
    FerEpicsProcReq process;
    /// Scan list for I/O Intr.
    /// NULL if record scanning is not an `I/O Intr`.
    IOSCANPVT ioscan_list;
    /// Interface variable.
    FerEpicsVar *var;
    /// User data.
    void *user_data;
} FerEpicsRecordDpvt;

/// Initialize record.
void fer_epics_record_init(dbCommon *rec, FerEpicsVar *var);
/// Deinitialize record.
void fer_epics_record_deinit(dbCommon *rec);

/// Get private data from record.
FerEpicsRecordDpvt *fer_epics_record_dpvt(dbCommon *rec);
/// Get interface variable info.
FerEpicsVar *fer_epics_record_var(dbCommon *rec);

/// Initialize process callback.
FerEpicsProcReq fer_epics_proc_req_create(dbCommon *rec);

/// Initialize record scan list.
IOSCANPVT fer_epics_record_ioscan_create(dbCommon *rec);

/// Request record processing
void fer_epics_record_request_proc(dbCommon *rec);
/// Process record.
long fer_epics_record_process(dbCommon *rec, const FerEpicsRecordInfo *info);
/// Notify that process is done.
void fer_epics_record_complete_proc(dbCommon *rec, FerEpicsRecordOp op, long status);
