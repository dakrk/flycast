#pragma once
#include <QMainWindow>
#include <QToolBar>
#include "emulator.h"
#include "debug/debug_agent.h"

namespace qdbg {

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

	QAction* actionSuspend;

	QToolBar* toolBar;

	DebugAgent debugAgent;
};

} // namespace qdbg
