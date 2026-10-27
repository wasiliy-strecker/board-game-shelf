#pragma once

#include <QString>

namespace shelf {
QString defaultDataDirectory();
QString resolveDataDirectory(const QString& overridePath, bool explicitlyProvided);
} // namespace shelf
