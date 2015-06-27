#include <QCoreApplication>
#include <QtGlobal>
#include <cstdlib>
#include <QByteArray>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QString>
#include <QTextStream>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDateTime>
#include "Crawler.hpp"

static QString _logFilePath = QString();

static void _myMessageOutput(QtMsgType type,
			     const QMessageLogContext &context,
			     const QString &msg) {
  Q_UNUSED(context);
  QString date = QDateTime::currentDateTime().toString("h:m:s  d/M/yyyy t");
  QByteArray localMsg = msg.toLocal8Bit();
  QFile output(_logFilePath + QDir::separator() + "log.txt");
  if (!output.open(QIODevice::Text | QIODevice::Append))
    return;
  QTextStream stream(&output);
  bool mustAbort = false;
  switch (type) {
  case QtDebugMsg:
    stream << date << "-" << localMsg.constData() << "\n";
    break;
  case QtWarningMsg:
    stream << "Warning:" << date << "-" << localMsg.constData() << "\n";
    break;
  case QtCriticalMsg:
    stream << "Critical:" << date << "-" << localMsg.constData() << "\n";
    break;
  case QtFatalMsg:
    stream << "Fatal:" << date << "-" << localMsg.constData() << "\n";
    mustAbort = true;
    break;
  default:
    stream << "Default:" << date << "-" << localMsg.constData() << "\n";
    break;
  }
  stream.flush();
  output.close();
  if (mustAbort)
    abort();
}

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  QCommandLineParser parser;
  QCoreApplication::setApplicationName("Dota Metrics Crawler");
  QCoreApplication::setApplicationVersion("1.0");
  parser.setApplicationDescription("Fetches data from the dota restful api");
  QCommandLineOption logFileOption(QStringList() << "l" << "log",
				   "log file path",
				   "log");
  parser.addOption(logFileOption);
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("key", "Your Steam API key");
  parser.process(app);
  const QStringList args = parser.positionalArguments();
  if (args.isEmpty()) {
    qCritical() << "You need to provide an API key\n";
    parser.showHelp();
    return -1;
  }

  _logFilePath = parser.value(logFileOption);
  if (!_logFilePath.isEmpty()) {
    qDebug() << "Saving log at:" << _logFilePath << "\n";
    qInstallMessageHandler(_myMessageOutput);
  }
  ApiRequester::getInstance(args.at(0));
  Crawler c;
  c.start();
  app.exec();
  return 0;
}
