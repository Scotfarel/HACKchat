#ifndef CLIENT_H
#define CLIENT_H
#include <google/protobuf/message.h>
#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>

#include "../proto/client.pb.h"

using hackchat::FromClient;

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
    QList<int> users_online;
    QString nickname;
    int id;
    void first_msg(const FromClient& msg);
private slots:
    void on_pushButton_clicked();
    void on_lineEdit_textEdited(const QString &arg1);
    void on_pushButton_2_pressed();
};

#endif // CLIENT_H
