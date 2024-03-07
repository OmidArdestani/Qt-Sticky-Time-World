#include "worldtimewidget.h"
#include "ui_worldtimewidget.h"
#include "ui_infowidget.h"

#include <QMouseEvent>
#include <QPushButton>
#include <QTime>
#include <QLocale>
#include <QDebug>
#include <QColorDialog>

struct WorldTimeWidget::PImpl
{
    QString Country = "";
    QString City = "";
    bool MarketAlert=false;
    QTimeZone TimeZone;
    QColor PannelColor = Qt::white;
    bool StayOnTop = false;
};

//------------------------------------------------
//------------------------------------------------
//------------------------------------------------

const QSize WorldTimeWidget::ItemSize = QSize(230,222);
const QSize WorldTimeWidget::InfoItemSize = QSize(210,133);

WorldTimeWidget::WorldTimeWidget() :
    QWidget(nullptr)
    ,ui(new Ui::WorldTimeWidget)
    ,Impl(new PImpl)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->resize(ItemSize);

    InfoWidgetBox = new InfoWidget(this);

    connect(ui->cbOpenMarketAlert,&QCheckBox::toggled,[=](bool state){Impl->MarketAlert = state;});

    UpdateSyncDateTime();

    QSizePolicy sp_retain = this->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(false);
    this->setSizePolicy(sp_retain);

    ui->lblMarketTimeRange->hide();
    ui->cbOpenMarketAlert->hide();
    this->setMaximumHeight(this->height() - 70);// 54px is for hiding label and checkbox and spacing.
}

WorldTimeWidget::~WorldTimeWidget()
{
    delete ui;
    delete Impl;
    delete InfoWidgetBox;
}

QColor WorldTimeWidget::PannelColor()
{
    return Impl->PannelColor;
}

void WorldTimeWidget::SetTimeZone(QTimeZone time_zone)
{
    Impl->TimeZone = time_zone;

    auto current_date_time = QDateTime::currentDateTime().toLocalTime().time();
    auto date_time = QDateTime::currentDateTime().toTimeZone(Impl->TimeZone).time();


    int minuts = current_date_time.secsTo(date_time) / 60;
    int h_part = minuts/60;
    double minut_part = ((double(minuts)/60.0) - h_part)*60;
    QString time_offset_str = QString("%1:%2").arg(QString::number(h_part),QString::number(abs(minut_part)));

    ui->lblTimeOffset->setText(time_offset_str);
}

void WorldTimeWidget::SetCountryName(QString country,QString city)
{
    Impl->Country = country;
    Impl->City = city;
    ui->lblCountry->setText(country);
    ui->lblCity->setText(city);
}

void WorldTimeWidget::SetMarketAlert(bool alert)
{
    Impl->MarketAlert = alert;
    ui->cbOpenMarketAlert->setChecked(alert);
}

void WorldTimeWidget::SetPannelColor(QColor color)
{
    Impl->PannelColor = color;
    this->setStyleSheet("background-color:" + color.name()+";");
    // set text color
    int avg_color_value = (Impl->PannelColor.red()+Impl->PannelColor.green()+Impl->PannelColor.blue()) / 3.0;
    if(avg_color_value <= 128)
    {
        this->setStyleSheet(this->styleSheet()+"color:white;");
    }
}

void WorldTimeWidget::FromJson(QJsonObject obj)
{
    if(obj.isEmpty())return;

    Impl->Country = obj.value("Country").toString();
    Impl->City = obj.value("City").toString();
    Impl->MarketAlert = obj.value("MarketAlert").toBool();
    Impl->TimeZone = QTimeZone(obj.value("TimeZone").toString().toUtf8());
    Impl->PannelColor = QColor(obj.value("PannelColor").toString());

    this->move(obj.value("rect-x").toInt(), obj.value("rect-y").toInt());
    this->setVisible(obj.value("Visibility").toBool());
    this->setStyleSheet("background-color:" + Impl->PannelColor.name()+";");
    this->SetStayOnTop(obj.value("StayOnTop").toBool());
    // set text color
    int avg_color_value = (Impl->PannelColor.red()+Impl->PannelColor.green()+Impl->PannelColor.blue()) / 3.0;
    if(avg_color_value <= 128)
    {
        this->setStyleSheet(this->styleSheet()+"color:white;");
    }


    auto widget = static_cast<InfoWidget*>(InfoWidgetBox);
    if(widget)
    {
        auto pb_style = "border:1px solid "+Impl->PannelColor.name()+"; border-radius:12px;";
        widget->ui->pbColorPannel->setStyleSheet(pb_style + "background-color:" + Impl->PannelColor.name());
        widget->ui->cbVisible->setChecked(obj.value("Visibility").toBool());
        widget->ui->cbTimeZones->setCurrentText(Impl->TimeZone.id());
    }
}

QJsonObject WorldTimeWidget::ToJson()
{
    QJsonObject obj;
    obj.insert("Country",Impl->Country);
    obj.insert("City",Impl->City);
    obj.insert("MarketAlert",Impl->MarketAlert);
    obj.insert("TimeZone", QString(Impl->TimeZone.id()));
    obj.insert("PannelColor",Impl->PannelColor.name());
    obj.insert("Visibility",this->isVisible());
    obj.insert("StayOnTop",Impl->StayOnTop);
    obj.insert("rect-x",x());
    obj.insert("rect-y",y());
    return obj;
}

void WorldTimeWidget::mouseMoveEvent(QMouseEvent *e)
{
    auto diff_pos = mapToGlobal(e->pos()) - MousePressedPos;
    this->move(WindowPos + diff_pos);
}

void WorldTimeWidget::mousePressEvent(QMouseEvent *e)
{
    MousePressedPos = mapToGlobal(e->pos());
    WindowPos = this->pos();
}

void WorldTimeWidget::UpdateSyncDateTime()
{
    auto current_date_time = QDateTime::currentDateTimeUtc();
    auto date_time = current_date_time.toTimeZone(Impl->TimeZone);
    ui->lblCurrentDate->setText(date_time.toString("dddd dd-MM-yyyy"));
    ui->lblCurrentTime->setText(date_time.toString("HH:mm:ss"));

    if(Impl->MarketAlert)
    {
        if(date_time.time().hour() > 9 && date_time.time().hour() < 17)
        {
            ui->widget->layout()->setContentsMargins(5,10,5,5);
            ui->widget->setStyleSheet("#widget{	border:5px solid "
                                      "qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.107955 rgba(255, 255, 255, 0), stop:0.795455 rgba(255, 0, 0, 255), stop:1 rgba(255, 0, 0, 214));}");
            emit AlertRequest();
        }
        else
        {
            ui->widget->setStyleSheet("");
            ui->widget->layout()->setContentsMargins(0,10,0,0);
        }
    }
    else
    {
        ui->widget->setStyleSheet("");
        ui->widget->layout()->setContentsMargins(0,10,0,0);
    }
}

void WorldTimeWidget::SetStayOnTop(bool value)
{
    auto visible_state = this->isVisible();
    Impl->StayOnTop = value;

    if(Impl->StayOnTop)
        this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    else
        this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    this->setVisible(visible_state);
}


//------------------------------------------------
//------------------------------------------------
//------------------------------------------------

InfoWidget::InfoWidget(WorldTimeWidget *parent):
    QWidget(nullptr)
  ,ui(new Ui::InfoWidget)
  ,Parent(parent)
{
    ui->setupUi(this);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    ui->cbVisible->setChecked(true);

    BuildTimeZoneList();

    connect(ui->pbColorPannel,&QPushButton::clicked,this,&InfoWidget::OpenChoseColor);
    connect(ui->cbVisible,&QCheckBox::toggled,Parent,&WorldTimeWidget::setVisible);
    connect(ui->cbStayOnTop,&QCheckBox::toggled,Parent,&WorldTimeWidget::SetStayOnTop);
    connect(ui->pbRemove,&QPushButton::clicked,Parent,&WorldTimeWidget::RemoveRequest);
}

void InfoWidget::BuildTimeZoneList()
{
    // Fill in combo box.
    QList<QByteArray> ids = QTimeZone::availableTimeZoneIds();
    ui->cbTimeZones->clear();

    foreach (QByteArray id, ids)
    {
        ui->cbTimeZones->addItem(id);
    }

    // Connect combo box to slot to update fields.
    connect(ui->cbTimeZones, SIGNAL(currentIndexChanged(int)),this,SLOT(UpdateFields()));

    ui->cbTimeZones->setCurrentIndex(0);
    UpdateFields();
}

void InfoWidget::UpdateFields()
{
    QByteArray id = ui->cbTimeZones->currentText().toLatin1();
    QTimeZone zone = QTimeZone(id);

    // Fill in fields for current time zone.
    if (zone.isValid())
    {
        QString regin_state_name = zone.id().split('/').last();
        Parent->SetTimeZone(zone);
        Parent->SetCountryName(QLocale::countryToString(zone.country()),regin_state_name);
    }
}

void InfoWidget::OpenChoseColor()
{
    auto color = QColorDialog::getColor(Parent->PannelColor());

    if(!color.isValid())return;

    ui->pbColorPannel->setStyleSheet("background-color:" + color.name());
    Parent->SetPannelColor(color);
}
