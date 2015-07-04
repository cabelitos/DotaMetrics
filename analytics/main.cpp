#include "Analytics.hpp"
#include "Utils.hpp"
#include <QString>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QCoreApplication>

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  QString train, test;
  QCommandLineParser parser;

  QCoreApplication::setApplicationName("Dota Metrics Analytics");
  QCoreApplication::setApplicationVersion("1.0");
  parser.setApplicationDescription("Test bed for AI algorithms");

  QCommandLineOption replaceDB(QStringList() << "r" << "replaceDB",
				   "Replace the current train and test database");

  parser.addOption(replaceDB);
  parser.addHelpOption();
  parser.addVersionOption();
  parser.process(app);

  if (!Utils::createTestAndTrainDatabase(train, test,
					 parser.isSet(replaceDB))) {
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
}
