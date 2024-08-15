#include <QApplication>
#include <QSplashScreen>
#include <QString>
#include <QStyle>
#include <QTimer>
#include <QCommandLineParser>

#include "mainwindow.h"
#include "version.h"
#include "darkstyletheme.h"

#ifdef _WIN32
#include <iostream>
#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);
    //Style program with our dark style
    a.setStyle(new DarkStyleTheme);

    QString version = QString("%1.%2.%3").arg(MAJORVERSION).arg(MINORVERSION).arg(PATCHVERSION);
    QCoreApplication::setApplicationVersion(version);

    // Start GUI version...
    if (argc == 1) {
        QPixmap splashPixmap(":/palaeoware_logo_square.png");
        QSplashScreen *splash = new QSplashScreen(splashPixmap, Qt::WindowStaysOnTopHint);
        splash->setAttribute(Qt::WA_DeleteOnClose, true);
        splash->show();
        splash->showMessage("<font><b>" + QString(PRODUCTNAME) + " - " + QString(PRODUCTTAG) + "</b></font>", Qt::AlignHCenter, Qt::white);
        QApplication::processEvents();
        QTimer::singleShot(5000, splash, SLOT(close()));

        MainWindow w;
        w.show();
        return a.exec();
    }
    // Start from a console...
    else {
        #ifdef _WIN32
        if (AttachConsole(ATTACH_PARENT_PROCESS)) {
            freopen("CON", "w", stdout);
            freopen("CON", "w", stderr);
            std::cout << std::endl;
        }
        #endif

        //Sort out command line option
        QCommandLineParser *parser = new QCommandLineParser();
        parser->setApplicationDescription("REvoSim is an individual-based evolutionary model. You are using the command line option. See documentation or Garwood et al. (2019) Palaeontology for description of software.");
        parser->addHelpOption();
        parser->addVersionOption();
        parser->setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

        QCommandLineOption opt_o(QStringList() << "o" << "open",
                                 QCoreApplication::translate("main", "Settings file to load on start."),
                                 QCoreApplication::translate("main", "Path to file (string)"));
        parser->addOption(opt_o);

        parser->process(a);

        if (parser->isSet(opt_o))
        {
            QString fileFromCommandLine = QString();
            fileFromCommandLine = parser->value(opt_o);
            qInfo() << "Program launched from command line, and will try to open file " << fileFromCommandLine << ".";

            if (!fileFromCommandLine.isNull()) {
                MainWindow w;
                w.runFromCommandLine(fileFromCommandLine);
                w.show();
            }
        }

        return a.exec();
    }
}
