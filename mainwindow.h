#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <back/headers/clustering.h>


#include <QTimer>
#include <QTime>
#include <QtWidgets/QTreeWidgetItem>
#include <back/headers/clusterWorker.h>

class TreeWidgetItem : public QTreeWidgetItem {
public:
    TreeWidgetItem() {}

    TreeWidgetItem(QTreeWidget *parent) : QTreeWidgetItem(parent) {}

private:
    bool operator<(const QTreeWidgetItem &other) const {
        int column = treeWidget()->sortColumn();
        if (column == 1) {
            column = 6;
        }
        if (column == 4) {
            column = 5;
        }
        bool isConvertAbleToInt;
        text(column).toInt(&isConvertAbleToInt);
        if (isConvertAbleToInt) {
            return text(column).toInt() < other.text(column).toInt();
        }

        return text(column).toLower() < other.text(column).toLower();
    }
};


namespace Ui {
    class MainWindow;
}

class main_window : public QMainWindow {
Q_OBJECT

public:
    explicit main_window(QWidget *parent = 0);

    ~main_window();

    QTimer *tmr;
private slots:

    //void select_directory();
    //void scan_directory(QString const& dir);
    void show_about_dialog();

    void onClusterEnded(int);

    void pushButton_runstop_clicked();

    void pushButton_selectall_clicked();

    void pushButton_delete_clicked();

    void change_selected_file(QTreeWidgetItem *, QTreeWidgetItem *);

    void change_selected_file(QTreeWidgetItem *, int);

    void pushButton_openpath_clicked();

    void pushButton_openfile_clicked();

    void startWorkInScanThread();

    void updateTime();

    void openCluster(QTreeWidgetItem *);
    //void startTimer();
    //void stopTimer();

    void buttons_start();

    //void buttons_selected();
    void buttons_running();

    void buttons_paused();

    void buttons_ended();

    void buttons_selected_cluster();

    void buttons_selected_files();

private:
    //void timerEvent(QTimerEvent *);
    bool running_scanning_time = false;
    //QDateTime startTime;

    std::unique_ptr<Ui::MainWindow> ui;

    QString size_human(float num);

    QScopedPointer<ClusterWorker> scoped_pointer_to_ClusterObject;
    QThread *thread;
    ClusterWorker *clusterWorker;

    void scan(QString dir);

signals:

    void startWork();

    void stopWork();
};

#endif // MAINWINDOW_H
