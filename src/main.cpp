#include <QCoreApplication>
#include "Crawler.hpp"

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    ApiRequester::getInstance("a");
    Crawler c;
    c.start();
    app.exec();
    return 0;
}
