#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <back/headers/clustering.h>
#include <back/clustering.cpp>
//#include <QtWidgets/QTreeWidgetItem>

#include <QThread>
#include <QtCore/QTime>
#include <assert.h>
#include <QtGui/QDesktopServices>
#include <back/headers/clusterWorker.h>

//ClusterThread *clusterThread = new ClusterThread;
int64_t printedFilesSize = 0;
bool is_running = false;
std::string selected_file = "";
std::string selected_path = "";

main_window::main_window(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget_clusters->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget_clusters->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeWidget_clusters->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->treeWidget_clusters->header()->setSectionResizeMode(3, QHeaderView::Interactive);
    ui->treeWidget_clusters->header()->setSectionResizeMode(4, QHeaderView::Stretch);


    ui->treeWidget_clusters->header()->setMinimumSectionSize(0);
    ui->treeWidget_clusters->hideColumn(5);
    ui->treeWidget_clusters->hideColumn(6);
    //ui->treeWidget_clusters->header()->setStretchLastSection(false);

    ui->treeWidget_clusters->setUniformRowHeights(true);


    ui->treeWidget_cluster->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget_cluster->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeWidget_cluster->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    //ui->treeWidget_cluster->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->treeWidget_cluster->setUniformRowHeights(true);
    ui->treeWidget_cluster->hideColumn(3);


    ui->label_Clusters->setFixedHeight(27);

    ui->progressBar_scanning->setValue(0);
    ui->progressBar_printing->setValue(0);


    QList<int> list = ui->splitter->sizes();
    list.replace(0, this->width() / 0.6);
    list.replace(1, this->width() / 0.4);
    ui->splitter->setSizes(list);


    ui->label_directory->setText(QString::fromStdString("is not still selected"));


    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::startWorkInScanThread);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);

    connect(ui->pushButton_runstop, SIGNAL(clicked()), this, SLOT(pushButton_runstop_clicked()));
    connect(ui->treeWidget_cluster, SIGNAL(currentItemChanged(QTreeWidgetItem * , QTreeWidgetItem * )), this,
            SLOT(change_selected_file(QTreeWidgetItem * , QTreeWidgetItem * )));

    connect(ui->pushButton_openpath, SIGNAL(clicked()), this, SLOT(pushButton_openpath_clicked()));
    connect(ui->treeWidget_cluster, SIGNAL(itemActivated(QTreeWidgetItem * , int)), this,
            SLOT(pushButton_openpath_clicked()));

    connect(ui->pushButton_openfile, SIGNAL(clicked()), this, SLOT(pushButton_openfile_clicked()));
    //if (ui->treeWidget_clusters->currentItem())
    //    ui->label_directory->setText(ui->treeWidget_clusters->currentItem()->text(3));
    //scan_directory(QDir::homePath());


    //ui->pushButton_runstop->setDown(true);
    ui->pushButton_runstop->setDisabled(true);
    ui->pushButton_openfile->setDisabled(true);
    ui->pushButton_openpath->setDisabled(true);
    ui->pushButton_delete->setDisabled(true);
    ui->pushButton_selectall->setDisabled(true);

    connect(ui->treeWidget_clusters, SIGNAL(itemActivated(QTreeWidgetItem * , int)), this,
            SLOT(openCluster(QTreeWidgetItem * )));

//    connect(ui->treeWidget_clusters, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(openCluster(QTreeWidgetItem*)));
//    connect(ui->treeWidget_clusters, SIGNAL(itemEntered(QTreeWidgetItem*, int)), this, SLOT(openCluster(QTreeWidgetItem*)));

    //connect(ui->label_scanning_time, SIGNAL(signal_start()), SLOT(startTimer()));
    //connect(ui->label_scanning_time, SIGNAL(signal_stop()), SLOT(stopTimer()));
//    ui->setupUi(this);
//
    tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
    tmr->setInterval(40); // Задаем интервал таймера
    connect(tmr, SIGNAL(timeout()), this, SLOT(updateTime())); // Подключаем сигнал таймера к нашему слоту
    tmr->start(); // Запускаем таймер


    //connect(clusterThread, SIGNAL(clusterEnded(int)), SLOT(onClusterEnded(int)));
    // connect(clusterThread, SIGNAL(finished()), clusterThread, SLOT(deleteLater()));

}

bool used = false;

bool is_scanning = false;
QTime start_scanning_time;
QTime stop_scanning_time;

//bool is_running = false;
bool stop_run_signal = false;
int msec_run_time;
QTime start_run_time;

void main_window::startWorkInScanThread() {
    ui->treeWidget_clusters->clear();
    ui->treeWidget_cluster->clear();
    ui->treeWidget_clusters->setSortingEnabled(false);
    printedFilesSize = 0;

    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    //if (ui->treeWidget_clusters->currentItem())
    //    ui->label_directory->setText(ui->treeWidget_clusters->currentItem()->text(3));

    setWindowTitle(QString("Directory Content - %1").arg(dir));
    QDir d(dir);

    pushButton_runstop_clicked();
    ui->pushButton_runstop->setDisabled(false);
    ui->label_directory->setText(dir);

    clusters = clustering();

    is_scanning = true;
    start_scanning_time = QTime::currentTime();
    clusters.cluster_sizes(dir);
    is_scanning = false;
    stop_scanning_time = QTime::currentTime();

    int check = 0;
    for (int i = 0; i < clusters.clusters_by_size.size(); ++i) {
        check += clusters.clusters_by_size[i].size();
    }
    //clusters.cluster(dir);

    msec_run_time = 0;
    //clusterThread->start();

    is_running = true;
    start_run_time.restart();

    // Create a thread
    thread = new QThread;
    clusterWorker = new ClusterWorker;

    // Give QThread ownership of Worker Object
    clusterWorker->moveToThread(thread);

    // Connect worker error signal to this errorHandler SLOT.
    //connect(clusterWorker, SIGNAL(error(QString)), this, SLOT(errorHandler(QString)));

    // Connects the thread’s started() signal to the process() slot in the worker, causing it to start.
    connect(clusterWorker, SIGNAL(clusterEnded(int)), SLOT(onClusterEnded(int)));
    connect(thread, SIGNAL(started()), clusterWorker, SLOT(StartWork()));

    // Connect worker finished signal to trigger thread quit, then delete.
    connect(clusterWorker, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);
    connect(clusterWorker, SIGNAL(finished()), clusterWorker, SLOT(deleteLater()), Qt::DirectConnection);

    // Connect worker to runstop-button
    connect(this, SIGNAL(startWork()), clusterWorker, SLOT(StartWork()));
    connect(this, SIGNAL(stopWork()), clusterWorker, SLOT(StopWork()));

    // Make sure the thread object is deleted after execution has finished.
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()), Qt::DirectConnection);

    thread->start();

    ui->pushButton_runstop->setText(QString::fromStdString("Stop"));

    //ui->treeWidget_clusters->setSortingEnabled(false);
    ui->treeWidget_clusters->setSortingEnabled(true);
    ui->pushButton_runstop->setDisabled(false);
    //ui->treeWidget_clusters->header()->setSortIndicatorShown(false);
}

main_window::~main_window() {
    if (clusterWorker)
        clusterWorker->deleteLater();
    if (thread) {
        thread->quit();
        thread->wait();
    }
    if (tmr) {
        delete tmr;
    }
}

void main_window::show_about_dialog() {
    QMessageBox::aboutQt(this);
}


QString main_window::size_human(float num) {
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while (num >= 1024.0 && i.hasNext()) {
        unit = i.next();
        num /= 1024.0;
    }

    if (unit == "bytes") {
        return QString().setNum(num, 'f', 0) + " " + unit;
    }
    return QString().setNum(num, 'f', 2) + " " + unit;
}

std::mutex mtxx;

void main_window::onClusterEnded(int id) {
    //ui->treeWidget_clusters->resizeColumnToContents(2);
    //QFileInfoList list = clusters.getList(i);
    if (id >= 0) {
        int i = id;
        //assert(id < clusters.clusters.size());

        mtx.lock();
        QFileInfo file_info = *clusters.clusters[i][0];
        int64_t file_size = file_info.size();
        int64_t cluster_size = clusters.clusters[i].size();
        mtx.unlock();

        //TreeWidgetItem *item = new TreeWidgetItem(ui->treeWidget_clusters);
        TreeWidgetItem *item = new TreeWidgetItem;
        item->setText(0, QString::number(i + 1));
        item->setText(1, size_human(file_size * (cluster_size - 1)));
        item->setText(2, QString::fromStdString(std::to_string(cluster_size)));
        item->setText(3, file_info.fileName());
        item->setText(4, size_human(file_size));
        item->setText(5, QString::number(file_size));
        item->setText(6, QString::number(file_size * (cluster_size - 1)));
        mtxx.lock();
        ui->treeWidget_clusters->addTopLevelItem(item);

        printedFilesSize += file_size * cluster_size;
        ui->progressBar_printing->setValue(static_cast<int>((100 * printedFilesSize) / clusters.getFilesSize()));
        mtxx.unlock();
    } else {
        ui->pushButton_runstop->setDisabled(true);
        is_running = false;
        stop_run_signal = true;
        QFileInfoList list_of_bads = clusters.getBad();
        for (int i = 0; i < list_of_bads.length(); ++i) {
            QFileInfo file_info = list_of_bads.at(i);
            //TreeWidgetItem *item = new TreeWidgetItem(ui->treeWidget_clusters);
            TreeWidgetItem *item = new TreeWidgetItem;
            item->setText(0, QString::number(i + 1));
            item->setText(1, QString::fromStdString("***"));
            item->setText(2, QString::fromStdString("***"));
            item->setText(3, file_info.absoluteFilePath());
            item->setText(4, QString::fromStdString("***"));
            mtxx.lock();
            ui->treeWidget_clusters->addTopLevelItem(item);
            mtxx.unlock();
        }
    }
}

void main_window::pushButton_runstop_clicked() {
    if (is_running) {
        //clusterThread->terminate();
//        clusterThread->Stop = true;
//        clusterThread->quit();
        stop_run_signal = true;
        running_scanning_time = false;
        emit stopWork();
    } else {
//        clusterThread->start();
        running_scanning_time = true;
        emit startWork();

    }
    is_running = !is_running;
    if (is_running)
        ui->pushButton_runstop->setText(QString::fromStdString("Stop"));
    else
        ui->pushButton_runstop->setText(QString::fromStdString("Run"));
}

void main_window::updateTime() {
    if (stop_run_signal) {
        stop_run_signal = false;
        msec_run_time += start_run_time.elapsed();
    }

    if (is_running) {
        ui->label_printing_time->setText(
                QTime(0, 0, 0, 0).addMSecs(start_run_time.elapsed() + msec_run_time).toString("mm:ss.zzz"));
    } else {
        ui->label_printing_time->setText(QTime(0, 0, 0, 0).addMSecs(msec_run_time).toString("mm:ss.zzz"));
    }


    if (!is_scanning) {
        ui->label_scanning_time->setText(QTime(0, 0, 0, 0).addMSecs(
                stop_scanning_time.msecsSinceStartOfDay() - start_scanning_time.msecsSinceStartOfDay()).toString(
                "mm:ss.zzz"));
    } else {
        ui->label_scanning_time->setText(QTime(0, 0, 0, 0).addMSecs(
                QTime::currentTime().msecsSinceStartOfDay() - start_scanning_time.msecsSinceStartOfDay()).toString(
                "mm:ss.zzz"));
        //ui->label_scanning_time->setText(QTime::currentTime().toString());
    }
}

void main_window::openCluster(QTreeWidgetItem *itemOpened) {
    ui->pushButton_openpath->setDisabled(true);
    ui->pushButton_openfile->setDisabled(true);

    selected_file = "";
    ui->treeWidget_cluster->clear();

    int cluster_number = itemOpened->data(0, Qt::DisplayRole).toInt() - 1;

    std::vector<QFileInfo *> *list = &clusters.clusters[cluster_number];
    for (int i = 0; i < list->size(); ++i) {
        QFileInfo file_info = *(*list)[i];
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget_cluster);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
        //item->setCheckState(0, Qt::Checked);
        item->setCheckState(0, Qt::Unchecked);
        item->setText(1, QString::number(i + 1));
        item->setText(2, file_info.absoluteFilePath());
        item->setText(3, file_info.absolutePath());
        //item->setText(2, QString::number(file_info.size()));
        ui->treeWidget_cluster->addTopLevelItem(item);
    }
}

void main_window::change_selected_file(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    ui->pushButton_openpath->setDisabled(false);
    ui->pushButton_openfile->setDisabled(false);
}

void main_window::pushButton_openpath_clicked() {
    QString selected = ui->treeWidget_cluster->currentItem()->text(3);
    if (!selected.isEmpty())
        QDesktopServices::openUrl(QUrl::fromLocalFile(selected + '/'));
}

void main_window::pushButton_openfile_clicked() {
    QString selected = ui->treeWidget_cluster->currentItem()->text(2);
    if (!selected.isEmpty())
        QDesktopServices::openUrl(QUrl::fromLocalFile(selected));
}