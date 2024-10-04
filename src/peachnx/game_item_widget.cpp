#include <peachnx/game_item_widget.h>

namespace Peachnx {
    GameItemWidget::GameItemWidget(const std::vector<u8>& logo, const QString& label, QWidget* parent) :
        QWidget(parent) {
        byteArray = std::make_unique<QByteArray>(reinterpret_cast<const char*>(logo.data()), logo.size());
        backing = std::make_unique<QBuffer>(byteArray.get());

        backing->open(QIODeviceBase::ReadOnly);

        UpdateMovie();
        SetTitle(label);

    }
    void GameItemWidget::UpdateMovie() {

        gifHd = std::make_unique<QMovie>(backing.get(), QByteArray());
        if (gifHd->isValid()) {
            gifHd->start();
        }
        movie = std::make_unique<QLabel>(this);

        movie->setMovie(gifHd.get());
    }
    void GameItemWidget::SetTitle(const QString& label) {
        title = std::make_unique<QLabel>(this);
        title->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
        title->setFixedSize(100, 100);
        title->setText(label);
    }
}
