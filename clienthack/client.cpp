#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    tcpSock = new QTcpSocket(this);
    tcpSock->connectToHost(QHostAddress::LocalHost, 8888);
    connect(tcpSock, SIGNAL(readyRead()), this, SLOT(leer()));
}

Client::~Client() {
    delete ui;
}

void Client::on_pushButton_clicked() {
    tcpSock->write( ui->lineEdit->text().toLatin1().data(), ui->lineEdit->text().size());
    ui->lineEdit->clear();
}

void Client::leer() {
    QByteArray buffer;
    buffer.resize( tcpSock->bytesAvailable() );
    tcpSock->read( buffer.data(), buffer.size() );
    ui->plainTextEdit->setReadOnly( true );
    ui->plainTextEdit->appendPlainText( QString (buffer));
}
