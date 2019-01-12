//
// Created by jesus on 01.12.18.
//

#include <back/headers/clustering.h>
#include <fstream>
#include <QtCore/QCryptographicHash>
#include <unordered_map>
#include <mutex>

clustering clusters;
std::mutex mtx;

bool by_size(QFileInfo &a, QFileInfo &b) {
    return a.size() < b.size();
}

void clustering::cluster(QString const &dir) {
    list = listFiles(dir);

    std::sort(list.begin(), list.end(), by_size);

    cluster_by_size(list);

    //cluster_inside_sizes();

    //clusters.push_back(clusters_by_size[3]);
}

void clustering::cluster_sizes(QString const &dir) {
    list = listFiles(dir);

    std::sort(list.begin(), list.end(), by_size);

    cluster_by_size(list);
}


void clustering::cluster_by_size(QFileInfoList &list) {
    int n = 0;
    int cluster_begin = 0;
    std::vector<QFileInfo *> list_of_same_size;
    int64_t sizes;
    while (n < list.size()) {
        sizes = 0;
        while (n < list.size() && list[cluster_begin].size() == list[n].size()) {
            list_of_same_size.push_back(&list[n]);
            sizes += list[n].size();
            n++;
        }
        cluster_begin = n;
        if (list_of_same_size.size() > 1) {
            // TODO: как пометка здесь. Здесь мы говорим, что оставляем только одинаковые по размеру. Мол цель программы обрабатывать *похожие* файлы, а не все
            filesSize += sizes;
            clusters_by_size.push_back(list_of_same_size);
        }
        list_of_same_size.clear();
    }
}

QFileInfoList clustering::listFiles(QDir directory) {
    QDir dir(directory);

    QFileInfoList list = dir.entryInfoList(
            QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden); //| QDir::NoDotAndDotDot
    QFileInfoList result;

    for (QFileInfo file_info : list) {
        if (!file_info.isSymLink() && file_info.isDir()) {
            result.append(listFiles(file_info.absoluteFilePath()));
        } else {
            if (file_info.isSymLink()) {
                file_info = file_info.symLinkTarget();
            }
            result.push_back(file_info);
        }
    }

    return result;
}


bool by_list_size(QFileInfoList &a, QFileInfoList &b) {
    return a.size() > b.size();
}

bool by_extra_size(QFileInfoList &a, QFileInfoList &b) {
    return ((a.size() - 1) * a[0].size()) > ((b.size() - 1) * b[0].size());
}

int clustering::getSize() {
    return clusters.size();
}

QFileInfoList clustering::getBad() {
    return bad_files;
}

int64_t clustering::getFilesSize() {
    if (filesSize > 0) {
        return filesSize;
    }

    for (int i = 0; i < list.size(); ++i) {
        filesSize += list.at(i).size();
    }
    return filesSize;
}

int clustering::push(std::vector<QFileInfo *> &list) {
    mtx.lock();
    clusters.push_back(list);
    cur_id++;
    int ans = cur_id;
    mtx.unlock();
    return ans;
}
