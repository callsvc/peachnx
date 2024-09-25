#pragma once
#include <QLabel>
#include <QWidget>
#include <QBuffer>
#include <QMovie>
#include <QGridLayout>
#include <QByteArray>

#include <core/application.h>
namespace peachnx {
    class GameItemWidget final : public QWidget {
        Q_OBJECT
    public:
        explicit GameItemWidget(const std::vector<u8>& logo, const QString& label, QWidget* parent = nullptr);

        void SetTitle(const QString& label);
    private:
        void UpdateMovie();

        std::unique_ptr<QByteArray> byteArray;
        std::unique_ptr<QBuffer> backing;

        std::unique_ptr<QMovie> gifHd;
        std::unique_ptr<QLabel> movie;
        std::unique_ptr<QLabel> title;
    };
}