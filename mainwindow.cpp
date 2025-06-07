#include "mainwindow.h"

#include <QtWidgets>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QAudioOutput>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Apply the application icon from resources. See instructions on creating a .qrc file.
    setWindowIcon(QIcon(":/logo.png"));

    setupUi();
    setupMenuBar();
    setupConnections();
    setupShortcuts();
    setWindowModified(false); // Start in an unmodified state
    updateWindowTitle();
    resize(840, 560);
}

MainWindow::~MainWindow()
{
    // Qt's parent-child model handles memory management of widgets
}

void MainWindow::setupUi()
{
    // --- Left (Media) Panel ---
    mediaPanel = new QWidget;

    // Stacked widget to hold either the video player or an image viewer
    mediaStack = new QStackedWidget;
    videoWidget = new QVideoWidget;
    imageDisplayLabel = new QLabel("Open a video or image file to begin");
    imageDisplayLabel->setAlignment(Qt::AlignCenter);
    imageDisplayLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    mediaStack->addWidget(videoWidget);
    mediaStack->addWidget(imageDisplayLabel);

    // Media player and audio output setup
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this); // <-- CRITICAL FIX for audio playback
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(videoWidget);


    // Media Controls
    openMediaButton = new QPushButton("Open Media...");
    playPauseButton = new QPushButton("▶");
    playPauseButton->setFixedSize(32, 32);

    mediaPositionSlider = new QSlider(Qt::Horizontal);
    mediaTimeLabel = new QLabel("--:--:-- / --:--:--");

    muteButton = new QPushButton();
    muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    muteButton->setFixedSize(32, 32);

    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(75);
    volumeSlider->setFixedWidth(120);
    setVolume(75); // Set initial volume

    // Layout for Media Controls
    QHBoxLayout *mediaControlsLayout = new QHBoxLayout;
    mediaControlsLayout->addWidget(openMediaButton);
    mediaControlsLayout->addWidget(playPauseButton);
    mediaControlsLayout->addWidget(mediaPositionSlider);
    mediaControlsLayout->addWidget(mediaTimeLabel);
    mediaControlsLayout->addSpacing(20);
    mediaControlsLayout->addWidget(muteButton);
    mediaControlsLayout->addWidget(volumeSlider);

    QVBoxLayout *mediaLayout = new QVBoxLayout(mediaPanel);
    mediaLayout->addWidget(mediaStack);
    mediaLayout->addLayout(mediaControlsLayout);

    setMediaControlsEnabled(false); // Initially disabled

    // ... The rest of the setupUi function for the right panel is unchanged ...
    // --- Right  Panel ---
    thePanel = new QWidget;
    QGroupBox *caseInfoBox = new QGroupBox("Case Information");
    caseNameEdit = new QLineEdit;
    subjectTargetEdit = new QLineEdit;
    QFormLayout *caseInfoLayout = new QFormLayout;
    caseInfoLayout->addRow("Case Name:", caseNameEdit);
    caseInfoLayout->addRow("Subject/Target:", subjectTargetEdit);
    caseInfoBox->setLayout(caseInfoLayout);
    theTabs = new QTabWidget;
    notesTab = new QWidget;
    notesTextEdit = new QTextEdit;
    notesTextEdit->setPlaceholderText("Enter observations, notes, and analysis here...");
    addTimestampButton = new QPushButton("Add Timestamp");
    QVBoxLayout *notesLayout = new QVBoxLayout(notesTab);
    notesLayout->addWidget(notesTextEdit);
    notesLayout->addWidget(addTimestampButton, 0, Qt::AlignRight);
    auto createTableTab = [&](QTableWidget* &table, QPushButton* &button, const QStringList &headers, const QString &buttonText) {
        QWidget *tab = new QWidget;
        table = new QTableWidget(0, headers.count());
        table->setHorizontalHeaderLabels(headers);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setContextMenuPolicy(Qt::CustomContextMenu);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        button = new QPushButton(buttonText);
        QVBoxLayout *layout = new QVBoxLayout(tab);
        layout->addWidget(table);
        layout->addWidget(button, 0, Qt::AlignRight);
        return tab;
    };
    entitiesTab = createTableTab(entitiesTable, addEntityButton, {"Timestamp", "Entity Name", "Type", "Notes"}, "Add Entity");
    eventsTab = createTableTab(eventsTable, addEventButton, {"Start Time", "End Time", "Event Description"}, "Log Event");
    resourcesTab = createTableTab(resourcesTable, addResourceButton, {"URL / File Path", "Description", "Date Accessed"}, "Add Resource");
    theTabs->addTab(notesTab, "Notes");
    theTabs->addTab(entitiesTab, "Entities");
    theTabs->addTab(eventsTab, "Event Timeline");
    theTabs->addTab(resourcesTab, "Web Resources");
    QVBoxLayout *theLayout = new QVBoxLayout(thePanel);
    theLayout->addWidget(caseInfoBox);
    theLayout->addWidget(theTabs);
    mainSplitter = new QSplitter(Qt::Horizontal);
    mainSplitter->addWidget(mediaPanel);
    mainSplitter->addWidget(thePanel);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 1);
    mainSplitter->setSizes({700, 700});
    setCentralWidget(mainSplitter);
    statusBar()->showMessage("Ready. Create a new case or open an existing one to begin.");
}

void MainWindow::setupMenuBar()
{
    // This function remains unchanged
    fileMenu = menuBar()->addMenu("&File");
    newCaseAction = fileMenu->addAction("&New Case");
    openCaseAction = fileMenu->addAction("&Open Case...");
    fileMenu->addSeparator();
    saveCaseAction = fileMenu->addAction("&Save");
    saveCaseAsAction = fileMenu->addAction("Save &As...");
    fileMenu->addSeparator();
    exitAction = fileMenu->addAction("E&xit");

    exportMenu = menuBar()->addMenu("&Export");
    exportEntitiesAction = exportMenu->addAction("Export &Entities to CSV...");
    exportEventsAction = exportMenu->addAction("Export &Events to CSV...");
    exportResourcesAction = exportMenu->addAction("Export &Resources to CSV...");
}

void MainWindow::setupConnections()
{
    // ... File and Export action connections are unchanged ...
    connect(newCaseAction, &QAction::triggered, this, &MainWindow::newCase);
    connect(openCaseAction, &QAction::triggered, this, &MainWindow::openCase);
    connect(saveCaseAction, &QAction::triggered, this, &MainWindow::saveCase);
    connect(saveCaseAsAction, &QAction::triggered, this, &MainWindow::saveCaseAs);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(exportEntitiesAction, &QAction::triggered, this, &MainWindow::exportTableToCsv);
    connect(exportEventsAction, &QAction::triggered, this, &MainWindow::exportTableToCsv);
    connect(exportResourcesAction, &QAction::triggered, this, &MainWindow::exportTableToCsv);


    // Media Controls
    connect(openMediaButton, &QPushButton::clicked, this, &MainWindow::openMediaFile);
    connect(playPauseButton, &QPushButton::clicked, this, &MainWindow::playPause);
    connect(mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &MainWindow::updatePlayPauseButton);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::mediaPositionChanged);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::mediaDurationChanged);
    connect(mediaPositionSlider, &QSlider::sliderMoved, this, &MainWindow::setMediaPosition);

    // Audio Controls
    connect(volumeSlider, &QSlider::valueChanged, this, &MainWindow::setVolume);
    connect(muteButton, &QPushButton::clicked, this, &MainWindow::toggleMute);


    // ... Data entry and other connections are unchanged ...
    connect(addTimestampButton, &QPushButton::clicked, this, &MainWindow::addTimestampToNotes);
    connect(addEntityButton, &QPushButton::clicked, this, &MainWindow::addEntityRow);
    connect(addEventButton, &QPushButton::clicked, this, &MainWindow::addEventRow);
    connect(addResourceButton, &QPushButton::clicked, this, &MainWindow::addResourceRow);
    connect(entitiesTable, &QWidget::customContextMenuRequested, this, &MainWindow::showTableContextMenu);
    connect(eventsTable, &QWidget::customContextMenuRequested, this, &MainWindow::showTableContextMenu);
    connect(resourcesTable, &QWidget::customContextMenuRequested, this, &MainWindow::showTableContextMenu);
    connect(caseNameEdit, &QLineEdit::textChanged, this, [this](){ setWindowModified(true); updateWindowTitle(); });
    connect(subjectTargetEdit, &QLineEdit::textChanged, this, [this](){ setWindowModified(true); });
    connect(notesTextEdit, &QTextEdit::textChanged, this, [this](){ setWindowModified(true); });
    connect(entitiesTable->model(), &QAbstractItemModel::dataChanged, this, [this](){ setWindowModified(true); });
    connect(eventsTable->model(), &QAbstractItemModel::dataChanged, this, [this](){ setWindowModified(true); });
    connect(resourcesTable->model(), &QAbstractItemModel::dataChanged, this, [this](){ setWindowModified(true); });
}

void MainWindow::setupShortcuts() {
    // This function remains unchanged
    newCaseAction->setShortcut(QKeySequence::New);
    openCaseAction->setShortcut(QKeySequence::Open);
    saveCaseAction->setShortcut(QKeySequence::Save);
    saveCaseAsAction->setShortcut(QKeySequence::SaveAs);
    exitAction->setShortcut(QKeySequence::Quit);
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), this, SLOT(addTimestampToNotes()));
}

void MainWindow::openMediaFile() {
    const QStringList videoExtensions = {"*.mp4", "*.avi", "*.mov", "*.mkv", "*.wmv"};
    const QStringList imageExtensions = {"*.png", "*.jpg", "*.jpeg", "*.bmp", "*.gif"};

    QString filter = "All Media Files (" + videoExtensions.join(" ") + " " + imageExtensions.join(" ") + ");;";
    filter += "Video Files (" + videoExtensions.join(" ") + ");;";
    filter += "Image Files (" + imageExtensions.join(" ") + ");;";
    filter += "All Files (*)";

    QString fileName = QFileDialog::getOpenFileName(this, "Open Media", QDir::homePath(), filter);
    if (fileName.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(fileName);
    QString extension = "*." + fileInfo.suffix().toLower();

    if (videoExtensions.contains(extension, Qt::CaseInsensitive)) {
        mediaStack->setCurrentWidget(videoWidget);
        mediaPlayer->setSource(QUrl::fromLocalFile(fileName));
        setMediaControlsEnabled(true);
        playPause();
    } else if (imageExtensions.contains(extension, Qt::CaseInsensitive)) {
        QPixmap pixmap(fileName);
        if (pixmap.isNull()) {
            QMessageBox::warning(this, "Error", "Could not load the selected image.");
            return;
        }
        imageDisplayLabel->setPixmap(pixmap.scaled(imageDisplayLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        mediaStack->setCurrentWidget(imageDisplayLabel);
        setMediaControlsEnabled(false);
    } else {
        QMessageBox::information(this, "Unsupported File", "The selected file format is not supported.");
    }
}

void MainWindow::setMediaControlsEnabled(bool enabled) {
    playPauseButton->setEnabled(enabled);
    mediaPositionSlider->setEnabled(enabled);
    // Audio controls are always enabled
}

void MainWindow::setVolume(int volume) {
    // QAudioOutput volume is a float between 0 and 1
    float floatVolume = volume / 100.0f;
    audioOutput->setVolume(floatVolume);
}

void MainWindow::toggleMute() {
    isMuted = !isMuted;
    audioOutput->setMuted(isMuted);
    if (isMuted) {
        muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    } else {
        muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}


// --- All other functions (save, load, export, etc.) are unchanged ---
void MainWindow::closeEvent(QCloseEvent *event) { if (maybeSave()) { event->accept(); } else { event->ignore(); } }
void MainWindow::newCase() { if (maybeSave()) { clearAllFields(); currentCaseFile.clear(); setWindowModified(false); updateWindowTitle(); statusBar()->showMessage("New case created.", 3000); } }
void MainWindow::openCase() { if (maybeSave()) { QString filePath = QFileDialog::getOpenFileName(this, "Open Case File", "", "Data Organizator (*.osintcase);;All Files (*)"); if (!filePath.isEmpty()) { if (readCaseData(filePath)) { currentCaseFile = filePath; setWindowModified(false); updateWindowTitle(); statusBar()->showMessage("Case loaded successfully: " + currentCaseFile, 3000); } else { QMessageBox::critical(this, "Error", "Failed to load the case file. The file may be corrupt or not a valid case file."); statusBar()->showMessage("Error loading case file.", 3000); } } } }
bool MainWindow::saveCase() { if (currentCaseFile.isEmpty()) { return saveCaseAs(); } else { if (writeCaseData(currentCaseFile)) { setWindowModified(false); statusBar()->showMessage("Case saved successfully: " + currentCaseFile, 3000); return true; } statusBar()->showMessage("Error saving case file.", 3000); return false; } }
bool MainWindow::saveCaseAs() { QString filePath = QFileDialog::getSaveFileName(this, "Save Case File As", "", "Data Organizator (*.dataorganization);;All Files (*)"); if (filePath.isEmpty()) { return false; } if (writeCaseData(filePath)) { currentCaseFile = filePath; setWindowModified(false); updateWindowTitle(); return true; } return false; }
bool MainWindow::writeCaseData(const QString &filePath) { QJsonObject caseObject; caseObject["caseName"] = caseNameEdit->text(); caseObject["subjectTarget"] = subjectTargetEdit->text(); caseObject["notes"] = notesTextEdit->toPlainText(); auto tableToJson = [](QTableWidget *table) { QJsonArray tableArray; for (int row = 0; row < table->rowCount(); ++row) { QJsonArray rowArray; for (int col = 0; col < table->columnCount(); ++col) { rowArray.append(table->item(row, col) ? table->item(row, col)->text() : ""); } tableArray.append(rowArray); } return tableArray; }; caseObject["entities"] = tableToJson(entitiesTable); caseObject["events"] = tableToJson(eventsTable); caseObject["resources"] = tableToJson(resourcesTable); QFile saveFile(filePath); if (!saveFile.open(QIODevice::WriteOnly)) { qWarning("Couldn't open save file."); QMessageBox::critical(this, "Error", "Could not open file for writing: " + filePath); return false; } saveFile.write(QJsonDocument(caseObject).toJson(QJsonDocument::Indented)); return true; }
bool MainWindow::readCaseData(const QString &filePath) { QFile loadFile(filePath); if (!loadFile.open(QIODevice::ReadOnly)) { qWarning("Couldn't open load file."); return false; } QByteArray saveData = loadFile.readAll(); QJsonDocument loadDoc(QJsonDocument::fromJson(saveData)); if (loadDoc.isNull() || !loadDoc.isObject()) { qWarning("Invalid JSON in case file."); return false; } QJsonObject json = loadDoc.object(); clearAllFields(); caseNameEdit->setText(json["caseName"].toString()); subjectTargetEdit->setText(json["subjectTarget"].toString()); notesTextEdit->setText(json["notes"].toString()); auto jsonToTable = [](QTableWidget *table, const QJsonValue &jsonValue) { if (!jsonValue.isArray()) return; QJsonArray jsonArray = jsonValue.toArray(); table->setRowCount(0); for (int i = 0; i < jsonArray.size(); ++i) { QJsonArray rowArray = jsonArray[i].toArray(); table->insertRow(i); for (int j = 0; j < rowArray.size(); ++j) { table->setItem(i, j, new QTableWidgetItem(rowArray[j].toString())); } } }; jsonToTable(entitiesTable, json["entities"]); jsonToTable(eventsTable, json["events"]); jsonToTable(resourcesTable, json["resources"]); return true; }
void MainWindow::exportTableToCsv() { QObject *senderObj = sender(); QTableWidget *sourceTable = nullptr; QString defaultFileName = caseNameEdit->text().isEmpty() ? "export.csv" : caseNameEdit->text().replace(" ", "_") + "_export.csv"; if (senderObj == exportEntitiesAction) { sourceTable = entitiesTable; defaultFileName = caseNameEdit->text().replace(" ", "_") + "_entities.csv"; } else if (senderObj == exportEventsAction) { sourceTable = eventsTable; defaultFileName = caseNameEdit->text().replace(" ", "_") + "_events.csv"; } else if (senderObj == exportResourcesAction) { sourceTable = resourcesTable; defaultFileName = caseNameEdit->text().replace(" ", "_") + "_resources.csv"; } if (!sourceTable) return; QString filePath = QFileDialog::getSaveFileName(this, "Export to CSV", defaultFileName, "CSV Files (*.csv);;All Files (*)"); if (filePath.isEmpty()) return; QFile file(filePath); if (file.open(QIODevice::WriteOnly | QIODevice::Text)) { QTextStream stream(&file); QStringList headers; for (int i = 0; i < sourceTable->columnCount(); ++i) { headers << "\"" + sourceTable->horizontalHeaderItem(i)->text() + "\""; } stream << headers.join(',') << "\n"; for (int row = 0; row < sourceTable->rowCount(); ++row) { QStringList rowData; for (int col = 0; col < sourceTable->columnCount(); ++col) { QString cellText = sourceTable->item(row, col) ? sourceTable->item(row, col)->text() : ""; cellText.replace("\"", "\"\""); rowData << "\"" + cellText + "\""; } stream << rowData.join(',') << "\n"; } file.close(); statusBar()->showMessage("Data exported successfully to " + filePath, 3000); } else { QMessageBox::critical(this, "Error", "Could not write to file: " + file.errorString()); statusBar()->showMessage("Export failed.", 3000); } }
bool MainWindow::maybeSave() { if (!isWindowModified()) { return true; } const QMessageBox::StandardButton ret = QMessageBox::warning(this, "Data Organizer", "The case has been modified.\nDo you want to save your changes?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel); switch (ret) { case QMessageBox::Save: return saveCase(); case QMessageBox::Cancel: return false; case QMessageBox::Discard: return true; default: break; } return true; }
void MainWindow::clearAllFields() { caseNameEdit->clear(); subjectTargetEdit->clear(); notesTextEdit->clear(); entitiesTable->setRowCount(0); eventsTable->setRowCount(0); resourcesTable->setRowCount(0); mediaPlayer->setSource(QUrl()); imageDisplayLabel->clear(); imageDisplayLabel->setText("Open a video or image file to begin"); mediaStack->setCurrentWidget(imageDisplayLabel); setMediaControlsEnabled(false); }
void MainWindow::setWindowModified(bool modified) { QMainWindow::setWindowModified(modified); }
void MainWindow::updateWindowTitle() { QString baseTitle = "Data Organizer"; QString casePart = currentCaseFile.isEmpty() ? "Untitled Case" : QFileInfo(currentCaseFile).fileName(); if (!caseNameEdit->text().isEmpty()) { casePart = caseNameEdit->text(); } setWindowTitle(casePart + "[*] - " + baseTitle); }
void MainWindow::showTableContextMenu(const QPoint &pos) { QTableWidget *table = qobject_cast<QTableWidget*>(sender()); if (!table || table->itemAt(pos) == nullptr) return; QMenu contextMenu; QAction *removeAction = contextMenu.addAction(style()->standardIcon(QStyle::SP_TrashIcon), "Remove Selected Row(s)"); connect(removeAction, &QAction::triggered, this, &MainWindow::removeSelectedTableRow); contextMenu.exec(table->viewport()->mapToGlobal(pos)); }
void MainWindow::removeSelectedTableRow() { QWidget* currentTab = theTabs->currentWidget(); QTableWidget* table = currentTab->findChild<QTableWidget*>(); if (!table) return; auto selectedItems = table->selectionModel()->selectedRows(); std::sort(selectedItems.begin(), selectedItems.end(), [](const QModelIndex& a, const QModelIndex& b) -> bool { return a.row() > b.row(); }); for(const QModelIndex &index : selectedItems) { table->removeRow(index.row()); } setWindowModified(true); }
void MainWindow::playPause(){ if (mediaPlayer->playbackState() == QMediaPlayer::PlayingState) mediaPlayer->pause(); else mediaPlayer->play(); }
void MainWindow::updatePlayPauseButton(QMediaPlayer::PlaybackState state){ if (state == QMediaPlayer::PlayingState) { playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause)); } else { playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay)); } }
void MainWindow::mediaPositionChanged(qint64 position){ if (!mediaPositionSlider->isSliderDown()) { mediaPositionSlider->setValue(position); } mediaTimeLabel->setText(formatTime(position) + " / " + formatTime(mediaPlayer->duration())); }
void MainWindow::mediaDurationChanged(qint64 duration){ mediaPositionSlider->setRange(0, duration); setMediaControlsEnabled(duration > 0); }
void MainWindow::setMediaPosition(int position){ mediaPlayer->setPosition(position); }
void MainWindow::addTimestampToNotes(){ notesTextEdit->insertPlainText(QString("[%1] ").arg(formatTime(mediaPlayer->position()))); notesTextEdit->setFocus(); }
void MainWindow::addEntityRow(){ int row = entitiesTable->rowCount(); entitiesTable->insertRow(row); entitiesTable->setItem(row, 0, new QTableWidgetItem(formatTime(mediaPlayer->position()))); entitiesTable->scrollToBottom(); entitiesTable->editItem(entitiesTable->item(row, 1)); }
void MainWindow::addEventRow(){ int row = eventsTable->rowCount(); eventsTable->insertRow(row); eventsTable->setItem(row, 0, new QTableWidgetItem(formatTime(mediaPlayer->position()))); eventsTable->scrollToBottom(); eventsTable->editItem(eventsTable->item(row, 2)); }
void MainWindow::addResourceRow(){ int row = resourcesTable->rowCount(); resourcesTable->insertRow(row); resourcesTable->setItem(row, 2, new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))); resourcesTable->scrollToBottom(); resourcesTable->editItem(resourcesTable->item(row, 0)); }
QString MainWindow::formatTime(qint64 timeMilliSeconds){ qint64 seconds = timeMilliSeconds / 1000; return QStringLiteral("%1:%2:%3").arg(seconds / 3600, 2, 10, QLatin1Char('0')).arg((seconds % 3600) / 60, 2, 10, QLatin1Char('0')).arg(seconds % 60, 2, 10, QLatin1Char('0')); }

