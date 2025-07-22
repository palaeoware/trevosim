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
    if (argc < 2)
    {

#if defined( _WIN32 )
        // hide console window
        ::ShowWindow( ::GetConsoleWindow(), SW_HIDE );
#endif

        QPixmap splashPixmap(":/palaeoware_logo_square.png");
        QSplashScreen *splash = new QSplashScreen(splashPixmap, Qt::WindowStaysOnTopHint);
        splash->setAttribute(Qt::WA_DeleteOnClose, true);
        splash->show();
        splash->showMessage("<font><b>" + QString(PRODUCTNAME) + " - " + QString(PRODUCTTAG) + "</b></font>", Qt::AlignHCenter, Qt::white);
        QApplication::processEvents();
        QTimer::singleShot(5000, splash, SLOT(close()));

        MainWindow *w = new MainWindow;
        w->show();

        int e =  a.exec();

        delete w; //needed to execute deconstructor
        exit(e); //needed to exit the hidden console
        return e;
    }
    // Start from a console...
    else
    {
        //Sort out command line option
        QCommandLineParser *parser = new QCommandLineParser();
        QString message("TREvoSim is an individual-based evolutionary model. You are using the command line option. See documentation and associated publications for description of software. This is ");
        QString version = QString(PRODUCTNAME) + " v" + QString("%1.%2.%3").arg(MAJORVERSION).arg(MINORVERSION).arg(PATCHVERSION);
        message.append(version);
        message.append(QString(", compiled on ") + __DATE__ + QString("."));

        parser->setApplicationDescription(message);
        parser->addHelpOption();
        parser->addVersionOption();
        parser->setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

        QCommandLineOption opt_o(QStringList() << "o" << "open",
                                 QCoreApplication::translate("main", "Settings file to load on start."),
                                 QCoreApplication::translate("main", "Path to file (string)."));
        parser->addOption(opt_o);

        QCommandLineOption opt_b(QStringList() << "b" << "batch",
                                 QCoreApplication::translate("main", "Run in batch mode."),
                                 QCoreApplication::translate("main", "An integer representing the required number of replicates."));
        parser->addOption(opt_b);

        QCommandLineOption opt_s(QStringList() << "s" << "skipInput",
                                 QCoreApplication::translate("main", "Skip requests for user input."),
                                 QCoreApplication::translate("main", " An integer of value between 1 and 1000000 that dictates the number of attempts to reach requested replicate number in batch mode."));
        parser->addOption(opt_s);

        parser->process(a);

        MainWindow *w = new MainWindow;

        QHash<QString, QString> parsedOptions;

        if (parser->isSet(opt_b)) parsedOptions.insert("batchReplicates", parser->value(opt_b));
        if (parser->isSet(opt_s)) parsedOptions.insert("skipInput", parser->value(opt_s));
        if (parser->isSet(opt_o))
        {
            QString fileFromCommandLine = QString();
            fileFromCommandLine = parser->value(opt_o);
            if (!fileFromCommandLine.isNull()) parsedOptions.insert("fileFromCommandLine", parser->value(opt_o));
        }

        w->runFromCommandLine(parsedOptions);

        int e =  a.exec();

        delete w; //needed to execute deconstructor
        exit(e); //needed to exit the hidden console
        return e;
    }
}
