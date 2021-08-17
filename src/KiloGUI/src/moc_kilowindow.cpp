/****************************************************************************
** Meta object code from reading C++ file 'kilowindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "kilowindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kilowindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KiloWindow_t {
    QByteArrayData data[45];
    char stringdata0[756];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KiloWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KiloWindow_t qt_meta_stringdata_KiloWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "KiloWindow"
QT_MOC_LITERAL(1, 11, 10), // "selectFTDI"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 10), // "selectVUSB"
QT_MOC_LITERAL(4, 34, 12), // "selectSerial"
QT_MOC_LITERAL(5, 47, 16), // "toggleConnection"
QT_MOC_LITERAL(6, 64, 9), // "showError"
QT_MOC_LITERAL(7, 74, 18), // "serialUpdateStatus"
QT_MOC_LITERAL(8, 93, 18), // "serialSpeedChanged"
QT_MOC_LITERAL(9, 112, 16), // "ftdiUpdateStatus"
QT_MOC_LITERAL(10, 129, 16), // "vusbUpdateStatus"
QT_MOC_LITERAL(11, 146, 24), // "chooseKilobotProgramFile"
QT_MOC_LITERAL(12, 171, 24), // "changeKilobotProgramFile"
QT_MOC_LITERAL(13, 196, 8), // "filename"
QT_MOC_LITERAL(14, 205, 25), // "chooseKilogridProgramFile"
QT_MOC_LITERAL(15, 231, 25), // "changeKilogridProgramFile"
QT_MOC_LITERAL(16, 257, 23), // "chooseConfigurationFile"
QT_MOC_LITERAL(17, 281, 20), // "uploadKilobotProgram"
QT_MOC_LITERAL(18, 302, 21), // "uploadKilogridProgram"
QT_MOC_LITERAL(19, 324, 19), // "kilogrid_disable_IR"
QT_MOC_LITERAL(20, 344, 24), // "kilogrid_disable_command"
QT_MOC_LITERAL(21, 369, 18), // "broadcastIRMessage"
QT_MOC_LITERAL(22, 388, 22), // "broadcastIRDataMessage"
QT_MOC_LITERAL(23, 411, 8), // "uint8_t*"
QT_MOC_LITERAL(24, 420, 17), // "IR_message_type_t"
QT_MOC_LITERAL(25, 438, 19), // "sendKilogridCommand"
QT_MOC_LITERAL(26, 458, 16), // "sendSerialPacket"
QT_MOC_LITERAL(27, 475, 19), // "dispatcherLEDToggle"
QT_MOC_LITERAL(28, 495, 9), // "calibShow"
QT_MOC_LITERAL(29, 505, 7), // "setPort"
QT_MOC_LITERAL(30, 513, 24), // "onExperimentDataReceived"
QT_MOC_LITERAL(31, 538, 25), // "serial_packet_tracking_t*"
QT_MOC_LITERAL(32, 564, 8), // "calibUID"
QT_MOC_LITERAL(33, 573, 9), // "calibLeft"
QT_MOC_LITERAL(34, 583, 10), // "calibRight"
QT_MOC_LITERAL(35, 594, 13), // "calibStraight"
QT_MOC_LITERAL(36, 608, 9), // "calibSave"
QT_MOC_LITERAL(37, 618, 9), // "calibStop"
QT_MOC_LITERAL(38, 628, 18), // "sendModuleBootPage"
QT_MOC_LITERAL(39, 647, 19), // "sendKilobotBootPage"
QT_MOC_LITERAL(40, 667, 7), // "onTimer"
QT_MOC_LITERAL(41, 675, 19), // "sendBufferedCommand"
QT_MOC_LITERAL(42, 695, 18), // "addTextDebugWindow"
QT_MOC_LITERAL(43, 714, 16), // "clearDebugWindow"
QT_MOC_LITERAL(44, 731, 24) // "saveDebugWindowToLogFile"

    },
    "KiloWindow\0selectFTDI\0\0selectVUSB\0"
    "selectSerial\0toggleConnection\0showError\0"
    "serialUpdateStatus\0serialSpeedChanged\0"
    "ftdiUpdateStatus\0vusbUpdateStatus\0"
    "chooseKilobotProgramFile\0"
    "changeKilobotProgramFile\0filename\0"
    "chooseKilogridProgramFile\0"
    "changeKilogridProgramFile\0"
    "chooseConfigurationFile\0uploadKilobotProgram\0"
    "uploadKilogridProgram\0kilogrid_disable_IR\0"
    "kilogrid_disable_command\0broadcastIRMessage\0"
    "broadcastIRDataMessage\0uint8_t*\0"
    "IR_message_type_t\0sendKilogridCommand\0"
    "sendSerialPacket\0dispatcherLEDToggle\0"
    "calibShow\0setPort\0onExperimentDataReceived\0"
    "serial_packet_tracking_t*\0calibUID\0"
    "calibLeft\0calibRight\0calibStraight\0"
    "calibSave\0calibStop\0sendModuleBootPage\0"
    "sendKilobotBootPage\0onTimer\0"
    "sendBufferedCommand\0addTextDebugWindow\0"
    "clearDebugWindow\0saveDebugWindowToLogFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KiloWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      39,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  209,    2, 0x08 /* Private */,
       3,    0,  210,    2, 0x08 /* Private */,
       4,    0,  211,    2, 0x08 /* Private */,
       5,    0,  212,    2, 0x08 /* Private */,
       6,    1,  213,    2, 0x08 /* Private */,
       7,    1,  216,    2, 0x08 /* Private */,
       8,    1,  219,    2, 0x08 /* Private */,
       9,    1,  222,    2, 0x08 /* Private */,
      10,    1,  225,    2, 0x08 /* Private */,
      11,    0,  228,    2, 0x08 /* Private */,
      12,    1,  229,    2, 0x08 /* Private */,
      14,    0,  232,    2, 0x08 /* Private */,
      15,    1,  233,    2, 0x08 /* Private */,
      16,    0,  236,    2, 0x08 /* Private */,
      17,    0,  237,    2, 0x08 /* Private */,
      18,    0,  238,    2, 0x08 /* Private */,
      19,    0,  239,    2, 0x08 /* Private */,
      20,    0,  240,    2, 0x08 /* Private */,
      21,    1,  241,    2, 0x08 /* Private */,
      22,    2,  244,    2, 0x08 /* Private */,
      25,    1,  249,    2, 0x08 /* Private */,
      26,    1,  252,    2, 0x08 /* Private */,
      27,    0,  255,    2, 0x08 /* Private */,
      28,    0,  256,    2, 0x08 /* Private */,
      29,    1,  257,    2, 0x08 /* Private */,
      30,    1,  260,    2, 0x08 /* Private */,
      32,    1,  263,    2, 0x08 /* Private */,
      33,    1,  266,    2, 0x08 /* Private */,
      34,    1,  269,    2, 0x08 /* Private */,
      35,    1,  272,    2, 0x08 /* Private */,
      36,    0,  275,    2, 0x08 /* Private */,
      37,    0,  276,    2, 0x08 /* Private */,
      38,    1,  277,    2, 0x08 /* Private */,
      39,    1,  280,    2, 0x08 /* Private */,
      40,    0,  283,    2, 0x08 /* Private */,
      41,    0,  284,    2, 0x08 /* Private */,
      42,    1,  285,    2, 0x08 /* Private */,
      43,    0,  288,    2, 0x08 /* Private */,
      44,    0,  289,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 23, 0x80000000 | 24,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QByteArray,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 31,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UChar,    2,
    QMetaType::Void, QMetaType::UChar,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void KiloWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KiloWindow *_t = static_cast<KiloWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selectFTDI(); break;
        case 1: _t->selectVUSB(); break;
        case 2: _t->selectSerial(); break;
        case 3: _t->toggleConnection(); break;
        case 4: _t->showError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->serialUpdateStatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->serialSpeedChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->ftdiUpdateStatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->vusbUpdateStatus((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->chooseKilobotProgramFile(); break;
        case 10: _t->changeKilobotProgramFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 11: _t->chooseKilogridProgramFile(); break;
        case 12: _t->changeKilogridProgramFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 13: _t->chooseConfigurationFile(); break;
        case 14: _t->uploadKilobotProgram(); break;
        case 15: _t->uploadKilogridProgram(); break;
        case 16: _t->kilogrid_disable_IR(); break;
        case 17: _t->kilogrid_disable_command(); break;
        case 18: _t->broadcastIRMessage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->broadcastIRDataMessage((*reinterpret_cast< uint8_t*(*)>(_a[1])),(*reinterpret_cast< IR_message_type_t(*)>(_a[2]))); break;
        case 20: _t->sendKilogridCommand((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->sendSerialPacket((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 22: _t->dispatcherLEDToggle(); break;
        case 23: _t->calibShow(); break;
        case 24: _t->setPort((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: _t->onExperimentDataReceived((*reinterpret_cast< serial_packet_tracking_t*(*)>(_a[1]))); break;
        case 26: _t->calibUID((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: _t->calibLeft((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 28: _t->calibRight((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 29: _t->calibStraight((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 30: _t->calibSave(); break;
        case 31: _t->calibStop(); break;
        case 32: _t->sendModuleBootPage((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 33: _t->sendKilobotBootPage((*reinterpret_cast< quint8(*)>(_a[1]))); break;
        case 34: _t->onTimer(); break;
        case 35: _t->sendBufferedCommand(); break;
        case 36: _t->addTextDebugWindow((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 37: _t->clearDebugWindow(); break;
        case 38: _t->saveDebugWindowToLogFile(); break;
        default: ;
        }
    }
}

const QMetaObject KiloWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_KiloWindow.data,
      qt_meta_data_KiloWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *KiloWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KiloWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KiloWindow.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KiloWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 39)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 39;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 39)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 39;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
