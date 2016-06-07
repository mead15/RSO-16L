#ifndef FRAMETYPE_H
#define FRAMETYPE_H
#include <QString>

class FrameType {
public:
        static QString UNDEFINED;
        static QString ERROR;
        static QString SERVER_STATUS_OK;
        static QString OK;

        //DB - DB
        static QString STATUS;
        static QString ACTIVE_SERVERS_DB;
        static QString ACTIVE_SERVERS_EXT;
        static QString ELECTION;
        static QString ELECTION_STOP;
        static QString COORDINATOR;
        static QString UPLOAD;
        static QString INSERT;
        static QString ATTACH;
        static QString DELETE;
        static QString UNLINK;

        //DB-Ext
        static QString GET_ACTIVE_SERVERS_DB;
        static QString GET_ACTIVE_SERVERS_EXT;
        static QString GET_AVAILABLE_RESULTS;
        static QString RESULTS;
        static QString RESULT;
        static QString GET_RESULT;
        static QString GET_STATISTICS;
        static QString STATISTICS;

        //Client-Ext
        static QString ACTIVE_SERVERS;
};

#endif // FRAMETYPE_H
