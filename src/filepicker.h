#pragma once

#include <QObject>
#include <QVariant>

class FilePicker : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool available READ available CONSTANT)
public:
    explicit FilePicker(QObject *parent = nullptr);
    bool available() const { return m_available; }
    Q_INVOKABLE void open();
signals:
    void filesSelected(QVariantList urls);
private:
    bool m_available;
    static bool isGnomeDesktop();
    static bool kioAvailable();
};
