#include <QtWidgets/QApplication>

#include <peachnx/main_window.h>
int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QWidget window;
    peachnx::MainWindow main(&window);

    return QApplication::exec();
}
