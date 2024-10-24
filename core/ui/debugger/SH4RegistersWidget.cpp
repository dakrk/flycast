#include <QGridLayout>
#include <QScrollArea>
#include <QVBoxLayout>
#include "SH4RegistersWidget.h"
#include "utils.h"
#include "hw/sh4/sh4_if.h"

#define MAKE_REGISTER_LABELS(reg, name) \
	QLabel* lbl##reg##Name = newBoldLabel(name); \
	lbl##reg = new QLabel(formatHex32(0));

#define MAKE_REGISTER_CHECKBOX(reg, name) \
	cb##reg = new QCheckBox(name);

#define ADD_REGISTER_LABELS(layout, reg, row, startCol) \
	layout->addWidget(lbl##reg##Name, row, startCol); \
	layout->addWidget(lbl##reg, row, startCol + 1);

// Not that the checkbox macros make anything shorter, but makes stuff more consistent
#define ADD_REGISTER_CHECKBOX(layout, reg, row, col) \
	layout->addWidget(cb##reg, row, col);

#define SET_REGISTER_LABEL(reg, val) \
	lbl##reg->setText(formatHex32(val));

#define SET_REGISTER_CHECKBOX(reg, val) \
	cb##reg->setChecked(val);

static QLabel* newBoldLabel(const QString& text, QWidget* parent = nullptr) {
	QLabel* label = new QLabel(text, parent);
	QFont font = label->font();
	font.setBold(true);
	label->setFont(std::move(font));
	return label;
}

namespace qdbg {

SH4RegistersWidget::SH4RegistersWidget(QWidget* parent) :
	QTabWidget(parent)
{
	addTab(new SH4RegistersGeneralTab, tr("General"));
	addTab(new SH4RegistersControlTab, tr("Control"));
	setTabPosition(QTabWidget::South);
	setMovable(true);

	connect(this, &QTabWidget::currentChanged, this, [this](int index) {
		if (index != -1 && p_sh4rcb) {
			SH4RegistersAbstractTab* tab = qobject_cast<SH4RegistersAbstractTab*>(widget(index));
			if (tab) {
				tab->fetch();
			}
		}
	});
}

/**
 * Undefined data can be shown because Flycast seemingly doesn't zero the
 * structs when it first starts
 */
void SH4RegistersWidget::fetch() {
	if (!p_sh4rcb)
		return;

	// a lot to keep copying around. needed for diffing later
	ctx = Sh4cntx;

	SH4RegistersAbstractTab* currentTab = qobject_cast<SH4RegistersAbstractTab*>(currentWidget());
	if (!currentTab)
		return;

	currentTab->fetch();
}

SH4RegistersAbstractTab::SH4RegistersAbstractTab(QWidget* parent) :
	QWidget(parent) {}

SH4RegistersGeneralTab::SH4RegistersGeneralTab(QWidget* parent) :
	SH4RegistersAbstractTab(parent),
	altBank(false)
{
	setFont(QFont("monospace"));

	MAKE_REGISTER_LABELS(PC, "pc");
	MAKE_REGISTER_LABELS(PR, "pr");

	QGridLayout* regLayout = new QGridLayout();
	ADD_REGISTER_LABELS(regLayout, PC, 0, 0);
	ADD_REGISTER_LABELS(regLayout, PR, 0, 2);
	regLayout->setColumnStretch(3, 1);

	lblR.resize(16);
	lblFR.resize(16);

	for (int i = 0; i < 16; i++) {
		QLabel* lblRName = newBoldLabel(QStringLiteral("r%1").arg(i));
		lblR[i] = new QLabel(formatHex32(0));

		QLabel* lblFRName = newBoldLabel(QStringLiteral("fr%1").arg(i));
		lblFR[i] = new QLabel(formatFloat32(0));

		auto row = regLayout->rowCount();
		regLayout->addWidget(lblRName, row, 0);
		regLayout->addWidget(lblR[i], row, 1);
		regLayout->addWidget(lblFRName, row, 2);
		regLayout->addWidget(lblFR[i], row, 3);
	}

	MAKE_REGISTER_LABELS(SR, "sr");
	MAKE_REGISTER_LABELS(FPSCR, "fpscr");

	QGridLayout* statusRegLayout = new QGridLayout();
	ADD_REGISTER_LABELS(statusRegLayout, SR, 0, 0);
	ADD_REGISTER_LABELS(statusRegLayout, FPSCR, 0, 2);
	statusRegLayout->setColumnStretch(3, 1);

	QCheckBox* cbAltBank = new QCheckBox(tr("View alt bank"));
	connect(cbAltBank, &QCheckBox::toggled, this, [this](bool checked) {
		altBank = checked;
		fetch();
	});

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addLayout(regLayout);
	mainLayout->addWidget(newHLine());
	mainLayout->addLayout(statusRegLayout);
	mainLayout->addWidget(cbAltBank);
	mainLayout->addStretch();

	setLayout(mainLayout);
}

void SH4RegistersGeneralTab::fetch() {
	lblPC->setText(formatHex32(Sh4cntx.pc));
	lblPR->setText(formatHex32(Sh4cntx.pr));

	for (int i = 0; i < 8; i++) {
		u32 r = !altBank ? Sh4cntx.r[i] : Sh4cntx.r_bank[i];
		lblR[i]->setText(formatHex32(r));
	}

	for (int i = 8; i < lblR.size(); i++) {
		lblR[i]->setText(formatHex32(Sh4cntx.r[i]));
	}

	for (int i = 0; i < lblFR.size(); i++) {
		f32 f = !altBank ? Sh4cntx.xffr[16 + i] : Sh4cntx.xffr[i];
		lblFR[i]->setText(formatFloat32(f));
	}

	lblSR->setText(formatHex32(Sh4cntx.sr.status));
	lblFPSCR->setText(formatHex32(Sh4cntx.fpscr.full));
}

SH4RegistersControlTab::SH4RegistersControlTab(QWidget* parent) :
	SH4RegistersAbstractTab(parent)
{
	setFont(QFont("monospace"));

	MAKE_REGISTER_LABELS(IMASK, "imask");
	MAKE_REGISTER_LABELS(SR, "sr");
	MAKE_REGISTER_CHECKBOX(T, "t");
	MAKE_REGISTER_CHECKBOX(S, "s");
	MAKE_REGISTER_CHECKBOX(Q, "q");
	MAKE_REGISTER_CHECKBOX(M, "m");
	MAKE_REGISTER_CHECKBOX(FD, "fd");
	MAKE_REGISTER_CHECKBOX(BL, "bl");
	MAKE_REGISTER_CHECKBOX(RB, "rb");
	MAKE_REGISTER_CHECKBOX(MD, "md");

	MAKE_REGISTER_LABELS(RM, "rm");
	MAKE_REGISTER_LABELS(FPSCR, "fpscr");
	MAKE_REGISTER_CHECKBOX(DN, "dn");
	MAKE_REGISTER_CHECKBOX(PR, "pr");
	MAKE_REGISTER_CHECKBOX(SZ, "sz");
	MAKE_REGISTER_CHECKBOX(FR, "fr");

	QGridLayout* srRegLayout = new QGridLayout();
	ADD_REGISTER_LABELS(srRegLayout, IMASK, 0, 0);
	ADD_REGISTER_LABELS(srRegLayout, SR, 0, 2);
	srRegLayout->setColumnStretch(3, 1);

	QGridLayout* srFlagLayout = new QGridLayout();
	ADD_REGISTER_CHECKBOX(srFlagLayout, T, 0, 0);
	ADD_REGISTER_CHECKBOX(srFlagLayout, S, 0, 1);
	ADD_REGISTER_CHECKBOX(srFlagLayout, Q, 0, 2);
	ADD_REGISTER_CHECKBOX(srFlagLayout, M, 0, 3);
	ADD_REGISTER_CHECKBOX(srFlagLayout, FD, 1, 0);
	ADD_REGISTER_CHECKBOX(srFlagLayout, BL, 1, 1);
	ADD_REGISTER_CHECKBOX(srFlagLayout, RB, 1, 2);
	ADD_REGISTER_CHECKBOX(srFlagLayout, MD, 1, 3);
	srFlagLayout->setColumnStretch(3, 1);

	QGridLayout* fpscrRegLayout = new QGridLayout();
	ADD_REGISTER_LABELS(fpscrRegLayout, RM, 0, 0);
	ADD_REGISTER_LABELS(fpscrRegLayout, FPSCR, 0, 2);
	fpscrRegLayout->setColumnStretch(3, 1);

	QGridLayout* fpscrFlagLayout = new QGridLayout();
	ADD_REGISTER_CHECKBOX(fpscrFlagLayout, DN, 0, 0);
	ADD_REGISTER_CHECKBOX(fpscrFlagLayout, PR, 0, 1);
	ADD_REGISTER_CHECKBOX(fpscrFlagLayout, SZ, 0, 2);
	ADD_REGISTER_CHECKBOX(fpscrFlagLayout, FR, 0, 3);
	fpscrFlagLayout->setColumnStretch(3, 1);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(newBoldLabel(tr("SR")));
	mainLayout->addLayout(srRegLayout);
	mainLayout->addLayout(srFlagLayout);
	mainLayout->addWidget(newHLine());
	mainLayout->addWidget(newBoldLabel(tr("FPSCR")));
	mainLayout->addLayout(fpscrRegLayout);
	mainLayout->addLayout(fpscrFlagLayout);
	mainLayout->addStretch();

	setLayout(mainLayout);
}

void SH4RegistersControlTab::fetch() {
	SET_REGISTER_LABEL(IMASK, Sh4cntx.sr.IMASK);
	SET_REGISTER_LABEL(SR, Sh4cntx.sr.status);
	SET_REGISTER_CHECKBOX(T, Sh4cntx.sr.T);
	SET_REGISTER_CHECKBOX(S, Sh4cntx.sr.S);
	SET_REGISTER_CHECKBOX(Q, Sh4cntx.sr.Q);
	SET_REGISTER_CHECKBOX(M, Sh4cntx.sr.M);
	SET_REGISTER_CHECKBOX(FD, Sh4cntx.sr.FD);
	SET_REGISTER_CHECKBOX(BL, Sh4cntx.sr.BL);
	SET_REGISTER_CHECKBOX(RB, Sh4cntx.sr.RB);
	SET_REGISTER_CHECKBOX(MD, Sh4cntx.sr.MD);

	SET_REGISTER_LABEL(RM, Sh4cntx.fpscr.RM);
	SET_REGISTER_LABEL(FPSCR, Sh4cntx.fpscr.full);
	SET_REGISTER_CHECKBOX(DN, Sh4cntx.fpscr.DN);
	SET_REGISTER_CHECKBOX(PR, Sh4cntx.fpscr.PR);
	SET_REGISTER_CHECKBOX(SZ, Sh4cntx.fpscr.SZ);
	SET_REGISTER_CHECKBOX(FR, Sh4cntx.fpscr.FR);
}

} // namespace qdbg
