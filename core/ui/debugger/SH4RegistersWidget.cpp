#include <QGridLayout>
#include "SH4RegistersWidget.h"
#include "utils.h"
#include "hw/sh4/sh4_if.h"

namespace qdbg {

SH4RegistersWidget::SH4RegistersWidget(QWidget* parent) {
	QFont font("monospace");
	QFont headingFont = font;
	headingFont.setBold(true);

	setFont(font);

	QLabel* lblPCName = new QLabel("pc");
	lblPCName->setFont(headingFont);
	lblPC = new QLabel(formatHex32(0));

	QLabel* lblPRName = new QLabel("pr");
	lblPRName->setFont(headingFont);
	lblPR = new QLabel(formatHex32(0));

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(lblPCName, 0, 0);
	layout->addWidget(lblPC, 0, 1);
	layout->addWidget(lblPRName, 0, 2);
	layout->addWidget(lblPR, 0, 3);
	layout->setColumnStretch(3, 1);

	lblR.resize(16);
	lblFR.resize(16);

	for (int i = 0; i < 16; i++) {
		QLabel* lblRName = new QLabel(QStringLiteral("r%1").arg(i));
		lblRName->setFont(headingFont);
		lblR[i] = new QLabel(formatHex32(0));

		QLabel* lblFRName = new QLabel(QStringLiteral("fr%1").arg(i));
		lblFRName->setFont(headingFont);
		lblFR[i] = new QLabel(formatFloat32(0));

		auto row = layout->rowCount();

		layout->addWidget(lblRName, row, 0);
		layout->addWidget(lblR[i], row, 1);
		layout->addWidget(lblFRName, row, 2);
		layout->addWidget(lblFR[i], row, 3);
	}

	setLayout(layout);
}

void SH4RegistersWidget::fetch() {
	if (!p_sh4rcb)
		return;

	lblPC->setText(formatHex32(Sh4cntx.pc));
	lblPR->setText(formatHex32(Sh4cntx.pr));

	for (int i = 0; i < lblR.size(); i++) {
		lblR[i]->setText(formatHex32(Sh4cntx.r[i]));
	}

	for (int i = 0; i < lblFR.size(); i++) {
		lblFR[i]->setText(formatFloat32(Sh4cntx.xffr[16 + i]));
	}
}

} // namespace qdbg
