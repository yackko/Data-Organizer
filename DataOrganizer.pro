#-------------------------------------------------
# Project: Data  Organizer
#-------------------------------------------------

# Qt Modules
QT += core gui widgets multimedia multimediawidgets

# Configuration
CONFIG += c++17

# Target Executable Name (used for CFBundleExecutable)
TARGET = DataOrganizer

# --- macOS Specific Settings ---

# The application version (used for CFBundleShortVersionString)
VERSION = 1.0.0

# The bundle identifier. A reverse-DNS format is standard.
# This is a very important field for macOS.
QMAKE_BUNDLE_IDENTIFIER = com.yourcompany.dataorganizer

# Path to the application icon file (.icns format for macOS)
# Create an icon.icns file and place it in your project directory.
ICON = macos_icon.icns

# --- End macOS Specific Settings ---

# Project Files
SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

TEMPLATE = app
RESOURCES += resources.qrc
