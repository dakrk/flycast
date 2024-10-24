#pragma once
#include <QCheckBox>
#include <QLabel>
#include <QTabWidget>
#include <QVector>
#include <QWidget>
#include "hw/sh4/sh4_if.h"

namespace qdbg {

class SH4RegistersWidget : public QTabWidget {
	Q_OBJECT
	Sh4Context ctx;
public:
	explicit SH4RegistersWidget(QWidget* parent = nullptr);
	void fetch();
};

// Qt's meta object compiler doesn't support nested classes unfortunately
class SH4RegistersAbstractTab : public QWidget {
	Q_OBJECT
public:
	explicit SH4RegistersAbstractTab(QWidget* parent = nullptr);
	virtual void fetch() = 0;
};

/**
 * Heap allocations for all of these small objects irks me a bit, and I could
 * get rid of that, but heap use is the way Qt wants you to do it
 */
class SH4RegistersGeneralTab : public SH4RegistersAbstractTab {
	Q_OBJECT
	bool altBank;
	QLabel* lblPC;
	QLabel* lblPR;
	QVector<QLabel*> lblR;
	QVector<QLabel*> lblFR;
	QLabel* lblSR;
	QLabel* lblFPSCR;
public:
	explicit SH4RegistersGeneralTab(QWidget* parent = nullptr);
	void fetch() override;
};

class SH4RegistersControlTab : public SH4RegistersAbstractTab {
	Q_OBJECT
	QLabel *lblIMASK, *lblSR;
	QCheckBox *cbT, *cbS;
	QCheckBox *cbQ, *cbM;
	QCheckBox *cbFD, *cbBL;
	QCheckBox *cbRB, *cbMD;

	QLabel *lblRM, *lblFPSCR;
	QCheckBox *cbDN, *cbPR;
	QCheckBox *cbSZ, *cbFR;
	QCheckBox *cbFInexact, *cbEInexact, *cbCInexact;
	QCheckBox *cbFUnderflow, *cbEUnderflow, *cbCUnderflow;
	QCheckBox *cbFOverflow, *cbEOverflow, *cbCOverflow;
	QCheckBox *cbFDivByZero, *cbEDivByZero, *cbCDivByZero;
	QCheckBox *cbFInvalid, *cbEInvalid, *cbCInvalid;
	QCheckBox *cbCFPUErr;
public:
	explicit SH4RegistersControlTab(QWidget* parent = nullptr);
	void fetch() override;
};

} // namespace qdbg
