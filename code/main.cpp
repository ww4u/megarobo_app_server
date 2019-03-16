#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QThread>

#include "mydebug.h"
#include "./t4/mrx_t4server.h"

//{
//      "FirstName": "John",
//      "LastName": "Doe",
//      "Age": 43,
//      "Address": {
//          "Street": "Downing Street 10",
//          "City": "London",
//          "Country": "Great Britain"
//      },
//      "Phone numbers": [
//          "+44 1234567",
//          "+44 2345678"
//      ]
//  }
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    QFile file("G:/study/data/a.txt");
//    if ( file.open( QIODevice::ReadOnly) )
//    {}
//    else
//    { return -1; }

//    QByteArray ary = file.readAll();

//    file.close();

//    QJsonDocument doc = QJsonDocument::fromJson( ary );

//    qDebug()<<doc.isObject();
//    QJsonObject obj ( doc.object() );
//    qDebug()<<obj.value("firstName").toString();
//    qDebug()<<obj.value("Address").toObject().value("Street");

    logDbg()<<QThread::currentThreadId();

    MRX_T4Server server;

    server.start();

    return a.exec();
}
