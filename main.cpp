#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>

class EventControll:public QObject
{
public:
    EventControll(MainWindow *parent){this->parent = parent;}
private:
    MainWindow* parent;
    bool eventFilter(QObject *watched, QEvent *event)override
    {
        auto e = static_cast<QWindowStateChangeEvent *>(event);
        if(watched->objectName() == "WorldTimeWidget" && e->oldState().testFlag(Qt::WindowMinimized))
            return true;

        return QObject::eventFilter(watched,event);
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    w.show();
//    a.installEventFilter(new EventControll(&w));
    return a.exec();
}
