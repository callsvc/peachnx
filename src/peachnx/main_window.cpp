#include <print>
#include <QApplication>

#include <peachnx/main_window.h>
namespace peachnx {
    MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent),
        peachnx(std::make_unique<core::Application>()) {

        resize(640, 480);
        show();
        const auto title{GetTitleWindow()};
        setWindowTitle(QApplication::translate("toplevel", title.c_str()));
    }
    void MainWindow::closeEvent(QCloseEvent* event) {
        std::print("The cleanup code should be here\n");
        QMainWindow::closeEvent(event);
        QApplication::quit();
    }
}
