//
// Created by jesus on 01.12.18.
//

#ifndef EXTRA_FILES_FINDER_CLUSTERING_H
#define EXTRA_FILES_FINDER_CLUSTERING_H

#include <QtCore/QFileInfoList>
#include <qt5/QtCore/QFileInfo>
#include <QtCore/QDir>
#include <vector>
#include <algorithm>
#include <headers/consts.h>
#include <QtCore/QCryptographicHash>
#include <mutex>

class clustering {
public:
    std::vector<std::vector<QFileInfo *>> clusters;
    QFileInfoList list;
    QFileInfoList bad_files;

    QFileInfoList listFiles(QDir directory);

    void cluster_by_size(QFileInfoList &list);

    int64_t filesSize = 0;

    int cur_id = -1;

public:
    std::vector<std::vector<QFileInfo *>> clusters_by_size;

    void cluster(QString const &dir);

    QFileInfoList getBad();

    int getSize();

    int64_t getFilesSize();

    void cluster_sizes(QString const &dir);

    int push(std::vector<QFileInfo *> &list);
};


template<>
struct std::hash<QByteArray> {
    size_t operator()(const QByteArray &x) const {
        size_t seed = 0;
        for (auto i : x)
            seed ^= i;
        return seed;
    }
};

extern clustering clusters;
extern std::mutex mtx;


#endif //EXTRA_FILES_FINDER_CLUSTERING_H
