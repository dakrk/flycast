#include <QFrame>
#include <QScrollArea>
#include "utils.h"

namespace qdbg {

QFrame* newHLine(QWidget* parent) {
	QFrame* line = new QFrame(parent);
	line->setFrameShape(QFrame::Shape::HLine);
	line->setFrameShadow(QFrame::Shadow::Sunken);
	return line;
}

QScrollArea* newScrollWidget(QWidget* widget, QWidget* parent) {
	QScrollArea* scroll = new QScrollArea(parent);
	scroll->setWidget(widget);
	return scroll;
}

} // namespace qdbg
