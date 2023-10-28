#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QJsonObject>
#include <memory>
#include <QSystemTrayIcon>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class WorldTimeWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QList<WorldTimeWidget*> WidgetList;
    QSize ItemSize;
    QTimer *TrSyndTime = nullptr;
    QPushButton *PbAddButton = nullptr;

private:
    void CreateTrayIcon();
    void UpdateLayout();
    void LoadJsonBackup();
    void SaveJsonBackup();
    void CreateNewWidget(QJsonObject ojb = QJsonObject());
    void AddWidgetToLayout(QWidget* widget);

protected:
    void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*e);

private slots:
    void ActionAddWorldTimeWidgetTriggered();
    void OnExitAction();
    void RemoveItemRequestHandel();
    void ShowAllWidgets(QSystemTrayIcon::ActivationReason reson);
    void on_ActionExit_triggered();
    void AddButton();
};

#endif // MAINWINDOW_H
