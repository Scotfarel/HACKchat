#ifndef HACKSERVER_H
#define HACKSERVER_H

#include <google/protobuf/message.h>
#include <google/protobuf/util/time_util.h>
#include <time.h>
#include <map>
#include <string.h>
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "../proto/hackmessage.pb.h"
#include "userhandler.h"
#include "userbuilder.h"
#include "messagehandler.h"
#include "messagebuilder.h"
#include "objectdao.h"

using hackchat::Package;
using hackchat::TextMsg;
using hackchat::StatusMsg;
using hackchat::PackageList;
using google::protobuf::Timestamp;
using google::protobuf::util::TimeUtil;

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
    bool register_user(const Package& msg, QTcpSocket* user);

};

#endif // HACKSERVER_H
