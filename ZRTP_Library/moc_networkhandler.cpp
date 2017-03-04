/****************************************************************************
** Meta object code from reading C++ file 'networkhandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "networkhandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'networkhandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NetworkHandler_t {
    QByteArrayData data[23];
    char stringdata[289];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_NetworkHandler_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_NetworkHandler_t qt_meta_stringdata_NetworkHandler = {
    {
QT_MOC_LITERAL(0, 0, 14),
QT_MOC_LITERAL(1, 15, 20),
QT_MOC_LITERAL(2, 36, 0),
QT_MOC_LITERAL(3, 37, 19),
QT_MOC_LITERAL(4, 57, 7),
QT_MOC_LITERAL(5, 65, 4),
QT_MOC_LITERAL(6, 70, 14),
QT_MOC_LITERAL(7, 85, 14),
QT_MOC_LITERAL(8, 100, 8),
QT_MOC_LITERAL(9, 109, 4),
QT_MOC_LITERAL(10, 114, 4),
QT_MOC_LITERAL(11, 119, 20),
QT_MOC_LITERAL(12, 140, 20),
QT_MOC_LITERAL(13, 161, 13),
QT_MOC_LITERAL(14, 175, 12),
QT_MOC_LITERAL(15, 188, 12),
QT_MOC_LITERAL(16, 201, 10),
QT_MOC_LITERAL(17, 212, 10),
QT_MOC_LITERAL(18, 223, 12),
QT_MOC_LITERAL(19, 236, 12),
QT_MOC_LITERAL(20, 249, 10),
QT_MOC_LITERAL(21, 260, 14),
QT_MOC_LITERAL(22, 275, 12)
    },
    "NetworkHandler\0readPendingDatagrams\0"
    "\0parseInputArguments\0char*[]\0argv\0"
    "processTimeOut\0processMessage\0uint8_t*\0"
    "data\0size\0processTimeoutInZrtp\0"
    "startZrtpNegotiation\0endAplication\0"
    "eraseThreads\0setAddresses\0Addresses*\0"
    "_addresses\0_sendingPort\0QHostAddress\0"
    "_sendingIP\0_receivingPort\0_receivingIP\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NetworkHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x0a,
       3,    1,   60,    2, 0x0a,
       6,    0,   63,    2, 0x0a,
       7,    2,   64,    2, 0x0a,
      11,    0,   69,    2, 0x0a,
      12,    0,   70,    2, 0x0a,
      13,    0,   71,    2, 0x0a,
      14,    0,   72,    2, 0x0a,
      15,    5,   73,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8, QMetaType::UShort,    9,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16, QMetaType::UShort, 0x80000000 | 19, QMetaType::UShort, 0x80000000 | 19,   17,   18,   20,   21,   22,

       0        // eod
};

void NetworkHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NetworkHandler *_t = static_cast<NetworkHandler *>(_o);
        switch (_id) {
        case 0: _t->readPendingDatagrams(); break;
        case 1: _t->parseInputArguments((*reinterpret_cast< char*(*)[]>(_a[1]))); break;
        case 2: _t->processTimeOut(); break;
        case 3: _t->processMessage((*reinterpret_cast< uint8_t*(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2]))); break;
        case 4: _t->processTimeoutInZrtp(); break;
        case 5: _t->startZrtpNegotiation(); break;
        case 6: _t->endAplication(); break;
        case 7: _t->eraseThreads(); break;
        case 8: _t->setAddresses((*reinterpret_cast< Addresses*(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< QHostAddress(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4])),(*reinterpret_cast< QHostAddress(*)>(_a[5]))); break;
        default: ;
        }
    }
}

const QMetaObject NetworkHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NetworkHandler.data,
      qt_meta_data_NetworkHandler,  qt_static_metacall, 0, 0}
};


const QMetaObject *NetworkHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkHandler.stringdata))
        return static_cast<void*>(const_cast< NetworkHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int NetworkHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
