#include <QtWidgets/QApplication>

#include <peachnx/main_window.h>
#include <settings/configuration.h>
int main(int argc, char** argv) {
    namespace pnx_set = peachnx::settings;
    pnx_set::options = std::make_unique<pnx_set::Options>();

    QApplication app(argc, argv);
    peachnx::MainWindow main{};

    return QApplication::exec();
}
