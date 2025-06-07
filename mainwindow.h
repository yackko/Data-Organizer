#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QShortcut>

// Forward declarations for UI elements and system classes
class QSplitter;
class QTextEdit;
class QLineEdit;
class QTableWidget;
class QVideoWidget;
class QPushButton;
class QTabWidget;
class QSlider;
class QLabel;
class QMenu;
class QAction;
class QStackedWidget;
class QAudioOutput;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // File Operations
    void newCase();
    bool saveCase();
    bool saveCaseAs();
    void openCase();
    void updateWindowTitle();

    // Media Operations
    void openMediaFile(); // Renamed from openVideoFile
    void playPause();
    void mediaPositionChanged(qint64 position);
    void mediaDurationChanged(qint64 duration);
    void setMediaPosition(int position);
    void updatePlayPauseButton(QMediaPlayer::PlaybackState state);

    // Audio Controls
    void setVolume(int volume);
    void toggleMute();

    // Data Entry & Management
    void addTimestampToNotes();
    void addEntityRow();
    void addEventRow();
    void addResourceRow();
    void showTableContextMenu(const QPoint &pos);
    void removeSelectedTableRow();

    // Export Operations
    void exportTableToCsv();

private:
    void setupUi();
    void setupMenuBar();
    void setupConnections();
    void setupShortcuts();

    // UI Helper Functions
    QString formatTime(qint64 timeMilliSeconds);
    void setWindowModified(bool modified);
    bool maybeSave();
    void clearAllFields();
    void setMediaControlsEnabled(bool enabled);

    // Data Persistence Functions
    bool writeCaseData(const QString &filePath);
    bool readCaseData(const QString &filePath);

    // --- Main Layout ---
    QSplitter *mainSplitter;

    // --- Left Panel: Media Player ---
    QWidget *mediaPanel;
    QStackedWidget *mediaStack; // To switch between video and image
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput; // <-- FIX for no sound
    QVideoWidget *videoWidget;
    QLabel *imageDisplayLabel; // For viewing images

    QPushButton *openMediaButton;
    QPushButton *playPauseButton;
    QSlider *mediaPositionSlider;
    QLabel *mediaTimeLabel;
    QPushButton *muteButton;
    QSlider *volumeSlider;

    // --- Right Panel: Panel ---
    QWidget *thePanel;
    QLineEdit *caseNameEdit;
    QLineEdit *subjectTargetEdit;
    QTabWidget *theTabs;

    // -- Tab Widgets
    QWidget *notesTab;
    QTextEdit *notesTextEdit;
    QPushButton *addTimestampButton;

    QWidget *entitiesTab;
    QTableWidget *entitiesTable;
    QPushButton *addEntityButton;

    QWidget *eventsTab;
    QTableWidget *eventsTable;
    QPushButton *addEventButton;

    QWidget *resourcesTab;
    QTableWidget *resourcesTable;
    QPushButton *addResourceButton;

    // --- Menu Bar and Actions ---
    QMenu *fileMenu;
    QMenu *exportMenu;
    QAction *newCaseAction;
    QAction *openCaseAction;
    QAction *saveCaseAction;
    QAction *saveCaseAsAction;
    QAction *exitAction;
    QAction *exportEntitiesAction;
    QAction *exportEventsAction;
    QAction *exportResourcesAction;

    // --- State and Data ---
    QString currentCaseFile;
    bool isMuted = false;
};

#endif // MAINWINDOW_H
