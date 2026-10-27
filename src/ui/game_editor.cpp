#include "ui/game_editor.h"
#include "ui/ui_support.h"

#include <QCloseEvent>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QShortcut>
#include <QVBoxLayout>

namespace shelf {

GameEditor::GameEditor(StorageController& controller, std::optional<BoardGame> existing,
                       QWidget* parent)
    : QDialog(parent), controller_(controller), name_(new QLineEdit(this)),
      publisher_(new QLineEdit(this)), location_(new QLineEdit(this)),
      note_(new QPlainTextEdit(this)), error_(new QLabel(this)),
      save_(new QPushButton(QStringLiteral("Speichern"), this)),
      cancel_(new QPushButton(QStringLiteral("Abbrechen"), this)) {
  setObjectName(QStringLiteral("gameEditor"));
  setWindowTitle(existing ? QStringLiteral("Spiel bearbeiten")
                          : QStringLiteral("Spiel hinzufügen"));
  setWindowModality(Qt::WindowModal);
  resize(560, 610);
  setMinimumSize(320, 280);
  name_->setObjectName(QStringLiteral("gameName"));
  publisher_->setObjectName(QStringLiteral("gamePublisher"));
  location_->setObjectName(QStringLiteral("gameLocation"));
  note_->setObjectName(QStringLiteral("gameNote"));
  save_->setObjectName(QStringLiteral("saveGame"));
  cancel_->setObjectName(QStringLiteral("cancelEdit"));
  error_->setObjectName(QStringLiteral("editorError"));
  for (auto* field : {name_, publisher_, location_}) {
    field->setMaxLength(1000000);
    field->setClearButtonEnabled(true);
  }
  name_->setPlaceholderText(QStringLiteral("Wie heißt das Brettspiel?"));
  publisher_->setPlaceholderText(QStringLiteral("Optional"));
  location_->setPlaceholderText(QStringLiteral("Zum Beispiel Wohnzimmer, Regal 2"));
  note_->setPlaceholderText(QStringLiteral("Was möchtest du dir zu diesem Spiel merken?"));
  note_->setMinimumHeight(120);
  note_->setTabChangesFocus(true);
  error_->setTextFormat(Qt::PlainText);
  error_->setWordWrap(true);
  error_->setAccessibleName(QStringLiteral("Hinweis zum Speichern"));
  error_->hide();
  connect(name_, &QLineEdit::textChanged, this, [this] {
    if (validationError_ && !name_->text().trimmed().isEmpty()) {
      validationError_ = false;
      error_->hide();
    }
  });
  if (existing) {
    id_ = existing->id;
    name_->setText(existing->fields.name);
    publisher_->setText(existing->fields.publisher);
    location_->setText(existing->fields.location);
    note_->setPlainText(existing->fields.note);
  }
  original_ = draft();

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(20, 20, 20, 20);
  root->setSpacing(14);
  auto* scroll = new QScrollArea(this);
  scroll->setWidgetResizable(true);
  scroll->setFrameShape(QFrame::NoFrame);
  auto* formWidget = new QWidget(scroll);
  auto* form = new QFormLayout(formWidget);
  form->setRowWrapPolicy(QFormLayout::WrapAllRows);
  form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
  form->setSpacing(10);
  auto* hint = new QLabel(QStringLiteral("Nur der Spielname ist erforderlich."), formWidget);
  hint->setWordWrap(true);
  form->addRow(hint);
  form->addRow(QStringLiteral("&Spielname *"), name_);
  form->addRow(QStringLiteral("&Verlag oder Hersteller"), publisher_);
  form->addRow(QStringLiteral("&Aufbewahrungsort"), location_);
  form->addRow(QStringLiteral("&Notiz"), note_);
  form->addRow(error_);
  scroll->setWidget(formWidget);
  root->addWidget(scroll, 1);
  auto* buttons = new QDialogButtonBox(this);
  buttons->addButton(cancel_, QDialogButtonBox::RejectRole);
  buttons->addButton(save_, QDialogButtonBox::AcceptRole);
  root->addWidget(buttons);
  save_->setDefault(true);
  connect(save_, &QPushButton::clicked, this, &GameEditor::save);
  connect(cancel_, &QPushButton::clicked, this, &GameEditor::reject);
  auto* saveShortcut = new QShortcut(QKeySequence::Save, this);
  connect(saveShortcut, &QShortcut::activated, this, &GameEditor::save);
  connect(&controller_, &StorageController::saved, this, [this](const BoardGame&) {
    if (saving_) {
      saving_ = false;
      accept();
    }
  });
  connect(&controller_, &StorageController::failed, this,
          [this, scroll](Operation operation, const QString& message) {
            if (operation == Operation::Save && saving_) {
              setSaving(false);
              error_->setText(message);
              error_->show();
              scroll->ensureWidgetVisible(error_);
            }
          });
  name_->setFocus();
}

GameDraft GameEditor::draft() const {
  return {name_->text(), publisher_->text(), location_->text(), note_->toPlainText()};
}

void GameEditor::setSaving(bool saving) {
  saving_ = saving;
  name_->setEnabled(!saving);
  publisher_->setEnabled(!saving);
  location_->setEnabled(!saving);
  note_->setEnabled(!saving);
  save_->setEnabled(!saving);
  cancel_->setEnabled(!saving);
  save_->setText(saving ? QStringLiteral("Wird gespeichert …") : QStringLiteral("Speichern"));
}

void GameEditor::save() {
  if (saving_ || controller_.isBusy()) {
    return;
  }
  try {
    const auto validated = validateDraft(draft());
    validationError_ = false;
    error_->hide();
    setSaving(true);
    if (!controller_.save(id_, validated)) {
      setSaving(false);
    }
  } catch (const ValidationError& error) {
    validationError_ = true;
    error_->setText(QString::fromUtf8(error.what()));
    error_->show();
    name_->setFocus();
  }
}

bool GameEditor::requestClose() {
  if (saving_) {
    return false;
  }
  if (draft() != original_ && !confirmDiscard(this)) {
    return false;
  }
  QDialog::reject();
  return true;
}

void GameEditor::reject() { requestClose(); }

void GameEditor::closeEvent(QCloseEvent* event) {
  if (requestClose()) {
    event->accept();
  } else {
    event->ignore();
  }
}

} // namespace shelf
