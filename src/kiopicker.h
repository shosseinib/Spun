#pragma once

#include <QObject>
#include <QVariant>

class KioPicker : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool available READ available CONSTANT)
public:
    explicit KioPicker(QObject *parent = nullptr);
    bool available() const { return m_available; }
    Q_INVOKABLE void open();
signals:
    void filesSelected(QVariantList urls);
private:
    bool m_available;
};
