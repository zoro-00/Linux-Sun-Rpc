/*
 * fileops.x
 * RPC protocol definition for remote file handling operations
 * (Create, Write, Append, Read, Delete)
 *
 * Compile with: rpcgen fileops.x
 */

const MAXNAMELEN = 256;
const MAXDATALEN = 65536;

struct file_data {
    string fname<MAXNAMELEN>;
    string content<MAXDATALEN>;
};

struct op_result {
    int status;            /* 0 = success, -1 = failure */
    string message<256>;
};

struct read_result {
    int status;             /* 0 = success, -1 = failure */
    string content<MAXDATALEN>;
};

struct list_result {
    int status;             /* 0 = success, -1 = failure */
    string files<MAXDATALEN>;
};

program FILEPROG {
    version FILEVERS {
        op_result   CREATE_FILE(string)     = 1;
        op_result   WRITE_FILE(file_data)   = 2;
        op_result   APPEND_FILE(file_data)  = 3;
        read_result READ_FILE(string)       = 4;
        op_result   DELETE_FILE(string)     = 5;
        op_result   UPLOAD_FILE(file_data)  = 6;
        read_result DOWNLOAD_FILE(string)   = 7;
        list_result LIST_FILES(void)        = 8;
    } = 1;
} = 0x31000099;

