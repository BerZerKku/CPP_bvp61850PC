#include "mainwindow.h"

#include <QApplication>

int
main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;

    QString pattern = QString("%1 %{function} (%2%3%4%5%6): %{message} %7%8").
                      arg("[%{time h:mm:ss.zzz}]").
                      arg("%{if-debug}\033[32;1mD%{endif}").
                      arg("%{if-info}\033[32;1mI%{endif}").
                      arg("%{if-warning}\033[33;1mW%{endif}").
                      arg("%{if-critical}\033[31;1mC%{endif}").
                      arg("%{if-fatal}\033[31;1mF%{endif}").
                      arg("\033[0m").
                      arg("\n   Loc: [%{file}, line %{line}]");

    qSetMessagePattern(pattern);

    w.show();
    return a.exec();
}
