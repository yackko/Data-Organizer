#include "mainwindow.h"

#include <QApplication>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // --- Professional Dark Theme for Data Analysts ---
    // This is the perfect place to define and apply a global stylesheet.
    QString darkStyleSheet = R"(
        /* Base settings for the entire application */
        QWidget {
            background-color: #1E1E1E; /* Dark charcoal background */
            color: #D4D4D4; /* Light grey text for readability */
            font-family: "Inter", "Segoe UI", "SF Pro Display", sans-serif;
            font-size: 14px;
            border: none;
        }

        /* Main Window and Group Boxes for structure */
        QMainWindow, QGroupBox {
            background-color: #191919; /* Slightly different dark shade for depth */
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #3A3A3A;
            border-radius: 8px;
            margin-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 8px;
            background-color: #191919;
            color: #007ACC; /* A highlight color for titles */
        }

        /* Input fields */
        QLineEdit, QTextEdit, QTableWidget {
            background-color: #2A2A2A;
            border: 1px solid #3A3A3A;
            border-radius: 4px;
            padding: 5px;
            color: #D4D4D4;
        }
        QLineEdit:focus, QTextEdit:focus {
            border: 1px solid #007ACC; /* Highlight on focus */
        }

        /* Buttons */
        QPushButton {
            background-color: #007ACC;
            color: #FFFFFF;
            font-weight: bold;
            border-radius: 4px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #005A9E;
        }
        QPushButton:pressed {
            background-color: #004C8C;
        }
        QPushButton:disabled {
            background-color: #3A3A3A;
            color: #888888;
        }

        /* Tab Bar */
        QTabWidget::pane {
            border-top: 1px solid #3A3A3A;
        }
        QTabBar::tab {
            background: #1E1E1E;
            border: 1px solid #3A3A3A;
            border-bottom: none;
            padding: 8px 16px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        QTabBar::tab:selected {
            background: #2A2A2A;
            border-color: #3A3A3A;
            color: #007ACC;
        }
        QTabBar::tab:!selected {
            color: #888888;
        }
        QTabBar::tab:hover {
            background: #2D2D2D;
        }

        /* Table Styling */
        QHeaderView::section {
            background-color: #2A2A2A;
            color: #007ACC;
            padding: 4px;
            border: 1px solid #3A3A3A;
            font-weight: bold;
        }
        QTableWidget {
            gridline-color: #3A3A3A;
        }

        /* Scrollbars */
        QScrollBar:vertical {
            border: none;
            background: #2A2A2A;
            width: 10px;
            margin: 0px 0px 0px 0px;
        }
        QScrollBar::handle:vertical {
            background: #555555;
            min-height: 20px;
            border-radius: 5px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QScrollBar:horizontal {
            border: none;
            background: #2A2A2A;
            height: 10px;
            margin: 0px 0px 0px 0px;
        }
        QScrollBar::handle:horizontal {
            background: #555555;
            min-width: 20px;
            border-radius: 5px;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
    )";

    // Apply the stylesheet to the entire application
    a.setStyleSheet(darkStyleSheet);

    MainWindow w;
    w.show();
    return a.exec();
}

