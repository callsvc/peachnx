#include <QApplication>

#include <peachnx/main_window.h>
namespace peachnx {
    MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent),
        process(std::make_unique<core::Process>()) {

        resize(640, 480);
        show();
        setWindowTitle(QApplication::translate("toplevel", "Peachnx"));
    }
}