#ifndef MESSAGEBUILDER_H
#define MESSAGEBUILDER_H

#include <QString>

class MessageBuilder {
public:
    MessageBuilder() {}

    QString insert(QVector<QString> values) {
        QString query = QString("INSERT INTO messages VALUES (%1, %2, '%3', %4)").arg(values[0], values[1], values[2], values[3]);
        return query;
    }
};

#endif // MESSAGEBUILDER_H
