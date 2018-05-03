#include "hackserver.h"
#include "ui_hackserver.h"

using hackchat::Package;
using hackchat::TextMsg;
using hackchat::StatusMsg;

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
    ui->users->addItem(QString::number(client_number));
    connect(sock, SIGNAL(readyRead()), this, SLOT(read()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(disconnect()));
    Package msg;
    msg.set_sender_id(-1);
    msg.set_host_id(client_number);
    StatusMsg* status = new StatusMsg;
    status->set_connected(true);
    msg.set_allocated_status(status);
    for (auto it = clients_map.begin(); it != clients_map.end(); ++it) {
            status->set_connected_id(it->first);
            QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
            sock->write(f_message);
            qDebug() << "sended" << msg.status().connected_id() << "to" << client_number << "can u" << f_message;
    }
    send_everyone_new(client_number);
    clients_map.emplace(client_number, sock);
    client_number++;
}

void hackserver::send_everyone_new(int id) {
    Package msg;
    msg.set_sender_id(-1);
    StatusMsg* status = new StatusMsg;
    status->set_connected(true);
    status->set_connected_id(id);
    msg.set_allocated_status(status);
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    for (std::pair<int const, QTcpSocket*>& c : clients_map) {
        c.second->write(f_message, f_message.size());
    }
}

void hackserver::send_everyone_disconnected(int id) {
    Package msg;
    msg.set_sender_id(-1);
    StatusMsg* status = new StatusMsg;
    status->set_connected(false);
    status->set_connected_id(id);
    msg.set_allocated_status(status);
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    for (std::pair<int const, QTcpSocket*>& c : clients_map) {
        c.second->write(f_message, f_message.size());
    }
}

void hackserver::read() {
    QTcpSocket* client = (QTcpSocket*)sender();
    QByteArray buffer;
    buffer.resize(client->bytesAvailable());
    client->read(buffer.data(), buffer.size());
    Package msg;
    if (!msg.ParseFromArray(buffer, buffer.size())) {
      qDebug() << "Failed to parse message.";
    }
    std::string text = std::to_string(msg.sender_id());
    text.append(" : ");
    text.append(msg.text().msg_text());
    ui->plainTextEdit->appendPlainText(QString::fromStdString(text));
    if (msg.host_id() != -1) {
        auto need_to = clients_map.find(msg.host_id());
        need_to->second->write(buffer);
    }
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
        return;
    }
    for (int i = 0; i < ui->users->count(); i++) {
        if (QString::compare(ui->users->item(i)->text(), QString::number(num)) == 0) {
            delete ui->users->item(i);
        }
    }
    clients_map.erase(num);
    send_everyone_disconnected(num);
}


void hackserver::on_pushButton_released()
{
    Package msg;
    msg.set_sender_id(100);
    msg.set_host_id(0);
    TextMsg* text = new TextMsg;
    text->set_is_feature(false);
    text->set_msg_text(ui->lineEdit->text().toStdString());
    msg.set_allocated_text(text);
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    for (std::pair<int const, QTcpSocket*>& c : clients_map) {
        c.second->write(f_message, f_message.size());
    }
    ui->lineEdit->clear();
}
