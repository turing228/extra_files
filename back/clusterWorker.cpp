//
// Created by jesus on 15.12.18.
//

#include <headers/clustering.h>
#include "headers/clusterWorker.h"
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <headers/clusterWorker.h>


size_t lastCluster;

ClusterWorker::ClusterWorker(QObject *parent) : QObject(parent), stopped(false), running(false) {
    lastCluster = 0;
    Stop = false;
    isNew = true;
}


void ClusterWorker::StopWork() {
    stopped = true;
    running = false;
}

void ClusterWorker::StartWork() {
    stopped = false;
    running = true;
    process();
}

void ClusterWorker::process() {
    if (!running || stopped) {
        return;
    }

    if (isNew) {
        lastCluster = 0;
        isNew = false;
    }

    size_t i = lastCluster;
    if (i < clusters.clusters_by_size.size()) {
        cluster_big_size(i);

//        if (clusters.clusters_by_size[i][0]->size() <= __BLOCK_SIZE && clusters.clusters_by_size[i].size() < 1000) {
//            cluster_small_size(i);
//        } else {
//            cluster_big_size(i);
//        }

        lastCluster++;


    } else {
        emit clusterEnded(-1);
        StopWork();
        emit finished();
    }

    QMetaObject::invokeMethod(this, "process", Qt::QueuedConnection);
}

void ClusterWorker::cluster_small_size(size_t cluster_number) {
    std::vector<QFileInfo *> local_list = clusters.clusters_by_size[cluster_number];

    std::vector<bool> used(local_list.size(), false);

    std::vector<std::pair<int, QByteArray>> files;

    for (size_t i = 0; i < local_list.size(); ++i) {
        QFile file(local_list.at(i)->absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            clusters.bad_files.push_back(*local_list[i]);
        } else {
            files.emplace_back(i, file.readAll());
        }
    }

    size_t files_size = files.size();

    for (size_t i = 0; i < files_size; ++i) {
        if (used[i])
            continue;

        std::vector<QFileInfo *> local_cluster;
        local_cluster.push_back(local_list[files[i].first]);
        used[i] = true;

        for (size_t j = i + 1; j < files_size; ++j) {
            if (used[j])
                continue;

            bool is_same = std::equal(files[i].second.begin(), files[i].second.end(), files[j].second.begin(),
                                      files[j].second.end());
            if (is_same) {
                local_cluster.push_back(local_list[files[j].first]);
                used[j] = true;
            }
        }

        emit clusterEnded(clusters.push(local_cluster));
    }
}

void ClusterWorker::cluster_big_size(size_t cluster_number) {
    std::vector<QFileInfo *> local_list = clusters.clusters_by_size[cluster_number];

    if (local_list.size() == 1) {
        QFile file(local_list[0]->absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            clusters.bad_files.push_back(*(local_list[0]));
        } else {
            emit clusterEnded(clusters.push(local_list));
        }
    } else {

        std::unordered_multimap<QByteArray, int> files_map;

        int test_files_map_size_eq_to_list_size = 0;

        QCryptographicHash hash(QCryptographicHash::Sha3_256);
        for (size_t i = 0; i < local_list.size(); ++i) {
            QFile file(local_list[i]->absoluteFilePath());
            if (!file.open(QIODevice::ReadOnly)) {
                clusters.bad_files.push_back(*local_list[i]);
                //test_files_map_size_eq_to_list_size++;
            } else {
                hash.reset();
                hash.addData(&file);
                files_map.emplace(hash.result(), i);
                test_files_map_size_eq_to_list_size++;
            }
        }
        //assert(files_map.size() == test_files_map_size_eq_to_list_size);

        auto it = files_map.begin();

        int test_files_map_size = 0;

        while (it != files_map.end()) {
            auto range = files_map.equal_range(it->first);
            std::vector<QFileInfo *> new_list;
            for (auto i = range.first; i != range.second; ++i) {
                new_list.push_back(local_list.at(i->second));

                test_files_map_size++;

            }
            it = range.second;

            //if (new_list.size() > 1)
            emit clusterEnded(clusters.push(new_list));
        }
        //assert(files_map.size() == test_files_map_size);
    }
}
