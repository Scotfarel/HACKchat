#include "hackserver.h"
#include "ui_hackserver.h"

hackserver::hackserver(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::hackserver)
{
    ui->setupUi(this);
    tcpServ = new QTcpServer(this);


    tcpServ->listen(QHostAddress::LocalHost, 8888);
    connect(tcpServ, SIGNAL(newConnection()), this, SLOT(connect_new()));
}

hackserver::~hackserver()
{
    delete ui;
}

void hackserver::connect_new() {
    QTcpSocket* sock = tcpServ->nextPendingConnection();
    clients_map.emplace(client_number, sock);
    ui->users->addItem(QString(client_number+'0'));
    client_number++;
    sock->write("hello there number\n");
    connect(sock, SIGNAL(readyRead()), this, SLOT(read()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(disconnect()));
    QByteArray users_on;
    QStringList online = getOnlineUsers();
    foreach (const QString &str, online)
    {
        users_on.append(str);
    }
    sock->write(users_on);
}

QStringList hackserver::getOnlineUsers(QTcpSocket* current) {
    QStringList online;
    for (auto c : clients_map) {
        if (c.second == current) {
            continue;
        }
        online.append(QString(c.first + '0'));
    }
    return online;
}

void hackserver::read(){
    QByteArray buffer;
    QTcpSocket* client = (QTcpSocket*)sender();
    buffer.resize(client->bytesAvailable());
    client->read(buffer.data(), buffer.size());
    ui->plainTextEdit->setReadOnly( true );
    ui->plainTextEdit->appendPlainText( QString (buffer));
}

void hackserver::disconnect() {
    QTcpSocket* disconnected_client = static_cast<QTcpSocket*>(sender());
    int num = -1;
    for (auto it = clients_map.begin(); it != clients_map.end(); ++it) {
      if (it->second == disconnected_client) {
          num = it->first;
      }
    }
    if (num == -1){
        qDebug() << "Not online!";
    }
    for (int i = 0; i < ui->users->count(); i++) {
        if (QString::compare(ui->users->item(i)->text(), QString(num + '0')) == 0) {
            delete ui->users->item(i);
        }
    }
    clients_map.erase(num);
}


void hackserver::on_pushButton_released()
{
    for (std::pair<int const, QTcpSocket*>& c : clients_map) {
        c.second->write( ui->lineEdit->text().toLatin1().data(), ui->lineEdit->text().size());
    }
    ui->lineEdit->clear();
}
