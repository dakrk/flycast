#pragma once
#include <QString>
#include "types.h"

class QFrame;
class QScrollArea;
class QWidget;

namespace qdbg {

inline QString formatHex32(u32 num) {
	return QStringLiteral("%1").arg(num, 8, 16, QChar('0'));
}

inline QString formatFloat32(f32 num) {
	return QStringLiteral("%1").arg(num, 0, 'f', 5);
}

QFrame* newHLine(QWidget* parent = nullptr);
QScrollArea* newScrollWidget(QWidget* widget, QWidget* parent = nullptr);

} // namespace qdbg
