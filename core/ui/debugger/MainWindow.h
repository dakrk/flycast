#pragma once
#include <QDockWidget>
#include <QMainWindow>
#include <QToolBar>
#include "emulator.h"
#include "debug/debug_agent.h"

namespace qdbg {

class SH4RegistersWidget;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

public slots:
	void openGame();

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	static void emuEventCallback(Event event, void* arg);

	void setState(bool running);
	void stateChanged(bool running);

	void restoreSettings();
	void saveSettings();

	bool lastState;

	QAction* actionSuspend;
	QAction* actionStep;
	QAction* actionStepOver;
	QAction* actionStepOut;
	QToolBar* toolBar;

	SH4RegistersWidget* sh4Registers;
	QDockWidget* dockSH4Registers;
};

} // namespace qdbg
