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
    db.setDatabaseName("/home/kaito/hackchat/hackserver/hackdb.db3");
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
}

void hackserver::send_everyone_new(int id, std::string login) {
    ObjectDAO<UserBuilder, UserHandler> obj_dao;
    QVector<QString> friends = obj_dao.get_friends(id);
    PackageList list;
    Package* msg = list.add_pack();
    msg->set_sender_id(-1);
    StatusMsg* status = new StatusMsg;
    status->set_status(StatusMsg::CONNECTED);
    status->set_user_id(id);
    status->set_user_login(login);
    msg->set_allocated_status_msg(status);
    for (int i = 0; i < friends.size(); i++) {
        int friend_id = friends.at(i).toInt();
        if (clients_map.find(friend_id) == clients_map.end()) {
            continue;
        }
        msg->set_host_id(friend_id);
        QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
        clients_map[friend_id]->write(f_message, f_message.size());
    }
}

void hackserver::send_everyone_disconnected(int id) {
    PackageList list;
    Package* msg = list.add_pack();
    msg->set_sender_id(-1);
    StatusMsg* status = new StatusMsg;
    status->set_status(StatusMsg::DISCONNECTED);
    status->set_user_id(id);
    msg->set_allocated_status_msg(status);
    for (std::pair<int const, QTcpSocket*>& c : clients_map) {
        msg->set_host_id(c.first);
        QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
        c.second->write(f_message, f_message.size());
    }
}

void hackserver::read() {
    QTcpSocket* client = (QTcpSocket*)sender();
    QByteArray buffer;
    buffer.resize(client->bytesAvailable());
    client->read(buffer.data(), buffer.size());
    PackageList msg;
    if (!msg.ParseFromArray(buffer, buffer.size())) {
      qDebug() << "Failed to parse message.";
    }
    for (const Package& p : msg.pack()) {
        std::string text = std::to_string(p.sender_id());
        text.append(" : ");
        text.append(p.text_msg().msg_text());
        ui->plainTextEdit->appendPlainText(QString::fromStdString(text));
        if (p.host_id() != -1) {
            if (!p.text_msg().is_feature()) {
                QVector<QString> values;
                values.append(QString::number(p.sender_id()));
                values.append(QString::number(p.host_id()));
                values.append(QString::fromStdString(p.text_msg().msg_text()));
                values.append(QString::number(TimeUtil::TimestampToSeconds(p.text_msg().date())));
                ObjectDAO<MessageBuilder, MessageHandler> obj_dao;
                obj_dao.insert(values);
            }
            auto need_to = clients_map.find(p.host_id());
            need_to->second->write(buffer);
        } else {
            if (p.status_msg().status() == StatusMsg::CONNECTED) {
                if (auth(p, client)) {
                    client_number++;
                }
            }
            if (p.status_msg().status() == StatusMsg::NEW_USER) {
                if (register_user(p, client)) {
                    if (auth(p, client)) {
                        client_number++;
                    }
                }
            }
            if (p.status_msg().status() == StatusMsg::SEARCH) {
                ObjectDAO<UserBuilder, UserHandler> search_obj;
                QMap<QString, QString> friends = search_obj.friend_search(QString::fromStdString(p.status_msg().user_login()));
                if (friends.empty()) {
                    continue; // or send "not found"
                }
                PackageList ans;
                Package* empty_pckg = ans.add_pack();
                empty_pckg->set_sender_id(-1);
                empty_pckg->set_host_id(p.sender_id());
                StatusMsg* e_status_msg = new StatusMsg;
                e_status_msg->set_status(StatusMsg::SEARCH);
                e_status_msg->set_user_id(0);
                empty_pckg->set_allocated_status_msg(e_status_msg);
                for (auto& it : friends.toStdMap()) {
                    Package* pckg = ans.add_pack();
                    pckg->set_sender_id(-1);
                    pckg->set_host_id(p.sender_id());
                    StatusMsg* status_msg = new StatusMsg;
                    status_msg->set_status(StatusMsg::SEARCH);
                    status_msg->set_user_id(it.first.toInt());
                    status_msg->set_user_login(it.second.toStdString());
                    pckg->set_allocated_status_msg(status_msg);
                }
                QByteArray f_message(ans.SerializeAsString().c_str(), ans.ByteSize());
                client->write(f_message);
            }
        }
    }
}

bool hackserver::register_user(const Package& msg, QTcpSocket* user) {
    QString login = QString::fromStdString(msg.status_msg().user_login());
    QByteArray password(msg.status_msg().user_pass().c_str(), msg.status_msg().user_pass().length());
    ObjectDAO<UserBuilder, UserHandler> obj_dao;
    QVector<QString> values;
    values.append(login);
    values.append(password);
    obj_dao.insert(values);
}

bool hackserver::auth(const Package& msg, QTcpSocket* user) {
    QString login = QString::fromStdString(msg.status_msg().user_login());
    QByteArray password(msg.status_msg().user_pass().c_str(), msg.status_msg().user_pass().length());

    ObjectDAO<UserBuilder, UserHandler> obj_dao;
    QMap<QString, QString> values = obj_dao.get_by_log(login);
    PackageList online_list;
    Package* answer = online_list.add_pack();
    answer->set_sender_id(-1);
    answer->set_host_id(0);
    StatusMsg* status = new StatusMsg;
    answer->set_allocated_status_msg(status);

    if (values.isEmpty()) {
        status->set_status(StatusMsg::LOGIN_NOT_FOUND);
        QByteArray f_message(online_list.SerializeAsString().c_str(), online_list.ByteSize());
        user->write(f_message);
        return false;
    }

    if (QString::compare(values["password"], password)) {
        status->set_status(StatusMsg::WRONG_PASS);
        QByteArray f_message(online_list.SerializeAsString().c_str(), online_list.ByteSize());
        user->write(f_message);
        return false;
    }

    int user_id = values["id"].toInt();
    status->set_status(StatusMsg::AUTH_SUCCESS);
    status->set_user_id(user_id);
    status->set_user_login(login.toStdString());
    ObjectDAO<UserBuilder, UserHandler> friend_list;
    QVector<QString> friends = friend_list.get_friends(user_id);
    for (int i = 0; i < friends.size(); i++) {
        int friend_id = friends.at(i).toInt();
        if (clients_map.find(friend_id) == clients_map.end()) {
            continue;
        }
        Package* user_online = online_list.add_pack();
        user_online->set_sender_id(-1);
        user_online->set_host_id(user_id);
        StatusMsg* online_user = new StatusMsg;
        online_user->set_status(StatusMsg::CONNECTED);
        ObjectDAO<UserBuilder, UserHandler> obj_dao;
        QMap<QString, QString> values = obj_dao.get_by_id(friend_id);
        online_user->set_user_id(friend_id);
        online_user->set_user_login(values["login"].toStdString());
        user_online->set_allocated_status_msg(online_user);
    }

    QByteArray f_message(online_list.SerializeAsString().c_str(), online_list.ByteSize());
    user->write(f_message);
    send_everyone_new(user_id, login.toStdString());
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
    if (num == -1) {
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
    msg.set_allocated_text_msg(text);
    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
    for (std::pair<int const, QTcpSocket*>& c : clients_map) {
        c.second->write(f_message, f_message.size());
    }
    ui->lineEdit->clear();
}
