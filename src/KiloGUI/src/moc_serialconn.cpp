/****************************************************************************
** Meta object code from reading C++ file 'serialconn.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "serialconn.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialconn.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SerialConnection_t {
    QByteArrayData data[12];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialConnection_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialConnection_t qt_meta_stringdata_SerialConnection = {
    {
QT_MOC_LITERAL(0, 0, 16), // "SerialConnection"
QT_MOC_LITERAL(1, 17, 8), // "readText"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 6), // "status"
QT_MOC_LITERAL(4, 34, 5), // "error"
QT_MOC_LITERAL(5, 40, 11), // "sendCommand"
QT_MOC_LITERAL(6, 52, 11), // "sendProgram"
QT_MOC_LITERAL(7, 64, 4), // "open"
QT_MOC_LITERAL(8, 69, 5), // "close"
QT_MOC_LITERAL(9, 75, 4), // "read"
QT_MOC_LITERAL(10, 80, 8), // "readLoop"
QT_MOC_LITERAL(11, 89, 11) // "programLoop"

    },
    "SerialConnection\0readText\0\0status\0"
    "error\0sendCommand\0sendProgram\0open\0"
    "close\0read\0readLoop\0programLoop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialConnection[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       3,    1,   67,    2, 0x06 /* Public */,
       4,    1,   70,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   73,    2, 0x0a /* Public */,
       6,    1,   76,    2, 0x0a /* Public */,
       7,    0,   79,    2, 0x0a /* Public */,
       8,    0,   80,    2, 0x0a /* Public */,
       9,    0,   81,    2, 0x0a /* Public */,
      10,    0,   82,    2, 0x08 /* Private */,
      11,    0,   83,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SerialConnection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SerialConnection *_t = static_cast<SerialConnection *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->readText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->status((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->sendCommand((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 4: _t->sendProgram((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->open(); break;
        case 6: _t->close(); break;
        case 7: _t->read(); break;
        case 8: _t->readLoop(); break;
        case 9: _t->programLoop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (SerialConnection::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialConnection::readText)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SerialConnection::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialConnection::status)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (SerialConnection::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialConnection::error)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject SerialConnection::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SerialConnection.data,
      qt_meta_data_SerialConnection,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SerialConnection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SerialConnection.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SerialConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void SerialConnection::readText(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SerialConnection::status(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SerialConnection::error(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
