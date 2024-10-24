#include <QApplication>
#include <QFileDialog>
#include <QLabel>
#include <QScreen>
#include <QMenuBar>
#include <QMessageBox>
#include "emulator.h"
#include "settings.h"
#include "MainWindow.h"
#include "SH4RegistersWidget.h"
#include "ui/gui.h"

/**
 * Code formatting inconsistent with the wider Flycast codebase but changing
 * how I type is hard, sorry :P
 */

namespace qdbg {

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	lastState(false)
{
	restoreSettings();

	EventManager::listen(Event::Start, emuEventCallback, this);
	EventManager::listen(Event::Pause, emuEventCallback, this);
	EventManager::listen(Event::Resume, emuEventCallback, this);
	EventManager::listen(Event::Terminate, emuEventCallback, this);
	EventManager::listen(Event::VBlank, emuEventCallback, this);

	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(QIcon::fromTheme("document-open"), tr("&Open game"), QKeySequence::Open, this, &MainWindow::openGame);

	QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(tr("About Qt"), this, [this]() { QMessageBox::aboutQt(this); });

	actionSuspend = new QAction();
	actionSuspend->setCheckable(emu.loaded());

	actionStep = new QAction(tr("Step"));
	actionStepOver = new QAction(tr("Step over"));
	actionStepOut = new QAction(tr("Step out"));

	// Setting objectName required for Qt to save state for toolbars and dockwidgets
	toolBar = new QToolBar(tr("State"));
	toolBar->setObjectName("stateToolbar");
	toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	toolBar->addAction(actionSuspend);
	toolBar->addAction(actionStep);
	toolBar->addAction(actionStepOver);
	toolBar->addAction(actionStepOut);
	
	sh4Registers = new SH4RegistersWidget();

	dockSH4Registers = new QDockWidget(tr("SH-4 Registers"));
	dockSH4Registers->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	dockSH4Registers->setWidget(sh4Registers);
	dockSH4Registers->setObjectName("dockSH4Registers");

	connect(actionSuspend, &QAction::triggered, this, &MainWindow::setState);

	setCentralWidget(new QWidget());

	addToolBar(toolBar);
	addDockWidget(Qt::RightDockWidgetArea, dockSH4Registers);

	stateChanged(emu.running());
};

MainWindow::~MainWindow() {
	EventManager::unlisten(Event::Start, emuEventCallback);
	EventManager::unlisten(Event::Pause, emuEventCallback);
	EventManager::unlisten(Event::Resume, emuEventCallback);
	EventManager::unlisten(Event::Terminate, emuEventCallback);
	EventManager::unlisten(Event::VBlank, emuEventCallback);
}

void MainWindow::openGame() {
	const QString path = QFileDialog::getOpenFileName(
		this,
		tr("Open game"),
		QString(),
		tr("All files (*.*)")
	);

	if (!path.isEmpty()) {
		gui_start_game(path.toStdString());
	}
}

void MainWindow::closeEvent(QCloseEvent* event) {
	saveSettings();
}

void MainWindow::emuEventCallback(Event event, void* arg) {
	MainWindow* that = static_cast<MainWindow*>(arg);
	that->actionSuspend->setCheckable(event != Event::Terminate);
	that->sh4Registers->fetch();

	bool running = emu.running();
	if (running != that->lastState) {
		that->stateChanged(running);
		that->lastState = running;
	}
}

void MainWindow::setState(bool run) {
	if (!emu.loaded())
		return;
	run ? emu.start() : emu.stop();
}

void MainWindow::stateChanged(bool running) {
	if (running) {
		actionSuspend->setIcon(QIcon::fromTheme("media-playback-pause"));
		actionSuspend->setText(tr("Suspend"));
	} else {
		actionSuspend->setIcon(QIcon::fromTheme("media-playback-start"));
		actionSuspend->setText(tr("Resume"));
	}

	actionSuspend->setChecked(running);
	actionStep->setDisabled(running);
	actionStepOver->setDisabled(running);
	actionStepOut->setDisabled(running);
}

void MainWindow::restoreSettings() {
	if (!restoreGeometry(settings->value("MainWindow/Geometry").toByteArray())) {
		resize(1024, 768);
		move(QApplication::primaryScreen()->availableGeometry().center() - frameGeometry().center());
	}

	restoreState(settings->value("MainWindow/WindowState").toByteArray());
}

void MainWindow::saveSettings() {
	settings->setValue("MainWindow/Geometry", saveGeometry());
	settings->setValue("MainWindow/WindowState", saveState());
}

} // namespace qdbg
