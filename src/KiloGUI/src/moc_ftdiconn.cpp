/****************************************************************************
** Meta object code from reading C++ file 'ftdiconn.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ftdiconn.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ftdiconn.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_FTDIConnection_t {
    QByteArrayData data[16];
    char stringdata0[193];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FTDIConnection_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FTDIConnection_t qt_meta_stringdata_FTDIConnection = {
    {
QT_MOC_LITERAL(0, 0, 14), // "FTDIConnection"
QT_MOC_LITERAL(1, 15, 15), // "received_packet"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 25), // "serial_packet_tracking_t*"
QT_MOC_LITERAL(4, 58, 13), // "received_text"
QT_MOC_LITERAL(5, 72, 6), // "status"
QT_MOC_LITERAL(6, 79, 13), // "process_trace"
QT_MOC_LITERAL(7, 93, 5), // "error"
QT_MOC_LITERAL(8, 99, 21), // "requestModuleBootPage"
QT_MOC_LITERAL(9, 121, 22), // "requestKilobotBootPage"
QT_MOC_LITERAL(10, 144, 14), // "canSendCommand"
QT_MOC_LITERAL(11, 159, 4), // "open"
QT_MOC_LITERAL(12, 164, 8), // "baudrate"
QT_MOC_LITERAL(13, 173, 5), // "close"
QT_MOC_LITERAL(14, 179, 4), // "read"
QT_MOC_LITERAL(15, 184, 8) // "readLoop"

    },
    "FTDIConnection\0received_packet\0\0"
    "serial_packet_tracking_t*\0received_text\0"
    "status\0process_trace\0error\0"
    "requestModuleBootPage\0requestKilobotBootPage\0"
    "canSendCommand\0open\0baudrate\0close\0"
    "read\0readLoop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FTDIConnection[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,
       4,    1,   77,    2, 0x06 /* Public */,
       5,    1,   80,    2, 0x06 /* Public */,
       6,    1,   83,    2, 0x06 /* Public */,
       7,    1,   86,    2, 0x06 /* Public */,
       8,    1,   89,    2, 0x06 /* Public */,
       9,    1,   92,    2, 0x06 /* Public */,
      10,    0,   95,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    1,   96,    2, 0x0a /* Public */,
      13,    0,   99,    2, 0x0a /* Public */,
      14,    0,  100,    2, 0x0a /* Public */,
      15,    0,  101,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::UChar,    2,
    QMetaType::Void, QMetaType::UChar,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void FTDIConnection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FTDIConnection *_t = static_cast<FTDIConnection *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->received_packet((*reinterpret_cast< serial_packet_tracking_t*(*)>(_a[1]))); break;
        case 1: _t->received_text((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->status((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->process_trace((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->requestModuleBootPage((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 6: _t->requestKilobotBootPage((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 7: _t->canSendCommand(); break;
        case 8: _t->open((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->close(); break;
        case 10: _t->read(); break;
        case 11: _t->readLoop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (FTDIConnection::*_t)(serial_packet_tracking_t * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FTDIConnection::received_packet)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (FTDIConnection::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FTDIConnection::received_text)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (FTDIConnection::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FTDIConnection::status)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (FTDIConnection::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FTDIConnection::process_trace)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (FTDIConnection::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FTDIConnection::error)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (FTDIConnection::*_t)(quint8 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FTDIConnection::requestModuleBootPage)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (FTDIConnection::*_t)(quint8 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FTDIConnection::requestKilobotBootPage)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (FTDIConnection::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FTDIConnection::canSendCommand)) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject FTDIConnection::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_FTDIConnection.data,
      qt_meta_data_FTDIConnection,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *FTDIConnection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FTDIConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FTDIConnection.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FTDIConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void FTDIConnection::received_packet(serial_packet_tracking_t * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FTDIConnection::received_text(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void FTDIConnection::status(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void FTDIConnection::process_trace(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void FTDIConnection::error(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void FTDIConnection::requestModuleBootPage(quint8 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void FTDIConnection::requestKilobotBootPage(quint8 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void FTDIConnection::canSendCommand()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
