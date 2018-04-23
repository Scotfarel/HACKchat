#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>

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
private slots:
    void on_pushButton_clicked();
    void on_lineEdit_textEdited(const QString &arg1);
};

#endif // CLIENT_H
