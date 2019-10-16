EXTRA files finder
-------------------------------------------------------

EXTRA files finder is the application of the future that allows you to find all files duplicates and delete unnecessary copies!

### Instruction of the using

**Important** if you want to be able to delete files, then uncomment the line 462 at `mainwindow.cpp`

    $ //QFile(item->text(2)).remove();                // TODO: IS OFF TO DON'T DELETE SOMETHING IMPORTANT

**Important** app shows all interesting files that have analogues of the same size. It means sometimes it shows you files with **NO COPIES**. It's lifehack and I'm sure app should work so.

### I already want to run it. How to do this?

You have two options:

CMake-way building:

    $ cmake -DCMAKE_BUILD_TYPE=Debug .
    $ make
    
You are also free to write if you want, for example, `-DCMAKE_BUILD_TYPE=RelWithDebInfo` instead of `-DCMAKE_BUILD_TYPE=Debug`

QMake-way building:

    $ qmake CONFIG+=debug -o Makefile extra_files_finder.pro
    $ make

To run it just write in terminal:

    $ ./extra_files_finder

### Used tecnhologies

- QT framework - for UI application and multi-threading
- C++ - as the language for a backend development

### Idea of the solution. What is behind code symbols?

Ok, that is very easy to describe:

1. We cluster files by their sizes by the method `clustering::cluster_by_size`
2. Compare files of the same size by their hashes Sha3_256

### Files description

Code of the application:
 - `main.cpp` - file with function `main`
 - `mainwindow.h`, `mainwindow.cpp` - the code of the main application
 - `clustering.cpp`, `clustering.h` - for the first step in our idea
 - `clusterWorker.cpp`, `clusterWorker.h` - for the second step. It starts in an extra thread by `mainwindow.cpp` to don't disturb UI-thread
 
Also:
 - `mainwindow.ui` — XML-file with the description of the main window. Utility `uic` uses it for the building of the file `ui-mainwindow.h`, which includes to `mainwindow.cpp`. `ui`-files can be opened by QT Designer or QT Creator.
 - `CMakeLists.txt` — `cmake`'s build-scrypt.
 - `extra_files_finder.pro` — `qmake`'s build-scrypt.
