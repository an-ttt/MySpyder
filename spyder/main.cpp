#include "config/config_main.h"
#include <QApplication>
#include <QDebug>
#include <QIcon>

#include "plugins/ipythonconsole.h"
#include "app/mainwindow.h"


void registe_meta_type()
{
    QCoreApplication::setOrganizationName("Quan");
    QCoreApplication::setApplicationName("spyder");
    qRegisterMetaType<ColorBoolBool>();
    qRegisterMetaTypeStreamOperators<ColorBoolBool>("ColorBoolBool");

    qRegisterMetaType<SplitSettings>();
    qRegisterMetaTypeStreamOperators<SplitSettings>("SplitSettings");

    qRegisterMetaType<LayoutSettings>();
    qRegisterMetaTypeStreamOperators<LayoutSettings>("LayoutSettings");

    // This attibute must be set before creating the application.
    bool high_dpi_scaling = CONF_get("main", "high_dpi_scaling").toBool();
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, high_dpi_scaling);
}

void read_default_to_settings()
{
    QSettings settings;
    foreach (auto pair, DEFAULTS) {
        QString section = pair.first;
        auto dict = pair.second;
        foreach (QString key, dict.keys()) {
            QString tmp = section + '/' + key;
            QVariant val = dict[key];
            settings.setValue(tmp, val);
        }
    }
}

int main(int argc, char *argv[])
{
    registe_meta_type();
    QApplication a(argc, argv);
    QSettings settings;
    QString first_run = "first_run";
    if (settings.value(first_run, true).toBool()) {
        read_default_to_settings();
        settings.setValue(first_run, false);
    }
    initialize();

    MainWindow* win = new MainWindow;
    win->setup();
    win->show();
    win->post_visible_setup();

    //if main.console:
    QObject::connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)),
            win, SLOT(change_last_focused_widget(QWidget *, QWidget *)));

    return a.exec();
}
