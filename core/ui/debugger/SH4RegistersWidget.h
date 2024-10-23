#pragma once
#include <QLabel>
#include <QVector>
#include <QWidget>

namespace qdbg {

class SH4RegistersWidget : public QWidget {
	Q_OBJECT
public:
	explicit SH4RegistersWidget(QWidget* parent = nullptr);
	void fetch();

private:
	QLabel* lblPC;
	QLabel* lblPR;
	QVector<QLabel*> lblR;
	QVector<QLabel*> lblFR;
};

} // namespace qdbg
