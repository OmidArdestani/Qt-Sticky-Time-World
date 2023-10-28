#ifndef WORLDTIMEWIDGET_H
#define WORLDTIMEWIDGET_H

#include <QTimeZone>
#include <QWidget>
#include <QJsonObject>


namespace Ui {
class WorldTimeWidget;
class InfoWidget;
}

class WorldTimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WorldTimeWidget();
    ~WorldTimeWidget();
    QWidget* InfoWidgetBox;

    void SetTimeZone(QTimeZone time_zone);
    void SetCountryName(QString country, QString city);
    void SetMarketAlert(bool alert);
    void SetPannelColor(QColor color);
    QColor PannelColor();
    static const QSize ItemSize;
    static const QSize InfoItemSize;

    void FromJson(QJsonObject obj);
    QJsonObject ToJson();

private:
    Ui::WorldTimeWidget *ui;
    QPoint MousePressedPos;
    QPoint WindowPos;
    struct PImpl;
    PImpl* Impl;

private:
    void mouseMoveEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);

public slots:
    void UpdateSyncDateTime();
    void SetStayOnTop(bool value);

signals:
    void RemoveRequest();
};

class InfoWidget : public QWidget
{
    Q_OBJECT
    friend class WorldTimeWidget;

private:
    void BuildTimeZoneList();

private slots:
    void UpdateFields();
    void OpenChoseColor();

private:
    Ui::InfoWidget *ui;
    InfoWidget(WorldTimeWidget *parent = nullptr);
    WorldTimeWidget *Parent = nullptr;
};
#endif // WORLDTIMEWIDGET_H
