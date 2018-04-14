#ifndef HACKSERVER_H
#define HACKSERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

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

private slots:
    void on_pushButton_released();

private:
    Ui::hackserver* ui;
    QTcpServer* tcpServ;
    QTcpSocket* tcpSock;
};

#endif // HACKSERVER_H
