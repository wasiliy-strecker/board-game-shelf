#pragma once

#include <QWidget>

namespace shelf {
bool confirmDiscard(QWidget* parent);
bool confirmDelete(QWidget* parent, const QString& name);
} // namespace shelf
