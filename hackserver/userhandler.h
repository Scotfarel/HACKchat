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

};

#endif // USERHANDLER_H
