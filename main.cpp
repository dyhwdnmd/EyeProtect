#include "QtWidgetsApplication1.h"
#include <QtWidgets/QApplication>
#include <QStyleFactory>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application information
    app.setApplicationName("EyeCare");
    app.setApplicationDisplayName("EyeCare");
    app.setOrganizationName("EyeCare");
    app.setOrganizationDomain("eyecare.app");
    
    // Set fusion style for a modern look
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Create and show the main window
    QtWidgetsApplication1 window;
    window.show();
    
    return app.exec();
}
