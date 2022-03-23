#include "dynamic_graph.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    dynamic_graph w;
    w.show();
    return a.exec();
}
