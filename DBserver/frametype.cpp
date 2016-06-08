#include<frametype.h>

QString FrameType::UNDEFINED = "-1";
QString FrameType::ERROR = "ERROR";
QString FrameType::SERVER_STATUS_OK = "STATUS_OK";
QString FrameType::OK = "OK";

//DB - DB
QString FrameType::STATUS = "STATUS";
QString FrameType::ACTIVE_SERVERS_DB = "ACTIVE_SERVERS_DB";
QString FrameType::ACTIVE_SERVERS_EXT = "ACTIVE_SERVERS_EXT";
QString FrameType::ELECTION = "ELECTION";
QString FrameType::ELECTION_STOP = "ELECTION_STOP";
QString FrameType::COORDINATOR = "COORDINATOR";
QString FrameType::UPLOAD = "UPLOAD";
QString FrameType::INSERT = "INSERT";
QString FrameType::ATTACH = "ATTACH";
QString FrameType::DELETE = "DELETE";
QString FrameType::UNLINK = "UNLINK";

//DB-Ext
QString FrameType::GET_ACTIVE_SERVERS_DB="GET_ACTIVE_SERVERS_DB";
QString FrameType::GET_ACTIVE_SERVERS_EXT="GET_ACTIVE_SERVERS_EXT";
QString FrameType::GET_AVAILABLE_RESULTS = "GET_AVAILABLE_RESULTS";
QString FrameType::RESULTS = "RESULTS";
QString FrameType::RESULT = "RESULT";
QString FrameType::GET_RESULT = "GET_RESULT";
QString FrameType::GET_STATISTICS = "GET_STATISTICS";
QString FrameType::STATISTICS = "STATISTICS";
