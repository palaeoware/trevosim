#include "darkstyletheme.h"

DarkStyleTheme::DarkStyleTheme():
    DarkStyleTheme(styleBase())
{

}

DarkStyleTheme::DarkStyleTheme(QStyle *style):
    QProxyStyle(style)
{

}

QStyle *DarkStyleTheme::styleBase(QStyle *style) const
{
    static QStyle *base = !style ? QStyleFactory::create(QStringLiteral("Fusion")) : style;
    return base;
}

QStyle *DarkStyleTheme::baseStyle() const
{
    return styleBase();
}

void DarkStyleTheme::polish(QPalette &palette)
{
    // modify palette to dark
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
    palette.setColor(QPalette::Base, QColor(42, 42, 42));
    palette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, QColor(53, 53, 53));
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    palette.setColor(QPalette::Dark, QColor(35, 35, 35));
    palette.setColor(QPalette::Shadow, QColor(20, 20, 20));
    palette.setColor(QPalette::Button, QColor(80, 80, 80));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(0, 100, 0)); //Green
    palette.setColor(QPalette::Highlight, QColor(0, 100, 0)); //Green
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
}

void DarkStyleTheme::polish(QApplication *app)
{
    if (!app) return;

    //ARTS - Increase font size for better reading,
    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(defaultFont.pointSize() + 1);
    app->setFont(defaultFont);

    // Load CSS Stylesheet from resources
    QFile darkstyle_qss(QStringLiteral(":/darkstyle/stylesheet_dark.qss"));
    if (darkstyle_qss.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // Set stylesheet
        QString stylesheet_qss = QString::fromLatin1(darkstyle_qss.readAll());
        app->setStyleSheet(stylesheet_qss);
        darkstyle_qss.close();
    }
}
