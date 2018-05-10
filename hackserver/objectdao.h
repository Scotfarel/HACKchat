#ifndef OBJECTDAO_H
#define OBJECTDAO_H

#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMap>

template <typename Builder, typename Handler>
class ObjectDAO {
private:
    Builder* builder;
    Handler* handler;
    QSqlQuery query;
    QMap<QString, QString> values;
    QVector<QString> vector_values;
public:
    void execute() {
        if (!query.exec()) {
            qDebug() << query.lastError().text();
            return;
        }
    }

    bool move() {
        if (!query.next()) {
            qDebug() << "Answer is empty.";
            return false;
        }
        return true;
    }

    QMap<QString, QString> get_by_id(int id) {
        if (query.prepare(builder->get(id))) {
            execute();
            if (move()) {
                handler->fill_by_id(query, &values);
            }
        }
        return values;
    }

    QVector<QString> get_friends(int id) {
        if (query.prepare(builder->get_first_friends(id))) {
            execute();
            handler->fill_first_friends(query, &vector_values);
        }
        if (query.prepare(builder->get_second_friends(id))) {
            execute();
            handler->fill_second_friends(query, &vector_values);
        }
        return vector_values;
    }

    QMap<QString, QString> get_by_log(QString login) {
        if (query.prepare(builder->by_log(login))) {
            execute();
            if (move()) {
                handler->fill_by_login(query, &values);
            }
        }
        return values;
    }

    void insert(QVector<QString> cols) {
        if (query.prepare(builder->insert(cols))) {
            execute();
        }
    }
};

#endif // OBJECTDAO_H
