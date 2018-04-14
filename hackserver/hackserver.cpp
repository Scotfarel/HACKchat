#include "hackserver.h"
#include "ui_hackserver.h"

hackserver::hackserver(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::hackserver)
{
    ui->setupUi(this);
    tcpServ = new QTcpServer(this);
    tcpSock = new QTcpSocket(this);

    tcpServ->listen(QHostAddress::LocalHost, 8888);
    connect(tcpServ, SIGNAL(newConnection()), this, SLOT(connect_new()));
}

hackserver::~hackserver()
{
    delete ui;
}

void hackserver::connect_new() {
    tcpSock = tcpServ->nextPendingConnection();
    tcpSock->write("hello there\n");
    connect(tcpSock, SIGNAL(readyRead()), this, SLOT(read()));
}

void hackserver::read(){
    QByteArray buffer;
    buffer.resize(tcpSock->bytesAvailable());
    tcpSock->read(buffer.data(), buffer.size());
    ui->plainTextEdit->setReadOnly( true );
    ui->plainTextEdit->appendPlainText( QString (buffer));
}


void hackserver::on_pushButton_released()
{

    tcpSock->write( ui->lineEdit->text().toLatin1().data(), ui->lineEdit->text().size());
    ui->lineEdit->clear();
}
