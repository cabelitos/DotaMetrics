#include "Analytics.hpp"
#include "Utils.hpp"
#include <QTemporaryFile>
#include <QString>
#include <QStringList>

int main(int argc, char **argv) {
  QString train, test;
  if (!Utils::createTestAndTrainDatabase(train, test)) {
    qCritical() << "Could not create test and train databases";
    return -1;
  }

  qDebug() << train;
  qDebug() << test;
  Analytics anal;
  if (!anal.learn(train))
    return -1;
  if (!anal.test(test))
    return -1;
  return 0;
  Q_UNUSED(argc);
  Q_UNUSED(argv);
}
