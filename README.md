dirdemo — небольшая демка получения списка файлов на Qt
-------------------------------------------------------

### Описание файлов

Исходный код расположен в файлах
 - `main.cpp`— файл с функцией `main`
 - `mainwindow.h`, `mainwindow.cpp` — код главного окна приложения
 
 Кроме исходного кода есть
 - `mainwindow.ui` — XML-файл с описанием главного окна. В процессе сборки на его основе утилитой `uic` будет сгенерён файл `ui_mainwindow.h`, который включается в `mainwindow.cpp`. `ui`-файлы можно открыть Qt Designer'ом. Qt Creator имеет Qt Designer встроенный в себя и тоже умеет открывать `ui`-файлы.
 - `CMakeLists.txt` — билд-скрипт для `cmake`.  Не используется при билде `qmake`'ом.
 - `dirdemo.pro` — файл-проект для `qmake`. Не используется при билде `cmake`'ом.

### Сборка `cmake`'ом

    $ cmake -DCMAKE_BUILD_TYPE=Debug .
    $ make

Вместо `-DCMAKE_BUILD_TYPE=Debug` может быть `-DCMAKE_BUILD_TYPE=RelWithDebInfo` или что-то другое в зависимости от того, какую конфигурацию вы желаете.

### Сборка qmake'ом

    $ qmake CONFIG+=debug -o Makefile dirdemo.pro
    $ make
