#ifndef CLIENT_H
#define CLIENT_H
#include <google/protobuf/message.h>
#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>

#include "../proto/hackmessage.pb.h"

using hackchat::Package;
using hackchat::TextMsg;
using hackchat::StatusMsg;
using hackchat::PackageList;

namespace Ui {
class Client;
}

class Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

public slots:
    void leer();

private:
    Ui::Client *ui;
    QTcpSocket *tcpSock;
    QMap<int, QString> users_online;
    QString nickname;
    bool first_connect = false; // i will change it
    int id;
    void msg_from_server(const Package& msg);
    void send_user_info();
private slots:
    void on_pushButton_clicked();
    void on_lineEdit_textEdited(const QString &arg1);
    void on_pushButton_2_pressed();
    void on_nickname_line_textEdited();
    void on_password_line_textEdited();
    void on_lineEdit_returnPressed();
};

#endif // CLIENT_H
