#include "filepicker.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QList>
#include <QStandardPaths>
#include <QUrl>
#include <QVBoxLayout>

#ifdef SPUN_WITH_KIO
#include <KFile>
#include <KFileFilter>
#include <KFileWidget>
#endif

FilePicker::FilePicker(QObject *parent) : QObject(parent) {
    m_available = true;
}

bool FilePicker::isGnomeDesktop() {
    const QString desktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
    if (!desktop.isEmpty() && desktop.contains("GNOME", Qt::CaseInsensitive))
        return true;
    const QString session = qEnvironmentVariable("DESKTOP_SESSION");
    if (!session.isEmpty() && session.contains("gnome", Qt::CaseInsensitive))
        return true;
    return false;
}

bool FilePicker::kioAvailable() {
#ifdef SPUN_WITH_KIO
    return true;
#else
    return false;
#endif
}

void FilePicker::open() {
    QVariantList urls;

    // On GNOME (and other non-KDE desktops), use Qt's native file dialog,
    // which resolves to the platform's GTK file chooser via Qt's platform
    // theme integration. This keeps Spun consistent with GNOME, tiling WMs,
    // and other non-Qt desktop environments without pulling in KDE libraries.
    if (isGnomeDesktop() || !kioAvailable()) {
        QFileDialog dialog;
        dialog.setWindowTitle("Add music");
        dialog.setFileMode(QFileDialog::ExistingFiles);
        dialog.setNameFilters({
            QStringLiteral("Music (*.mp3 *.flac *.wav *.ogg *.opus *.m4a *.aac *.aiff *.aif *.wma)"),
            QStringLiteral("All files (*)")
        });
        const QString music = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
        if (!music.isEmpty())
            dialog.setDirectory(music);
        if (dialog.exec() == QDialog::Accepted) {
            for (const QString &path : dialog.selectedFiles())
                urls.append(QUrl::fromLocalFile(path));
        }
        emit filesSelected(urls);
        return;
    }

    // KDE / Plasma: prefer KIO's native file picker.
#ifdef SPUN_WITH_KIO
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
    if (dialog.exec() == QDialog::Accepted) {
        for (const auto &url : picker->selectedUrls()) urls.append(url);
    }
#endif
    emit filesSelected(urls);
}
