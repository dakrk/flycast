#include <QMessageBox>
#include "debugger.h"
#include "settings.h"
#include "MainWindow.h"
#include "stdclass.h"

// Have to have all other Flycast headers last because of stupid macros conflicting with Qt

namespace qdbg {

QApplication* app = nullptr;
QSettings* settings = nullptr;
MainWindow* mainWindow = nullptr;

void init(int& argc, char** argv) {
	app = new QApplication(argc, argv);
	app->setApplicationName("flycast-debugger");
	app->setApplicationDisplayName(app->tr("Flycast Debugger"));

	settings = new QSettings(QString::fromStdString(get_writable_config_path("qdbg.ini")), QSettings::IniFormat, app);

	if (settings->status() != QSettings::NoError) {
		QMessageBox::warning(
			nullptr,
			app->tr("Failed to open config"),
			app->tr("Failed to open config file at %1").arg(settings->fileName())
		);
	}

#ifdef _WIN32
	// Allows for dark theme
	app.setStyle(QStyleFactory::create("Fusion"));
#endif

	mainWindow = new MainWindow();
}

void show() {
	mainWindow->show();
}

bool mainIsVisible() {
	return mainWindow->isVisible();
}

void uninit() {
	delete mainWindow;
	app->exit();
}

} // namespace qdbg
