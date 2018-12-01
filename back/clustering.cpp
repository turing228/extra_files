//
// Created by jesus on 01.12.18.
//

#include <headers/clustering.h>
#include <fstream>
#include <QtCore/QCryptographicHash>

bool by_size(QFileInfo &a, QFileInfo &b) {
    return a.size() < b.size();
}

void clustering::cluster(QString const &dir) {
    list = listFiles(dir);

    std::sort(list.begin(), list.end(), by_size);

    cluster_by_size(list);

    cluster_inside_sizes();

    //clusters.push_back(clusters_by_size[3]);
}

void clustering::cluster_inside_sizes() {
    for (size_t i = 0; i < clusters_by_size.size(); ++i) {
        if (clusters_by_size[i][0].size() <= __BLOCK_SIZE) {
            cluster_small_size(i);
        } else {
            cluster_big_size(i);
        }
    }
}


void clustering::cluster_small_size(size_t cluster_number) {
    QFileInfoList local_list = clusters_by_size[cluster_number];

    std::vector<bool> used(local_list.length(), false);
    //std::vector<bool> is_bad(local_list.length(), false);

    std::vector<std::pair<int, QByteArray>> files;

    for (size_t i = 0; i < local_list.length(); ++i) {
        QFile file(local_list.at(i).absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly)) {
            //is_bad[i] = true;
            bad_files.push_back(local_list[i]);
        } else {
            files.emplace_back(i, file.readAll());
        }
        file.close();
    }

    int files_size = files.size();

    int blob_size = 0;
    if (files_size > 0) {
        blob_size = files[0].second.size();
    }

    for (size_t i = 0; i < files_size - 1; ++i) {
        if (!used[i]) {
            QFileInfoList local_cluster;
            local_cluster.push_back(local_list[files[i].first]);
            used[i] = true;

            for (size_t j = i + 1; j < files_size; ++j) {

                if (!used[j]) {
                    bool is_same = true;
                    for (int h = 0; h < blob_size; ++h) {
                        if (files[i].second[h] != files[j].second[h]) {
                            is_same = false;
                            break;
                        }
                    }
                    if (is_same) {
                        local_cluster.push_back(local_list[files[j].first]);
                        used[j] = true;
                    }
                }
            }
            clusters.push_back(local_cluster);
        }
    }

    /*
    for (size_t i = 0; i < local_list.length(); ++i) {
        if (!is_bad[i] && !used[i]) {
            QFile file_cluster(local_list.at(i).absoluteFilePath());
            file_cluster.open(QIODevice::ReadOnly);
            QByteArray blob_cluster = file_cluster.readAll();

            QFileInfoList local_cluster;
            local_cluster.push_back(local_list[i]);
            used[i] = true;

            for (size_t j = i + 1; j < local_list.length(); ++j) {

                QFile file(local_list.at(j).absoluteFilePath());
                file.open(QIODevice::ReadOnly);

                if (!is_bad[j] && !used[j]) {
                    QByteArray blob = file.readAll();
                    bool is_same = true;
                    for (int h = 0; h < blob_cluster.size(); ++h) {
                        if (blob_cluster.at(h) != blob.at(h)) {
                            is_same = false;
                            break;
                        }
                    }
                    if (is_same) {
                        local_cluster.push_back(local_list[j]);
                        used[j] = true;
                    }
                }
                file.close();
            }
            file_cluster.close();
            clusters.push_back(local_cluster);
        }
    }*/
}

void clustering::cluster_big_size(size_t cluster_number) {
    QFileInfoList local_list = clusters_by_size[cluster_number];

    if (local_list.length() == 1) {
        clusters.push_back(local_list);
    } else {
        std::vector<std::pair<QByteArray, int>> files;

        QCryptographicHash hash(QCryptographicHash::Sha256);
        for (size_t i = 0; i < local_list.length(); ++i) {
            QFile file(local_list.at(i).absoluteFilePath());
            if (!file.open(QIODevice::ReadOnly)) {
                bad_files.push_back(local_list[i]);
            } else {
                hash.reset();
                hash.addData(&file);
                files.emplace_back(hash.result(), i);
            }
            file.close();
        }

        sort(files.begin(), files.end());

        int n = 0;
        int clust_size = 1;
        while (n < files.size()) {
            QFileInfoList new_list;
            new_list.push_back(local_list.at(files[n].second));
            while (n + clust_size < files.size() && files[n].first == files[n + clust_size].first) {
                new_list.push_back(local_list.at(files[n + clust_size].second));
                clust_size++;
            }
            n = n + clust_size;
            clust_size = 1;
            clusters.push_back(new_list);
        }
    }
    int x = 10;

    /*
    std::vector<FILE *> files;
    files.resize(local_list.length());
    std::vector<int> first_list;
    for (int i = 0; i < local_list.length(); ++i) {
        FILE *file = std::fopen(local_list.at(i).absoluteFilePath().toStdString().c_str(), "rb");
        if (file == NULL) {
            bad_files.push_back(local_list.at(i));
            std::fclose(file);
        } else {
            files[i] = file;
            first_list.push_back(i);
        }
    }

    std::vector<char *> cur_local_clusters_buffers;
    std::vector<std::vector<int>> local_clusters_A;
    std::vector<std::vector<int>> local_clusters_B;

    local_clusters_A.push_back(first_list);

    int file_sizes = local_list[0].size();

    int sizes_dev_blocksize = file_sizes / __BLOCK_SIZE;

    char *buffer_A = (char *) malloc(sizeof(char) * __BLOCK_SIZE);
    char *buffer_B = (char *) malloc(sizeof(char) * __BLOCK_SIZE);

    std::vector<QFileInfoList> vector_one;
    QFileInfoList list_one;
    list_one.push_back(local_list[0]);

    for (int i = 0; i < sizes_dev_blocksize + 1; ++i) {
        int cur_block_size = __BLOCK_SIZE;
        if (file_sizes - __BLOCK_SIZE * i < __BLOCK_SIZE) {
            cur_block_size = file_sizes - __BLOCK_SIZE * i;
        }
        if (cur_block_size > 0) {
            for (int t = 0; t < local_clusters_A.size(); ++t) {
                for (int j = 0; j < local_clusters_A[t].size(); ++j) {
                    fread(cur_local_clusters_buffers[j], 1, cur_block_size,
                          files[local_clusters_A[t][j]] + i * __BLOCK_SIZE);
                }
                std::vector<bool> used(local_list.length(), false);
                for (int j = 0; j < local_clusters_A[t].size(); ++j) {
                    if (!used[j]) {
                        used[j] = true;
                        std::vector<int> local_cluster;
                        local_cluster.push_back(j);
                        bool is_same = true;
                        for (int g = 0; g < local_clusters_A[t].size(); ++g) {
                            if (!used[g]) {
                                for (int k = 0; k < cur_block_size; ++k) {
                                    if (cur_local_clusters_buffers[j][k] != cur_local_clusters_buffers[j][g]) {
                                        is_same = false;
                                        break;
                                    }
                                }
                                if (is_same) {
                                    local_cluster.push_back(g);
                                    used[g] = true;
                                }
                            }
                        }
                        if (local_cluster.size() == 1) {
                            //std::vector<QFileInfoList> vector_one;
                            //QFileInfoList list_one;
                            list_one[0] = local_list.at(local_clusters_A[t][j]);
                            clusters.push_back(list_one);
                        } else {
                            local_clusters_B.push_back(local_cluster);
                        }
                    }
                }
            }
        }
        local_clusters_A = local_clusters_B;
        local_clusters_B.clear();
    }


    for (int i = 0; i < local_clusters_A.size(); ++i) {
        QFileInfoList new_list;
        for (int j = 0; j < local_clusters_A[i].size(); ++j) {
            new_list.push_back(local_list.at(local_clusters_A[i][j]));
        }
        clusters.push_back(new_list);
    }



    for (int i = 0; i < local_list.length(); ++i) {
        std::fclose(files[i]);
    }*/
}


void clustering::cluster_by_size(QFileInfoList &list) {
    int n = 0;
    int cluster_begin = 0;
    while (n < list.size()) {
        QFileInfoList list_of_same_size;
        while (n < list.size() && list[cluster_begin].size() == list[n].size()) {
            list_of_same_size.push_back(list[n]);
            n++;
        }
        cluster_begin = n;
        clusters_by_size.push_back(list_of_same_size);
    }
}

QFileInfoList clustering::listFiles(QDir directory) {
    QDir dir(directory);
    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList result;

    for (QFileInfo file_info : list) {
        if (file_info.isDir()) {
            result.append(listFiles(file_info.absoluteFilePath()));
        } else {
            result.push_back(file_info);
        }
    }

    return result;
}

QFileInfoList clustering::getList(size_t i) {
    return clusters[i];
}

QFileInfoList clustering::getList() {
    QFileInfoList final_list;
    for (size_t i = 0; i < clusters.size(); ++i) {
        final_list.append(clusters[i]);
    }
    return final_list;
}


bool by_list_size(QFileInfoList &a, QFileInfoList &b) {
    return a.length() > b.length();
}

bool by_extra_size(QFileInfoList &a, QFileInfoList &b) {
    return ((a.length() - 1) * a[0].size()) > ((b.length() - 1) * b[0].size());
}

QFileInfoList clustering::getSortedList() {
    QFileInfoList final_list;
    std::sort(clusters.begin(), clusters.end(), by_extra_size);
    for (size_t i = 0; i < clusters.size(); ++i) {
        final_list.append(clusters[i]);
    }
    return final_list;
}

int clustering::getSize() {
    return clusters.size();
}
