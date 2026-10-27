#pragma once

#include "ui/storage_controller.h"

#include <QDialog>
#include <optional>

class QCloseEvent;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;

namespace shelf {

class GameEditor final : public QDialog {
  Q_OBJECT
public:
  GameEditor(StorageController& controller, std::optional<BoardGame> existing,
             QWidget* parent = nullptr);
  GameDraft draft() const;
  bool requestClose();

public slots:
  void reject() override;
  void save();

protected:
  void closeEvent(QCloseEvent* event) override;

private:
  void setSaving(bool saving);
  StorageController& controller_;
  qint64 id_ = 0;
  GameDraft original_;
  QLineEdit* name_;
  QLineEdit* publisher_;
  QLineEdit* location_;
  QPlainTextEdit* note_;
  QLabel* error_;
  QPushButton* save_;
  QPushButton* cancel_;
  bool saving_ = false;
  bool validationError_ = false;
};

} // namespace shelf
