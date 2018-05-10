#ifndef USERHANDLER_H
#define USERHANDLER_H

#include <QString>
#include <QSqlQuery>

class UserHandler {
public:
    void fill_by_id(QSqlQuery query, QMap<QString, QString>* values) {
        values->insert("login", query.value("login").toString());
        values->insert("password", query.value("p_hash").toString());
    }

    void fill_by_login(QSqlQuery query, QMap<QString, QString>* values) {
        values->insert("id", query.value("rowid").toString());
        values->insert("password", query.value("p_hash").toString());
    }

    void fill_first_friends(QSqlQuery query, QVector<QString>* values) {
        while (query.next()) {
            int friend_id = query.value("user_2").toInt();
            values->append(QString::number(friend_id));
        }
    }

    void fill_second_friends(QSqlQuery query, QVector<QString>* values) {
        while (query.next()) {
            int friend_id = query.value("user_1").toInt();
            values->append(QString::number(friend_id));
        }
    }

    void friend_found(QSqlQuery query, QMap<QString, QString>* values) {
        while (query.next()) {
            values->insert(query.value("rowid").toString(), query.value("login").toString());
        }
    }

};

#endif // USERHANDLER_H
