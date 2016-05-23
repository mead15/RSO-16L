#ifndef FRAMETYPE_H
#define FRAMETYPE_H

class FrameType {
public:
    enum Type {
        UNDEFINED = "-1",
        ERROR = "ERROR",

        CLIENT_GET_RESULTS = "GET_AVAILABLE_RESULTS",
        CLIENT_RESULTS = "RESULTS",
        CLIENT_GET_EXFILE = "GET_RESULT",
        CLIENT_EXFILE = "RESULT",
        CLIENT_GET_STAT = "GET_STATISTICS",
        CLIENT_STAT = "STATISTICS",
        CLIENT_SERVERS = "ACTIVE_SERVERS",
        //
        SERVER_STATUS = "STATUS", //check if alive
        SERVER_STATUS_OK = "STATUS_OK",
        SERVER_ACTIVE_SERVERS = "ACTIVE_SERVERS_EXT",
        SERVER_ACTIVE_SERVDB = "ACTIVE_SERVERS_DB",
        //elekja
        SERVER_ELECT = "ELECTION",
        SERVER_ELECT_STOP = "ELECTION_STOP",
        SERVER_COORDINATOR = "COORDINATOR",
        //DB
        SERVDB_UPLOAD = "UPLOAD",
        SERVDB_INSERT = "INSERT",
        SERVDB_ATTACH = "ATTACH",
        SERVDB_DELETE = "DELETE",
        SERVDB_UNLINK = "UNLINK",
        //...
    };
};

#endif // FRAMETYPE_H
