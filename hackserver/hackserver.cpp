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
    sock->write("hello there\n");
    connect(sock, SIGNAL(readyRead()), this, SLOT(read()));
}

void hackserver::read(){
    QByteArray buffer;
    buffer.resize(clients_map[0]->bytesAvailable());
    clients_map[0]->read(buffer.data(), buffer.size());
    ui->plainTextEdit->setReadOnly( true );
    ui->plainTextEdit->appendPlainText( QString (buffer));
}


void hackserver::on_pushButton_released()
{

    clients_map[0]->write( ui->lineEdit->text().toLatin1().data(), ui->lineEdit->text().size());
    ui->lineEdit->clear();
}
