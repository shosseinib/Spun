#include "kiopicker.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QList>
#include <QStandardPaths>
#include <QUrl>
#include <QVBoxLayout>

#ifdef SPUN_WITH_KIO
#include <KFile>
#include <KFileFilter>
#include <KFileWidget>
#endif

KioPicker::KioPicker(QObject *parent) : QObject(parent) {
#ifdef SPUN_WITH_KIO
    m_available = true;
#else
    m_available = false;
#endif
}

void KioPicker::open() {
#ifndef SPUN_WITH_KIO
    emit filesSelected({});
    return;
#endif
    QDialog dialog;
    dialog.setWindowTitle("Add music");
    QVBoxLayout layout(&dialog);
    const QString music = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    auto *picker = new KFileWidget(music.isEmpty() ? QUrl() : QUrl::fromLocalFile(music), &dialog);
    picker->setMode(KFile::File | KFile::ExistingOnly);
    picker->setOperationMode(KFileWidget::Opening);
    picker->setFilters({
        KFileFilter("Music", {"*.mp3", "*.flac", "*.wav", "*.ogg", "*.opus", "*.m4a", "*.aac", "*.aiff", "*.aif", "*.wma"},
                    {"audio/mpeg", "audio/flac", "audio/x-wav", "audio/ogg", "audio/opus", "audio/mp4", "audio/aac", "audio/x-aiff", "audio/x-ms-wma"}),
        KFileFilter("All files", {"*"}, {})
    });
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout.addWidget(picker);
    layout.addWidget(buttons);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    QVariantList urls;
    if (dialog.exec() == QDialog::Accepted) {
        for (const auto &url : picker->selectedUrls()) urls.append(url);
    }
    emit filesSelected(urls);
}
