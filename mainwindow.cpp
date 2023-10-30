#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worldtimewidget.h"

#include <QDebug>
#include <QScopedPointer>
#include <QTimer>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QMouseEvent>
#include <QWindow>
#include <QSizePolicy>
#include <QMediaPlayer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    new QGridLayout(ui->wdScollContent);

    connect(ui->ActionAddWorldTimeWidget,&QAction::triggered,this,&MainWindow::ActionAddWorldTimeWidgetTriggered);

    TrSyndTime = new QTimer(this);
    TrSyndTime->start(1000);

    PbAddButton = new QPushButton(this);
    PbAddButton->setMaximumSize(WorldTimeWidget::InfoItemSize);
    PbAddButton->setMinimumSize(WorldTimeWidget::InfoItemSize);
    PbAddButton->setText("+Add Item");
    connect(PbAddButton,SIGNAL(clicked()),this,SLOT(AddButton()));

    CreateTrayIcon();
    LoadJsonBackup();

    AddWidgetToLayout(PbAddButton);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::CreateTrayIcon()
{
    auto m_tray_icon = new QSystemTrayIcon(QIcon(":/new/prefix1/WorldTimeIcon.png"), this);

    connect( m_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(ShowAllWidgets(QSystemTrayIcon::ActivationReason)) );

    QAction *quit_action = new QAction( "Exit", m_tray_icon );
    connect( quit_action, SIGNAL(triggered()), this, SLOT(OnExitAction()) );

    QAction *hide_action = new QAction( "Show", m_tray_icon );
    connect( hide_action, SIGNAL(triggered()), this, SLOT(show()) );

    QMenu *tray_icon_menu = new QMenu;
    tray_icon_menu->addAction( hide_action );
    tray_icon_menu->addAction( quit_action );

    m_tray_icon->setContextMenu( tray_icon_menu );

    m_tray_icon->show();
}

void MainWindow::UpdateLayout()
{
    if(WidgetList.count()==0)return;

    int numberof_item_per_line = (this->width() / WidgetList.first()->width()) - 1;

    ui->wdScollContent->setProperty("numberof_item_per_line",numberof_item_per_line);
    ui->wdScollContent->setProperty("row",0);
    ui->wdScollContent->setProperty("col",0);

    for(auto item = WidgetList.begin(); item != WidgetList.end(); ++item)
    {
        QWidget* item_info = (*item)->InfoWidgetBox;
        AddWidgetToLayout(item_info);
    }
    AddWidgetToLayout(PbAddButton);
}

void MainWindow::LoadJsonBackup()
{
    QFile file("Backup");
    if(file.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array = doc.array();

        if(array.count() == 0)
            this->show();

        for(int i=0;i<array.count();++i)
            CreateNewWidget(array.at(i).toObject());
    }
    else
    {
        this->show();
    }
}

void MainWindow::SaveJsonBackup()
{
    QJsonArray backup_data;

    if(WidgetList.count() == 0) return;

    for(auto item = WidgetList.begin(); item != WidgetList.end(); ++item)
        backup_data.append((*item)->ToJson());

    QJsonDocument doc(backup_data);
    QFile file("Backup");
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        file.close();
    }
}

void MainWindow::CreateNewWidget(QJsonObject ojb)
{
    auto widget = new WorldTimeWidget();
    widget->FromJson(ojb);

    WidgetList.append(widget);

    UpdateLayout();
    connect(TrSyndTime,&QTimer::timeout,widget,&WorldTimeWidget::UpdateSyncDateTime);
    connect(widget,&WorldTimeWidget::RemoveRequest,this,&MainWindow::RemoveItemRequestHandel);
    connect(widget,&WorldTimeWidget::AlertRequest,this,&MainWindow::PlayAlertSound,Qt::DirectConnection);
    widget->show();
}

void MainWindow::AddWidgetToLayout(QWidget *widget)
{
    auto row = ui->wdScollContent->property("row").toInt();
    auto col = ui->wdScollContent->property("col").toInt();
    auto numberof_item_per_line = ui->wdScollContent->property("numberof_item_per_line").toInt();

    static_cast<QGridLayout*>(ui->wdScollContent->layout())->addWidget(widget,row,col);
    if(col >= numberof_item_per_line)
    {
        ui->wdScollContent->setProperty("row" , ++row);
        ui->wdScollContent->setProperty("col" , 0);
    }
    else
        ui->wdScollContent->setProperty("col" , ++col);

    widget->show();
    widget->raise();
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    UpdateLayout();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    this->hide();
    e->ignore();
}

void MainWindow::ActionAddWorldTimeWidgetTriggered()
{
    CreateNewWidget();
}

void MainWindow::OnExitAction()
{
    SaveJsonBackup();
    WidgetList.clear();
    qApp->quit();
}

void MainWindow::RemoveItemRequestHandel()
{
    WorldTimeWidget* item = static_cast<WorldTimeWidget*>(sender());
    if(item)
    {
        int index = WidgetList.indexOf(item);
        if(index>-1)
        {
            auto item = WidgetList.takeAt(index);
            item->deleteLater();
        }
    }
}

void MainWindow::ShowAllWidgets(QSystemTrayIcon::ActivationReason reson)
{
    switch(reson)
    {
    case QSystemTrayIcon::DoubleClick:
        this->show();
        this->activateWindow();

    case QSystemTrayIcon::Trigger:
        for(auto item = WidgetList.begin(); item != WidgetList.end(); ++item)
            (*item)->activateWindow();
    }
}

void MainWindow::on_ActionExit_triggered()
{
    this->OnExitAction();
}

void MainWindow::AddButton()
{
    CreateNewWidget();
}

void MainWindow::PlayAlertSound()
{
    sender()->setProperty("AlertSoundIsRun",true);

    QMediaPlayer player;
    player.setMedia(QUrl::fromLocalFile(":/new/prefix1/alert_sound.wav"));
    player.setVolume(50);
    player.play();
}
