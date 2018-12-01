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

main_window::main_window(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);


    ui->treeWidget->setUniformRowHeights(true);


    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);

    //scan_directory(QDir::homePath());
}

main_window::~main_window() {}

void main_window::select_directory() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    scan_directory(dir);
}

void main_window::scan_directory(QString const &dir) {
    ui->treeWidget->clear();
    setWindowTitle(QString("Directory Content - %1").arg(dir));
    QDir d(dir);

    clustering clusters;
    clusters.cluster(dir);
    clusters.getSortedList();

    for (int i = 0; i < clusters.getSize(); ++i) {
        QFileInfoList list = clusters.getList(i);
        for (int j = 0; j < list.length(); ++j) {
            QFileInfo file_info = list.at(j);
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
            item->setText(0, QString::number(i + 1));
            item->setText(1, QString::number(file_info.size() * (list.length() - 1)));
            item->setText(2, QString::fromStdString(std::to_string(j + 1) + '/' + std::to_string(list.length())));
            item->setText(3, file_info.absoluteFilePath());
            //item->setText(0, file_info.fileName());
            item->setText(4, QString::number(file_info.size()));
            ui->treeWidget->addTopLevelItem(item);
        }
        //ui->progressBar->setValue(100);
    }
}

void main_window::show_about_dialog() {
    QMessageBox::aboutQt(this);
}
