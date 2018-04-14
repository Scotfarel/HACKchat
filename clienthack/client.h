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
private slots:
    void on_pushButton_clicked();
};

#endif // CLIENT_H
