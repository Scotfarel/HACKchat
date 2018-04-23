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

    // hard hardcode
    users_online = QList<int>() << 0 << 1 << 2 << 3;
    for (auto u : users_online) {
        ui->online->addItem(QString(u+'0'));
    }
}

Client::~Client() {
    delete ui;
}

void Client::on_pushButton_clicked() {
    // hardcode part 2
    QListWidgetItem* selected = ui->online->selectedItems().at(0);
    QString send = selected->text();
    send.append(ui->lineEdit->text());
    tcpSock->write(send.toLatin1().data(), ui->lineEdit->text().size() + 1);
    ui->lineEdit->clear();
}

void Client::leer() {
    QByteArray buffer;
    buffer.resize( tcpSock->bytesAvailable() );
    tcpSock->read( buffer.data(), buffer.size() );
    ui->plainTextEdit->setReadOnly( true );
    ui->plainTextEdit->appendPlainText( QString (buffer));
}

void Client::on_lineEdit_textEdited(const QString &arg1)
{
    // HERE! RIGHT HERE!
    qDebug(arg1.toLatin1().data());
    // also u can show text this way:
    ui->feature->setPlainText(arg1);
}
