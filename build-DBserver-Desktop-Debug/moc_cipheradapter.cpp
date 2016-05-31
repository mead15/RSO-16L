/****************************************************************************
** Meta object code from reading C++ file 'cipheradapter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../communication/cipheradapter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cipheradapter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CipherAdapter_t {
    QByteArrayData data[16];
    char stringdata[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_CipherAdapter_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_CipherAdapter_t qt_meta_stringdata_CipherAdapter = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 12),
QT_MOC_LITERAL(2, 27, 0),
QT_MOC_LITERAL(3, 28, 11),
QT_MOC_LITERAL(4, 40, 3),
QT_MOC_LITERAL(5, 44, 5),
QT_MOC_LITERAL(6, 50, 9),
QT_MOC_LITERAL(7, 60, 12),
QT_MOC_LITERAL(8, 73, 9),
QT_MOC_LITERAL(9, 83, 4),
QT_MOC_LITERAL(10, 88, 7),
QT_MOC_LITERAL(11, 96, 3),
QT_MOC_LITERAL(12, 100, 17),
QT_MOC_LITERAL(13, 118, 6),
QT_MOC_LITERAL(14, 125, 8),
QT_MOC_LITERAL(15, 134, 10)
    },
    "CipherAdapter\0frameContent\0\0QTcpSocket*\0"
    "log\0error\0sendFrame\0QHostAddress\0"
    "recipient\0port\0content\0key\0catchFrameContent\0"
    "socket\0catchLog\0catchError\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CipherAdapter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   54,    2, 0x06,
       4,    1,   59,    2, 0x06,
       5,    1,   62,    2, 0x06,

 // slots: name, argc, parameters, tag, flags
       6,    4,   65,    2, 0x0a,
       6,    3,   74,    2, 0x0a,
      12,    2,   81,    2, 0x08,
      14,    1,   86,    2, 0x08,
      15,    1,   89,    2, 0x08,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QStringList,    2,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 7, QMetaType::Int, QMetaType::QStringList, QMetaType::QString,    8,    9,   10,   11,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QStringList, QMetaType::QString,    8,   10,   11,
    QMetaType::Void, 0x80000000 | 3, QMetaType::QStringList,   13,   10,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   10,

       0        // eod
};

void CipherAdapter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CipherAdapter *_t = static_cast<CipherAdapter *>(_o);
        switch (_id) {
        case 0: _t->frameContent((*reinterpret_cast< QTcpSocket*(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2]))); break;
        case 1: _t->log((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->sendFrame((*reinterpret_cast< QHostAddress(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QStringList(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        case 4: _t->sendFrame((*reinterpret_cast< QTcpSocket*(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 5: _t->catchFrameContent((*reinterpret_cast< QTcpSocket*(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2]))); break;
        case 6: _t->catchLog((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->catchError((*reinterpret_cast< QString(*)>(_a[1]))); break;
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
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTcpSocket* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTcpSocket* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CipherAdapter::*_t)(QTcpSocket * , QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CipherAdapter::frameContent)) {
                *result = 0;
            }
        }
        {
            typedef void (CipherAdapter::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CipherAdapter::log)) {
                *result = 1;
            }
        }
        {
            typedef void (CipherAdapter::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CipherAdapter::error)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject CipherAdapter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CipherAdapter.data,
      qt_meta_data_CipherAdapter,  qt_static_metacall, 0, 0}
};


const QMetaObject *CipherAdapter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CipherAdapter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CipherAdapter.stringdata))
        return static_cast<void*>(const_cast< CipherAdapter*>(this));
    return QObject::qt_metacast(_clname);
}

int CipherAdapter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void CipherAdapter::frameContent(QTcpSocket * _t1, QStringList _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CipherAdapter::log(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CipherAdapter::error(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
