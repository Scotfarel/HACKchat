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
    prepare_status_msg(msg, StatusMsg::CONNECTED, id, login);
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
    prepare_status_msg(msg, StatusMsg::DISCONNECTED, id);
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
                register_user(p, client);
            }
            if (p.status_msg().status() == StatusMsg::SEARCH) {
                ObjectDAO<UserBuilder, UserHandler> search_obj;
                QMap<QString, QString> friends = search_obj.friend_search(QString::fromStdString(p.status_msg().user_login()));
                PackageList ans;
                Package* empty_pckg = ans.add_pack();
                empty_pckg->set_host_id(p.sender_id());
                if (friends.isEmpty()) {
                    prepare_status_msg(empty_pckg, StatusMsg::NOT_FOUND);
                } else {
                    prepare_status_msg(empty_pckg, StatusMsg::SEARCH);
                    QVector<QString> already_friends = search_obj.get_friends(p.sender_id());
                    for (auto& it : friends.toStdMap()) {
                        if (p.sender_id() == it.first.toInt() || already_friends.contains(it.first)) {
                            continue;
                        }
                        Package* pckg = ans.add_pack();
                        pckg->set_host_id(p.sender_id());
                        prepare_status_msg(pckg, StatusMsg::SEARCH, it.first.toInt(), it.second.toStdString());
                    }
                }
                QByteArray f_message(ans.SerializeAsString().c_str(), ans.ByteSize());
                client->write(f_message);
            }
            if (p.status_msg().status() == StatusMsg::ADD) {
                ObjectDAO<UserBuilder, UserHandler> user;
                QMap<QString, QString> data = user.get_by_log(QString::fromStdString(p.status_msg().user_login()));
                int friend_id = data["id"].toInt();
                user.add_friend(p.sender_id(), friend_id);
                if (clients_map[friend_id]) {
                    PackageList msg;
                    Package* pck = msg.add_pack();
                    pck->set_host_id(p.sender_id());
                    prepare_status_msg(pck, StatusMsg::CONNECTED, friend_id, (p.status_msg().user_login()));
                    QByteArray f_message(msg.SerializeAsString().c_str(), msg.ByteSize());
                    client->write(f_message);

                    pck->set_host_id(friend_id);
                    data = user.get_by_id(p.sender_id());
                    prepare_status_msg(pck, StatusMsg::CONNECTED, p.sender_id(), (data["login"].toStdString()));
                    QByteArray f_message_second(msg.SerializeAsString().c_str(), msg.ByteSize());
                    clients_map.at(friend_id)->write(f_message_second);
                }
            }
        }
    }
}

void hackserver::register_user(const Package& msg, QTcpSocket* user) {
    QString login = QString::fromStdString(msg.status_msg().user_login());
    QByteArray password(msg.status_msg().user_pass().c_str(), msg.status_msg().user_pass().length());

    PackageList list;
    Package* answer = list.add_pack();
    answer->set_host_id(msg.sender_id());

    ObjectDAO<UserBuilder, UserHandler> obj_dao;
    QMap<QString, QString> users = obj_dao.get_by_log(login);
    if (!users.empty()) {
        prepare_status_msg(answer, StatusMsg::LOGIN_FOUND);
    } else {
        QVector<QString> values;
        values.append(login);
        values.append(password);
        obj_dao.insert(values);
        prepare_status_msg(answer, StatusMsg::NEW_USER);
    }
    QByteArray f_message(list.SerializeAsString().c_str(), list.ByteSize());
    user->write(f_message);
    return;
}

bool hackserver::auth(const Package& msg, QTcpSocket* user) {
    QString login = QString::fromStdString(msg.status_msg().user_login());
    QByteArray password(msg.status_msg().user_pass().c_str(), msg.status_msg().user_pass().length());

    ObjectDAO<UserBuilder, UserHandler> obj_dao;
    QMap<QString, QString> values = obj_dao.get_by_log(login);
    PackageList online_list;
    Package* answer = online_list.add_pack();
    answer->set_host_id(0);

    if (values.isEmpty() || QString::compare(values["password"], password)) {
        prepare_status_msg(answer, StatusMsg::AUTH_UNSUCCESS);
        QByteArray f_message(online_list.SerializeAsString().c_str(), online_list.ByteSize());
        user->write(f_message);
        return false;
    }

    int user_id = values["id"].toInt();
    prepare_status_msg(answer, StatusMsg::AUTH_SUCCESS, user_id, login.toStdString());
    ObjectDAO<UserBuilder, UserHandler> friend_list;
    QVector<QString> friends = friend_list.get_friends(user_id);
    for (int i = 0; i < friends.size(); i++) {
        int friend_id = friends.at(i).toInt();
        if (clients_map.find(friend_id) == clients_map.end()) {
            continue;
        }
        Package* user_online = online_list.add_pack();
        user_online->set_host_id(user_id);
        ObjectDAO<UserBuilder, UserHandler> obj_dao;
        QMap<QString, QString> values = obj_dao.get_by_id(friend_id);
        prepare_status_msg(user_online, StatusMsg::CONNECTED, friend_id, values["login"].toStdString());
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

void hackserver::prepare_status_msg(Package* package, StatusMsg::Status status, int user_id, std::string user_login) {
    package->set_sender_id(-1);
    StatusMsg* status_msg = new StatusMsg;
    status_msg->set_status(status);
    status_msg->set_user_id(user_id);
    status_msg->set_user_login(user_login);
    package->set_allocated_status_msg(status_msg);
}
