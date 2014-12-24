#include <QCoreApplication>
#include <QtGlobal>
#include <cstdlib>
#include <QByteArray>
#include <QFile>
#include <QDir>
#include <QString>
#include <QTextStream>
#include <QCommandLineParser>
#include <iostream>
#include <QDateTime>
#include "Crawler.hpp"

static void _myMessageOutput(QtMsgType type,
			     const QMessageLogContext &context,
			     const QString &msg) {
  Q_UNUSED(context);
  QString date = QDateTime::currentDateTime().toString("h:m:s  d/M/yyyy t");
  QByteArray localMsg = msg.toLocal8Bit();
  QFile output(QDir::currentPath() + QDir::separator() + "log.txt");
  //Prey for the gods, this can't fail!
  output.open(QIODevice::Text | QIODevice::Append);
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
  qInstallMessageHandler(_myMessageOutput);
  QCoreApplication app(argc, argv);
  QCommandLineParser parser;
  QCoreApplication::setApplicationName("Dota Metrics Crawler");
  QCoreApplication::setApplicationVersion("1.0");
  parser.setApplicationDescription("Fetches data from the dota restful api");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("key", "Your Steam API key");
  parser.process(app);
  const QStringList args = parser.positionalArguments();
  if (args.isEmpty()) {
    std::cout << "You need to provide an API key\n";
    parser.showHelp();
    return -1;
  }
  ApiRequester::getInstance(args.at(0));
  Crawler c;
  c.start();
  app.exec();
  return 0;
}
