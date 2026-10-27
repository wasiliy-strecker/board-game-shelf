#pragma once

#include "ui/game_editor.h"

#include <QMainWindow>
#include <QPointer>

class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QTableWidget;

namespace shelf {

class MainWindow final : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(StorageController& controller, QString dataDirectory, QWidget* parent = nullptr);

protected:
  void closeEvent(QCloseEvent* event) override;
  bool eventFilter(QObject* watched, QEvent* event) override;

private:
  void refresh(std::optional<qint64> preferredId = std::nullopt, int fallbackRow = 0);
  void updateSelection();
  void updateActions();
  std::optional<BoardGame> selectedGame() const;
  void openEditor(bool editing);
  void deleteSelected();
  void showAbout();
  StorageController& controller_;
  QString dataDirectory_;
  GameList games_;
  GameList visibleGames_;
  QPointer<GameEditor> editor_;
  QLineEdit* search_;
  QTableWidget* table_;
  QPlainTextEdit* note_;
  QLabel* detailTitle_;
  QLabel* state_;
  QLabel* feedback_;
  QLabel* count_;
  QPushButton* add_;
  QPushButton* edit_;
  QPushButton* remove_;
  QPushButton* reload_;
  bool ready_ = false;
};

} // namespace shelf
