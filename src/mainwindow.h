#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
 #include <QSignalMapper>
#include "qdebugstream.h"
#include "stdio.h"
#include "iostream"
#include "MeshViewerWidget.hh"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void toggleMeshVisible(int index);
private slots:
    void on_axisButton_clicked();
    void setMeshvisible(int index);

    void on_openButton_clicked();

    void on_resetPositionButton_clicked();

    void on_computeButton_clicked();


    void on_clearButton_clicked();

    void on_alignButton_clicked();

private:
    Ui::MainWindow *ui;
    QSignalMapper *signalMapper;

    int nViewers;
    MeshViewerWidget ** leftWidgets;
    MeshViewerWidget ** rightWidgets;
    bool computedMeshes;
    QDebugStream *qout;
    QDebugStream *qlog;
    QDebugStream *qerr;


    static const int MAX_VIEWERS=10;


    void updateMenu();
};

#endif // MAINWINDOW_H
