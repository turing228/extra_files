//
// Created by jesus on 01.12.18.
//

#ifndef DIRDEMO_CLUSTERING_H
#define DIRDEMO_CLUSTERING_H

#include <QtCore/QFileInfoList>
#include <qt5/QtCore/QFileInfo>
#include <QtCore/QDir>
#include <vector>
#include <algorithm>
#include <headers/consts.h>

class clustering {
private:
    std::vector<QFileInfoList> clusters;
    std::vector<QFileInfoList> clusters_by_size;
    QFileInfoList list;
    QFileInfoList bad_files;

    QFileInfoList listFiles(QDir directory);

    void fill_vector_files();

    void cluster_by_size(QFileInfoList &list);

    void cluster_inside_sizes();

    void cluster_small_size(size_t cluster_number);

    void cluster_big_size(size_t cluster_number);

public:
    void cluster(QString const &dir);

    QFileInfoList getList(size_t i);

    QFileInfoList getList();

    QFileInfoList getSortedList();

    int getSize();
};


#endif //DIRDEMO_CLUSTERING_H
