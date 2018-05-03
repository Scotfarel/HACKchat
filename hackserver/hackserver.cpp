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

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/kaito/hackchat/hackserver/hack.db3");
    if(!db.open()) {
        qDebug() << db.lastError().text();
    }
}

hackserver::~hackserver() {
    delete ui;
}

void hackserver::connect_new() {
    QTcpSocket* sock = tcpServ->nextPendingConnection();
    connect(sock, SIGNAL(readyRead()), this, SLOT(read()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(disconnect()));
    /*Package msg;
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
    }*/
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
    qDebug() << "msg u mean:" << QString::fromStdString(text);
    ui->plainTextEdit->appendPlainText(QString::fromStdString(text));
    if (msg.host_id() != -1) {
        auto need_to = clients_map.find(msg.host_id());
        need_to->second->write(buffer);
    }
    if (msg.sender_id() == 0) {
        if (auth(msg, client)) {
            client_number++;
        }
    }
}

bool hackserver::auth(Package& msg, QTcpSocket* user) {
    QStringList info = QString::fromStdString(msg.text().msg_text()).split(' ', QString::SkipEmptyParts);
    QString login = info.at(0);
    QString password = info.at(1);
    QSqlQuery query;
    query.prepare("SELECT * FROM user WHERE login = ? AND p_hash = ?");
    query.bindValue(0, login);
    query.bindValue(1, password);
    if (!query.exec()) {
        qDebug() << query.lastError().text();
        return false;
    }
    Package answer;
    answer.set_sender_id(-1);
    if (!query.next()) {
        answer.set_host_id(0);
        QByteArray f_message(answer.SerializeAsString().c_str(), answer.ByteSize());
        user->write(f_message);
        return false;
    }
    int user_id = query.value("user_id").toInt();

    answer.set_host_id(user_id);
    QByteArray f_message(answer.SerializeAsString().c_str(), answer.ByteSize());
    user->write(f_message);
    send_everyone_new(user_id);
    clients_map.emplace(user_id, user);
    ui->users->addItem(QString::number(user_id));
    return true;
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
