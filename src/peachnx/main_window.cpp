#include <print>
#include <QApplication>

#include <peachnx/main_window.h>
namespace peachnx {
    MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent),
        peachnx(std::make_unique<core::Application>()) {

        resize(640, 480);
        AddGameList();

        show();
        const auto title{GetTitleWindow()};
        setWindowTitle(QApplication::translate("toplevel", title.c_str()));
    }
    void MainWindow::AddGameList() {
        gameGrid = std::make_unique<QGridLayout>();

        auto games{peachnx->GetGameList()};
        for (const auto& loader : games) {
            const auto logo{loader->GetLogo()};
            auto label{loader->GetGameTitle()};

            auto gameItem{std::make_unique<GameItemWidget>(logo, QString::fromStdString(label), this)};
            gameGrid->addWidget(gameItem.get(), 0, 0);
            gridItems.emplace_back(std::move(gameItem));
        }

        middle = std::make_unique<QWidget>(this);
        middle->setLayout(gameGrid.get());
        setCentralWidget(middle.get());
    }
    void MainWindow::closeEvent(QCloseEvent* event) {
        std::print("The cleanup code should be here\n");
        QMainWindow::closeEvent(event);
        QApplication::quit();
    }
}
