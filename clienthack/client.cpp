#include "client.h"
#include "ui_client.h"

hackchat::Message *msg = src.add_message();


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
    QByteArray byte_Array = readSerializedPersonFromQTcpSocket();
    Message msg;
    if (!msg.ParseFromArray(byte_Array, byte_Array.size())) {
      std::cerr << "Failed to parse person.pb." << std::endl;
    }
    ui->plainTextEdit->setReadOnly( true );
    ui->plainTextEdit->appendPlainText( QString (buffer));
}

void Client::on_lineEdit_textEdited(const QString &arg1)
{

    // HERE! RIGHT HERE!
    Message msg;
    msg.set_sender_id();
    msg.set_host_id();
    msg.set_msg_text(tcpSock->write(send.toLatin1().data(), ui->lineEdit->text().size() + 1));

    msg.SerializeToOstream(tcpSock);
    QByteArray byte_Array(msg.SerializeAsString().c_str(), msg.ByteSize());

    qDebug(arg1.toLatin1().data());

    // also u can show text this way:
    ui->feature->setPlainText(arg1);
}
