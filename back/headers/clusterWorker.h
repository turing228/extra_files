//
// Created by jesus on 15.12.18.
//

#ifndef EXTRA_FILES_FINDER_CLUSTERWORKER_H
#define EXTRA_FILES_FINDER_CLUSTERWORKER_H

#include <QThread>
#include <headers/clustering.h>
#include <mutex>
#include <QtCore/QObject>

class ClusterWorker : public QObject {
Q_OBJECT
public:
    explicit ClusterWorker(QObject *parent = nullptr);
    //ClusterWorker();

    //virtual ~ClusterWorker();
    bool isNew = true;
    bool Stop = false;

public slots:

    void process();

    void StopWork();

    void StartWork();

signals:

    void finished();

    void error(QString err);

    void clusterEnded(int);

    //void running();
    //void stopped();

private:
    void cluster_small_size(size_t cluster_number);

    void cluster_big_size(size_t cluster_number);

    size_t lastCluster;
    volatile bool stopped, running;
};

extern int cur_id;

#endif //EXTRA_FILES_FINDER_CLUSTERWORKER_H
