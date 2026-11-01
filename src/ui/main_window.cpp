#include "ui/main_window.h"
#include "ui/language_dialog.h"
#include "ui/sort_header.h"
#include "ui/ui_support.h"
#include <QCoreApplication>

#include <QApplication>
#include <QCloseEvent>
#include <QDialogButtonBox>
#include <QFile>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QShortcut>
#include <QSplitter>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>
#include <utility>

namespace shelf {

MainWindow::MainWindow(StorageController& controller, QString dataDirectory,
                       QString languagePreference, QWidget* parent)
    : QMainWindow(parent), controller_(controller), dataDirectory_(std::move(dataDirectory)),
      languagePreference_(std::move(languagePreference)), search_(new QLineEdit(this)),
      table_(new QTableWidget(this)), note_(new QPlainTextEdit(this)),
      detailTitle_(new QLabel(this)), state_(new QLabel(this)), feedback_(new QLabel(this)),
      count_(new QLabel(this)),
      add_(new QPushButton(QCoreApplication::translate("App", "Spiel hinzufügen"), this)),
      edit_(new QPushButton(QCoreApplication::translate("App", "Bearbeiten"), this)),
      remove_(new QPushButton(QCoreApplication::translate("App", "Löschen"), this)),
      reload_(new QPushButton(QCoreApplication::translate("App", "Neu laden"), this)),
      language_(new QPushButton(QCoreApplication::translate("App", "Sprache"), this)) {
  setWindowTitle(QStringLiteral("BrettspielRegal"));
  setObjectName(QStringLiteral("mainWindow"));
  resize(1000, 740);
  setMinimumSize(440, 420);
  auto* central = new QWidget(this);
  auto* root = new QVBoxLayout(central);
  root->setContentsMargins(24, 22, 24, 22);
  root->setSpacing(14);
  auto* header = new QHBoxLayout;
  auto* title = new QLabel(QStringLiteral("BrettspielRegal"), central);
  QFont titleFont = title->font();
  titleFont.setPointSize(22);
  titleFont.setBold(true);
  title->setFont(titleFont);
  header->addWidget(title, 1);
  add_->setObjectName(QStringLiteral("addGame"));
  header->addWidget(add_, 0, Qt::AlignVCenter);
  auto* about = new QPushButton(QCoreApplication::translate("App", "Über die App"), central);
  about->setObjectName(QStringLiteral("aboutApp"));
  root->addLayout(header);
  language_->setObjectName(QStringLiteral("changeLanguage"));
  auto* subtitle = new QLabel(
      QCoreApplication::translate("App", "Deine Spiele. Dein Platz für alles, was dazugehört."),
      central);
  subtitle->setWordWrap(true);
  root->addWidget(subtitle);
  search_->setObjectName(QStringLiteral("searchGames"));
  search_->setAccessibleName(QCoreApplication::translate("App", "Sammlung durchsuchen"));
  search_->setPlaceholderText(
      QCoreApplication::translate("App", "Name, Verlag, Aufbewahrungsort oder Notiz suchen …"));
  search_->setClearButtonEnabled(true);
  auto* clear = new QPushButton(QCoreApplication::translate("App", "Suche leeren"), central);
  clear->setObjectName(QStringLiteral("clearSearch"));
  auto* searchRow = new QHBoxLayout;
  searchRow->addWidget(search_, 1);
  searchRow->addWidget(clear);
  search_->setMinimumHeight(36);
  root->addLayout(searchRow);

  auto* splitter = new QSplitter(Qt::Vertical, central);
  auto* collection = new QWidget(splitter);
  auto* collectionLayout = new QVBoxLayout(collection);
  collectionLayout->setContentsMargins(0, 0, 0, 0);
  state_->setObjectName(QStringLiteral("collectionState"));
  state_->setWordWrap(true);
  state_->setTextFormat(Qt::PlainText);
  collectionLayout->addWidget(state_);
  table_->setObjectName(QStringLiteral("gamesTable"));
  table_->setAccessibleName(QCoreApplication::translate("App", "Brettspielsammlung"));
  table_->setColumnCount(3);
  auto* sortHeader = new SortHeader(table_);
  table_->setHorizontalHeader(sortHeader);
  table_->setHorizontalHeaderLabels({QCoreApplication::translate("App", "Spielname"),
                                     QCoreApplication::translate("App", "Verlag / Hersteller"),
                                     QCoreApplication::translate("App", "Aufbewahrungsort")});
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_->setSelectionMode(QAbstractItemView::SingleSelection);
  table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table_->setAlternatingRowColors(true);
  table_->setShowGrid(false);
  table_->verticalHeader()->hide();
  table_->verticalHeader()->setDefaultSectionSize(table_->fontMetrics().height() + 20);
  table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  table_->horizontalHeader()->setMinimumSectionSize(105);
  table_->installEventFilter(this);
  collectionLayout->addWidget(table_, 1);
  auto* actions = new QHBoxLayout;
  count_->setObjectName(QStringLiteral("gameCount"));
  edit_->setObjectName(QStringLiteral("editGame"));
  remove_->setObjectName(QStringLiteral("removeGame"));
  actions->setContentsMargins(0, 10, 0, 6);
  actions->addWidget(count_, 1);
  actions->addWidget(edit_);
  actions->addWidget(remove_);
  collectionLayout->addLayout(actions);
  auto* detail = new QWidget(splitter);
  auto* detailLayout = new QVBoxLayout(detail);
  detailLayout->setContentsMargins(0, 0, 0, 0);
  detailTitle_->setTextFormat(Qt::PlainText);
  detailTitle_->setWordWrap(true);
  QFont detailFont = detailTitle_->font();
  detailFont.setBold(true);
  detailTitle_->setFont(detailFont);
  detailLayout->addWidget(detailTitle_);
  note_->setObjectName(QStringLiteral("selectedNote"));
  note_->setAccessibleName(QCoreApplication::translate("App", "Vollständige Notiz"));
  note_->setReadOnly(true);
  note_->setTabChangesFocus(true);
  note_->setMinimumHeight(72);
  detailLayout->addWidget(note_);
  splitter->addWidget(collection);
  splitter->addWidget(detail);
  splitter->setStretchFactor(0, 4);
  splitter->setStretchFactor(1, 1);
  splitter->setSizes({420, 110});
  splitter->setChildrenCollapsible(false);
  root->addWidget(splitter, 1);
  feedback_->setObjectName(QStringLiteral("mainFeedback"));
  feedback_->setTextFormat(Qt::PlainText);
  feedback_->setWordWrap(true);
  feedback_->setTextInteractionFlags(Qt::TextSelectableByMouse);
  root->addWidget(feedback_);

  auto* footer = new QHBoxLayout;
  auto* localStorage = new QLabel(
      QCoreApplication::translate("App", "Lokal auf diesem Computer gespeichert"), central);
  localStorage->setWordWrap(true);
  footer->addWidget(localStorage, 1);
  footer->addWidget(about);
  footer->addWidget(language_);
  root->addLayout(footer);
  for (auto* button : {add_, edit_, remove_, clear, about, language_}) {
    button->setMinimumHeight(36);
  }
  reload_->setObjectName(QStringLiteral("reloadGames"));
  root->addWidget(reload_, 0, Qt::AlignRight);
  reload_->hide();
  setCentralWidget(central);
  setTabOrder(add_, search_);
  setTabOrder(search_, clear);
  setTabOrder(clear, table_);
  setTabOrder(table_, edit_);
  setTabOrder(edit_, remove_);
  setTabOrder(remove_, note_);
  setTabOrder(note_, about);
  setTabOrder(about, language_);

  connect(about, &QPushButton::clicked, this, &MainWindow::showAbout);
  connect(language_, &QPushButton::clicked, this, &MainWindow::showLanguage);
  auto* languageShortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+,")), this);
  connect(languageShortcut, &QShortcut::activated, this, &MainWindow::showLanguage);
  connect(&controller_, &StorageController::languageSaved, this, [this](const QString& preference) {
    languagePreference_ = preference;
    feedback_->setText(
        QCoreApplication::translate("App", "Sprache gespeichert. Sie gilt beim nächsten Start."));
  });
  connect(clear, &QPushButton::clicked, search_, &QLineEdit::clear);
  connect(search_, &QLineEdit::textChanged, this, [this, clear] {
    clear->setEnabled(!search_->text().isEmpty());
    const auto selected = selectedGame();
    refresh(selected ? selected->id : 0);
  });
  clear->setEnabled(false);
  connect(sortHeader, &QHeaderView::sortIndicatorChanged, this, [this] {
    const auto selected = selectedGame();
    refresh(selected ? selected->id : 0);
  });
  connect(table_, &QTableWidget::itemSelectionChanged, this, &MainWindow::updateSelection);
  connect(table_, &QTableWidget::cellDoubleClicked, this, [this] { openEditor(true); });
  connect(add_, &QPushButton::clicked, this, [this] { openEditor(false); });
  connect(edit_, &QPushButton::clicked, this, [this] { openEditor(true); });
  connect(remove_, &QPushButton::clicked, this, &MainWindow::deleteSelected);
  connect(reload_, &QPushButton::clicked, &controller_, &StorageController::load);
  auto* newShortcut = new QShortcut(QKeySequence::New, this);
  connect(newShortcut, &QShortcut::activated, this, [this] { openEditor(false); });
  auto* findShortcut = new QShortcut(QKeySequence::Find, this);
  connect(findShortcut, &QShortcut::activated, this, [this] {
    search_->setFocus();
    search_->selectAll();
  });
  auto* quitShortcut = new QShortcut(QKeySequence::Quit, this);
  connect(quitShortcut, &QShortcut::activated, this, &QWidget::close);
  connect(&controller_, &StorageController::busyChanged, this, [this](bool busy) {
    updateActions();
    if (busy) {
      feedback_->setText(QCoreApplication::translate("App", "Daten werden verarbeitet …"));
    }
  });
  connect(&controller_, &StorageController::loaded, this, [this](const GameList& games) {
    const auto selected = selectedGame();
    games_ = games;
    ready_ = true;
    feedback_->clear();
    reload_->hide();
    refresh(selected ? selected->id : 0);
  });
  connect(&controller_, &StorageController::saved, this, [this](const BoardGame& game) {
    auto found = std::find_if(games_.begin(), games_.end(),
                              [&game](const BoardGame& row) { return row.id == game.id; });
    if (found == games_.end()) {
      games_.append(game);
    } else {
      *found = game;
    }
    refresh(game.id);
    const bool visible = std::any_of(visibleGames_.cbegin(), visibleGames_.cend(),
                                     [&game](const BoardGame& row) { return row.id == game.id; });
    feedback_->setText(
        visible
            ? QCoreApplication::translate("App", "„%1“ wurde gespeichert.").arg(game.fields.name)
            : QCoreApplication::translate(
                  "App", "„%1“ wurde gespeichert, passt aber nicht zur aktuellen Suche. "
                         "Mit „Suche leeren“ findest du es wieder.")
                  .arg(game.fields.name));
    reload_->hide();
  });
  connect(&controller_, &StorageController::removed, this, [this](qint64 id) {
    const int row = table_->currentRow();
    games_.erase(std::remove_if(games_.begin(), games_.end(),
                                [id](const BoardGame& game) { return game.id == id; }),
                 games_.end());
    refresh(0, row);
    feedback_->setText(QCoreApplication::translate("App", "Das Spiel wurde gelöscht."));
    reload_->hide();
  });
  connect(&controller_, &StorageController::failed, this,
          [this](Operation operation, const QString& message) {
            feedback_->setText(message);
            if (operation != Operation::Save && operation != Operation::Language) {
              reload_->show();
            }
            if (!ready_) {
              state_->setText(
                  QCoreApplication::translate("App", "Die Sammlung konnte nicht geöffnet werden."));
              count_->clear();
            }
            updateActions();
          });
  refresh();
  QTimer::singleShot(0, &controller_, &StorageController::load);
}

std::optional<BoardGame> MainWindow::selectedGame() const {
  const int row = table_->currentRow();
  if (row < 0 || row >= visibleGames_.size()) {
    return std::nullopt;
  }
  return visibleGames_.at(row);
}

void MainWindow::refresh(std::optional<qint64> preferredId, int fallbackRow) {
  const QSignalBlocker block(table_);
  const auto* header = table_->horizontalHeader();
  visibleGames_ = searchGames(games_, search_->text(),
                              static_cast<GameSortField>(header->sortIndicatorSection()),
                              header->sortIndicatorOrder());
  table_->setRowCount(static_cast<int>(visibleGames_.size()));
  int selection = visibleGames_.isEmpty()
                      ? -1
                      : std::clamp(fallbackRow, 0, static_cast<int>(visibleGames_.size()) - 1);
  for (int row = 0; row < visibleGames_.size(); ++row) {
    const auto& game = visibleGames_.at(row);
    const QStringList values{game.fields.name, game.fields.publisher, game.fields.location};
    for (int column = 0; column < values.size(); ++column) {
      auto* item = new QTableWidgetItem(values.at(column));
      item->setData(Qt::UserRole, game.id);
      item->setToolTip(values.at(column));
      table_->setItem(row, column, item);
    }
    if (preferredId && game.id == *preferredId) {
      selection = row;
    }
  }
  table_->clearSelection();
  if (selection >= 0) {
    table_->setCurrentCell(selection, 0);
    table_->selectRow(selection);
  } else {
    table_->setCurrentCell(-1, -1);
  }
  state_->setText(!ready_ ? QCoreApplication::translate("App", "Sammlung wird geladen …")
                  : games_.isEmpty()
                      ? QCoreApplication::translate(
                            "App", "Dein Regal ist noch leer. Füge dein erstes Brettspiel hinzu.")
                  : visibleGames_.isEmpty()
                      ? QCoreApplication::translate(
                            "App", "Keine Spiele gefunden. Ändere oder leere die Suche.")
                      : QString());
  state_->setVisible(!state_->text().isEmpty());
  count_->setText(
      ready_ ? QCoreApplication::translate("App", "Spiele: %1 / %2")
                   .arg(QLocale().toString(visibleGames_.size()), QLocale().toString(games_.size()))
             : QString());
  updateSelection();
}

void MainWindow::updateSelection() {
  const auto selected = selectedGame();
  detailTitle_->setText(
      selected ? QCoreApplication::translate("App", "Notiz zu „%1“").arg(selected->fields.name)
               : QCoreApplication::translate("App", "Notiz"));
  note_->setPlainText(selected ? selected->fields.note : QString());
  note_->setPlaceholderText(
      selected
          ? QCoreApplication::translate("App", "Zu diesem Spiel gibt es noch keine Notiz.")
          : QCoreApplication::translate("App", "Wähle ein Spiel aus, um seine Notiz zu lesen."));
  updateActions();
}

void MainWindow::updateActions() {
  const bool available = ready_ && !controller_.isBusy() && (!editor_ || !editor_->isVisible());
  add_->setEnabled(available);
  language_->setEnabled(available);
  edit_->setEnabled(available && selectedGame().has_value());
  remove_->setEnabled(available && selectedGame().has_value());
  reload_->setEnabled(!controller_.isBusy() && (!editor_ || !editor_->isVisible()));
}

void MainWindow::openEditor(bool editing) {
  if (!ready_ || controller_.isBusy() || (editor_ && editor_->isVisible())) {
    return;
  }
  const auto selected = editing ? selectedGame() : std::nullopt;
  if (editing && !selected) {
    return;
  }
  editor_ = new GameEditor(controller_, selected, this);
  editor_->setAttribute(Qt::WA_DeleteOnClose);
  connect(editor_, &QDialog::finished, this, [this] {
    editor_ = nullptr;
    updateActions();
  });
  editor_->open();
  updateActions();
}

void MainWindow::deleteSelected() {
  const auto selected = selectedGame();
  if (!selected || !remove_->isEnabled()) {
    return;
  }
  if (confirmDelete(this, selected->fields.name)) {
    controller_.remove(selected->id);
  }
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
  if (watched == table_ && event->type() == QEvent::KeyPress) {
    const auto* key = static_cast<QKeyEvent*>(event);
    if (key->key() == Qt::Key_Return || key->key() == Qt::Key_Enter) {
      openEditor(true);
      return true;
    }
    if (key->key() == Qt::Key_Delete ||
        (key->key() == Qt::Key_Backspace && key->modifiers().testFlag(Qt::ControlModifier))) {
      deleteSelected();
      return true;
    }
  }
  return QMainWindow::eventFilter(watched, event);
}

void MainWindow::closeEvent(QCloseEvent* event) {
  if (controller_.isBusy()) {
    feedback_->setText(QCoreApplication::translate(
        "App",
        "Bitte warte, bis der laufende Speichervorgang beendet ist, und schließe dann erneut."));
    event->ignore();
    return;
  }
  if (editor_ && editor_->isVisible() && !editor_->requestClose()) {
    event->ignore();
    return;
  }
  event->accept();
}

void MainWindow::showAbout() {
  QDialog dialog(this);
  dialog.setWindowTitle(QCoreApplication::translate("App", "Über BrettspielRegal"));
  dialog.resize(640, 500);
  auto* layout = new QVBoxLayout(&dialog);
  auto* text = new QTextBrowser(&dialog);
  QFile notices(QStringLiteral(":/licenses/THIRD_PARTY_NOTICES.md"));
  QString licenseText;
  if (notices.open(QIODevice::ReadOnly)) {
    licenseText = QString::fromUtf8(notices.readAll());
  }
  text->setPlainText(
      QCoreApplication::translate(
          "App", "BrettspielRegal %1\n\nDeine Sammlung bleibt lokal.\nDatenverzeichnis: %2\n\n%3")
          .arg(QCoreApplication::applicationVersion(), dataDirectory_, licenseText));
  layout->addWidget(text);
  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
  buttons->button(QDialogButtonBox::Close)
      ->setText(QCoreApplication::translate("App", "Schließen"));
  connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
  layout->addWidget(buttons);
  dialog.exec();
}

void MainWindow::showLanguage() {
  if (!language_->isEnabled()) {
    return;
  }
  LanguageDialog dialog(controller_, dataDirectory_, languagePreference_, this);
  dialog.exec();
}

} // namespace shelf
