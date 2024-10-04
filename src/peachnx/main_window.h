#pragma once

#include <QtWidgets/QMainWindow>

#include <core/application.h>
#include <peachnx/game_item_widget.h>
namespace Peachnx {
    class MainWindow final : public QMainWindow {
    public:
        explicit MainWindow(QWidget* parent = nullptr);
    private:
        void AddGameList();

        void closeEvent(QCloseEvent* event) override;
        std::unique_ptr<core::Application> peachnx;

        std::unique_ptr<QWidget> middle;
        std::unique_ptr<QGridLayout> gameGrid;

        std::list<std::unique_ptr<GameItemWidget>> gridItems;
    };
}