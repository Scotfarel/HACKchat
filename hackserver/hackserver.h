#ifndef HACKSERVER_H
#define HACKSERVER_H

#include <google/protobuf/message.h>
#include <map>
#include <string.h>
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QListWidgetItem>

#include "../proto/hackmessage.pb.h"

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
    int client_number = 0;
    std::map<int, QTcpSocket*> clients_map;
    void send_everyone_new(int id);
    void send_everyone_disconnected(int id);
};

#endif // HACKSERVER_H
