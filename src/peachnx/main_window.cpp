#include <print>
#include <QApplication>

#include <peachnx/main_window.h>
namespace peachnx {
    MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent),
        process(std::make_unique<core::Process>()) {

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
