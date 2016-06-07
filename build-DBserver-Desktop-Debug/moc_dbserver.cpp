/****************************************************************************
** Meta object code from reading C++ file 'dbserver.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../DBserver/dbserver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dbserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_dbServer_t {
    QByteArrayData data[16];
    char stringdata[172];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_dbServer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_dbServer_t qt_meta_stringdata_dbServer = {
    {
QT_MOC_LITERAL(0, 0, 8),
QT_MOC_LITERAL(1, 9, 15),
QT_MOC_LITERAL(2, 25, 0),
QT_MOC_LITERAL(3, 26, 11),
QT_MOC_LITERAL(4, 38, 6),
QT_MOC_LITERAL(5, 45, 3),
QT_MOC_LITERAL(6, 49, 14),
QT_MOC_LITERAL(7, 64, 18),
QT_MOC_LITERAL(8, 83, 20),
QT_MOC_LITERAL(9, 104, 5),
QT_MOC_LITERAL(10, 110, 2),
QT_MOC_LITERAL(11, 113, 19),
QT_MOC_LITERAL(12, 133, 23),
QT_MOC_LITERAL(13, 157, 4),
QT_MOC_LITERAL(14, 162, 3),
QT_MOC_LITERAL(15, 166, 4)
    },
    "dbServer\0frameExtRecived\0\0QTcpSocket*\0"
    "socket\0msg\0frameDBRecived\0frameClientRecived\0"
    "frameExtRecivedError\0error\0ip\0"
    "frameDBRecivedError\0frameClientRecivedError\0"
    "stop\0log\0what\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_dbServer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   54,    2, 0x0a,
       6,    2,   59,    2, 0x0a,
       7,    2,   64,    2, 0x0a,
       8,    2,   69,    2, 0x0a,
      11,    2,   74,    2, 0x0a,
      12,    2,   79,    2, 0x0a,
      13,    0,   84,    2, 0x0a,
      14,    1,   85,    2, 0x0a,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QStringList,    4,    5,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QStringList,    4,    5,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QStringList,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,

       0        // eod
};

void dbServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        dbServer *_t = static_cast<dbServer *>(_o);
        switch (_id) {
        case 0: _t->frameExtRecived((*reinterpret_cast< QTcpSocket*(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2]))); break;
        case 1: _t->frameDBRecived((*reinterpret_cast< QTcpSocket*(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2]))); break;
        case 2: _t->frameClientRecived((*reinterpret_cast< QTcpSocket*(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2]))); break;
        case 3: _t->frameExtRecivedError((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 4: _t->frameDBRecivedError((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 5: _t->frameClientRecivedError((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: _t->stop(); break;
        case 7: _t->log((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTcpSocket* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTcpSocket* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTcpSocket* >(); break;
            }
            break;
        }
    }
}

const QMetaObject dbServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_dbServer.data,
      qt_meta_data_dbServer,  qt_static_metacall, 0, 0}
};


const QMetaObject *dbServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *dbServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_dbServer.stringdata))
        return static_cast<void*>(const_cast< dbServer*>(this));
    return QObject::qt_metacast(_clname);
}

int dbServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
