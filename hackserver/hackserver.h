#ifndef HACKSERVER_H
#define HACKSERVER_H

#include <map>
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QListWidgetItem>

namespace Ui {
class hackserver;
}

class hackserver : public QMainWindow
{
    Q_OBJECT

public:
    explicit hackserver(QWidget *parent = 0);
    QStringList getOnlineUsers(QTcpSocket* current = nullptr);
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

};

#endif // HACKSERVER_H
