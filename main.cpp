#include <QApplication>
#include <QSplashScreen>
#include <QString>
#include <QStyle>
#include <QDesktopWidget>

#include "mainwindow.h"
#include "version.h"
#include "darkstyletheme.h"

int main(int argc, char *argv[])
{
    //This has the app draw at HiDPI scaling on HiDPI displays, usually two pixels for every one logical pixel
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    //This has QPixmap images use the @2x images when available
    //See this bug for more details on how to get this right: https://bugreports.qt.io/browse/QTBUG-44486#comment-327410
#if (QT_VERSION >= 0x050600)
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication a(argc, argv);

    //Close on last window close
    a.setQuitOnLastWindowClosed(true);

    //Style program with our dark style
    a.setStyle(new DarkStyleTheme);

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
