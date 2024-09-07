#pragma once

#include <QtWidgets/QMainWindow>

#include <core/process.h>
namespace peachnx {
    class MainWindow final : public QMainWindow {
    public:
        explicit MainWindow(QWidget* parent);
    private:
        std::unique_ptr<core::Process> process;
    };
}