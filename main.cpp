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
    QApplication a(argc, argv);

    //Close on last window close
    a.setQuitOnLastWindowClosed(true);

    //Style program with our dark style
    a.setStyle(new DarkStyleTheme);

    QPixmap splashPixmap(":/palaeoware_logo_square.png");
    QSplashScreen splash(splashPixmap,Qt::WindowStaysOnTopHint);
    splash.show();
    splash.showMessage("<font><b>" + QString(PRODUCTNAME) + " - " + QString(PRODUCTTAG) + "</b></font>",Qt::AlignHCenter,Qt::white);
    a.processEvents();

    MainWindow w;
    w.show();

    return a.exec();
}
