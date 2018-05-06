#ifndef HACKSERVER_H
#define HACKSERVER_H

#include <google/protobuf/message.h>
#include <map>
#include <string.h>
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "../proto/hackmessage.pb.h"

using hackchat::Package;
using hackchat::TextMsg;
using hackchat::StatusMsg;
using hackchat::PackageList;

namespace Ui {
class hackserver;
}

class hackserver : public QMainWindow
{
    Q_OBJECT

public:
    explicit hackserver(QWidget *parent = 0);
    ~hackserver();

public slots:
    void connect_new();
    void read();
    void disconnect();

private slots:
    void on_pushButton_released();

private:
    Ui::hackserver* ui;
    QTcpServer* tcpServ;
    QSqlDatabase db;
    int client_number = 1;
    std::map<int, QTcpSocket*> clients_map;
    void send_everyone_new(int id, std::string login);
    void send_everyone_disconnected(int id);
    bool auth(const Package& msg, QTcpSocket* user);

};

#endif // HACKSERVER_H
