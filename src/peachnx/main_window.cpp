#include <print>
#include <QApplication>

#include <peachnx/main_window.h>
namespace peachnx {
    MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent),
        process(std::make_unique<core::Process>()) {

        connect(QApplication::instance(), &QApplication::aboutToQuit, this, [this] {
            std::print("The cleanup code should be here\n");
        });

        resize(640, 480);
        show();
        setWindowTitle(QApplication::translate("toplevel", "Peachnx"));
    }
}