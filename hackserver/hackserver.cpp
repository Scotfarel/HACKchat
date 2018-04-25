#include "hackserver.h"
#include "ui_hackserver.h"

using hackchat::FromClient;

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
    FromClient msg;
    msg.set_sender_id(-1);
    msg.set_host_id(client_number);
    msg.set_is_feature(false);
    std::string online;
    for (auto it = clients_map.begin(); it != clients_map.end(); ++it) {
        online += it->first + '0';
        online += ' ';
    }
    msg.set_msg_text(online);
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    sock->write(f_message);
    connect(sock, SIGNAL(readyRead()), this, SLOT(read()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(disconnect()));
    send_everyone_new(client_number);
    client_number++;
}

void hackserver::send_everyone_new(int id) {
    FromClient msg;
    msg.set_sender_id(-1);
    msg.set_is_feature(false);
    msg.set_msg_text(std::string(1, id + '0')); // not perfect
    for (auto it = clients_map.begin(); it != clients_map.end(); ++it) {
        if (it->first == id) {
            continue;
        }
        msg.set_host_id(it->first);
        QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
        it->second->write(f_message);
    }
}

void hackserver::read() {
    QTcpSocket* client = (QTcpSocket*)sender();
    QByteArray buffer;
    buffer.resize(client->bytesAvailable());
    client->read(buffer.data(), buffer.size());
    FromClient msg;
    if (!msg.ParseFromArray(buffer, buffer.size())) {
      qDebug() << "Failed to parse message.";
    }
    std::string text = std::to_string(msg.sender_id());
    text.append(" : ");
    text.append(msg.msg_text());
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
    FromClient msg;
    msg.set_sender_id(100);
    msg.set_host_id(0);
    msg.set_is_feature(false);
    msg.set_msg_text(ui->lineEdit->text().toStdString());
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    for (std::pair<int const, QTcpSocket*>& c : clients_map) {
        c.second->write(f_message, f_message.size());
    }
    ui->lineEdit->clear();
}
