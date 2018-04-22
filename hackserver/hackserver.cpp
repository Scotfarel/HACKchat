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
    client_number++;
    qDebug() << client_number;
    ui->users->addItem(QString(client_number+'0'));
    sock->write("hello there number\n");
    connect(sock, SIGNAL(readyRead()), this, SLOT(read()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(disconnect()));
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
    QListWidgetItem* item = ui->users->takeItem(num);
    delete item;
}


void hackserver::on_pushButton_released()
{
    for (std::pair<int const, QTcpSocket*>& c : clients_map) {
        c.second->write( ui->lineEdit->text().toLatin1().data(), ui->lineEdit->text().size());
    }
    qDebug() << ui->lineEdit->text().toLatin1().data();
    ui->lineEdit->clear();
}
