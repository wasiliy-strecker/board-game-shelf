#pragma once

#include <QString>

class QWidget;
class QDir;

namespace shelf {
QString exportFilename(const QString& entered);
QString chooseExportFile(QWidget* parent, const QDir& directory, const QString& suggestedName,
                         bool selectDirectory);
} // namespace shelf
