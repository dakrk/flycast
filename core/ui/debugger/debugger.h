#pragma once
#include <QApplication>

namespace qdbg {

class MainWindow;

extern QApplication* app;
extern MainWindow* mainWindow;

void init(int& argc, char** argv);
void show();
bool mainIsVisible();
void uninit();

} // namespace qdbg
