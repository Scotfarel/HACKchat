#include <QDebug>
#include "objectdao.h"

void ObjectDAO::execute() {
    if (!query.exec()) {
        qDebug() << query.lastError().text();
        return;
    }
}

bool ObjectDAO::move() {
    if (!query.next()) {
        qDebug() << "Answer is empty.";
        return false;
    }
    return true;
}

QMap<QString, QString> ObjectDAO::get_user_by_id(int id) {
    query.prepare(QString("SELECT %1, %2 FROM %3 WHERE %4 = :id").arg(login_col, pass_col, user_table, user_id_col));
    query.bindValue(":id", id);
    execute();
    if (move()) {
        values.insert("login", query.value(login_col).toString());
        values.insert("password", query.value(pass_col).toString());
    }
    return values;
}

QMap<QString, QString> ObjectDAO::get_user_by_login(QString login) {
    query.prepare(QString("SELECT %1, %2 FROM %3 WHERE %4 = :login").arg(user_id_col, pass_col, user_table, login_col));
    query.bindValue(":login", login);
    execute();
    if (move()) {
        values.insert("id", query.value(user_id_col).toString());
        values.insert("password", query.value(pass_col).toString());
    }
    return values;
}

void ObjectDAO::add_user(QString login, QString pass) {
    query.prepare(QString("INSERT INTO %1 VALUES (:login, :pass)").arg(user_table));
    query.bindValue(":login", login);
    query.bindValue(":pass", pass);
    query.exec();
}

QMap<QString, QString> ObjectDAO::friend_search(QString searchStr) {
    query.prepare(QString("SELECT %1, %2 FROM %3 WHERE %4 LIKE ? ESCAPE '%'").arg(user_id_col, login_col, user_table, login_col));
    query.addBindValue('%' + searchStr + '%');
    execute();
    while (query.next()) {
        values.insert(query.value("rowid").toString(), query.value("login").toString());
    }
    qDebug() << query.lastQuery();
    return values;
}

QVector<QString> ObjectDAO::get_friends(int id) {
    query.prepare(QString("SELECT user_1, user_2 FROM %1 WHERE user_1 = :id").arg(friends_table));
    query.bindValue(":id", id);
    execute();
    while (query.next()) {
        int friend_id = query.value("user_2").toInt();
        vector_values.append(QString::number(friend_id));
    }
    // OR statement didn't work in QSqlQuery
    query.prepare(QString("SELECT user_1, user_2 FROM %1 WHERE user_2 = :id").arg(friends_table));
    query.bindValue(":id", id);
    execute();
    while (query.next()) {
        int friend_id = query.value("user_1").toInt();
        vector_values.append(QString::number(friend_id));
    }
    return vector_values;
}

void ObjectDAO::add_friend(int id1, int id2) {
    query.prepare(QString("INSERT INTO %1 VALUES (:user_1, :user_2)").arg(friends_table));
    query.bindValue(":user_1", id1);
    query.bindValue(":user_2", id2);
    execute();
}

void ObjectDAO::add_message(int sender_id, int host_id, QString text, QString time) {
    query.prepare(QString("INSERT INTO %1 VALUES (:sender, :hoster, :text, :time)").arg(message_table));
    query.bindValue(":sender", sender_id);
    query.bindValue(":hoster", host_id);
    query.bindValue(":text", text);
    query.bindValue(":time", time);
    execute();
}
