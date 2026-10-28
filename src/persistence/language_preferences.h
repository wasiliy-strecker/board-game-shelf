#pragma once

#include <QDir>

namespace shelf {
struct LanguagePreference {
  QString value = QStringLiteral("auto");
  bool readFailed = false;
};
LanguagePreference readLanguagePreference(const QString& directory);
void saveLanguagePreference(const QDir& directory, const QString& preference);
} // namespace shelf
