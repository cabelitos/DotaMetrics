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
#include "Crawler.hpp"

static void _myMessageOutput(QtMsgType type,
			     const QMessageLogContext &context,
			     const QString &msg) {
  Q_UNUSED(context);
  QByteArray localMsg = msg.toLocal8Bit();
  QFile output(QDir::currentPath() + QDir::separator() + "log.txt");
  //Prey for the gods, this can't fail!
  output.open(QIODevice::Text | QIODevice::Append);
  QTextStream stream(&output);
  switch (type) {
  case QtDebugMsg:
    stream << localMsg.constData() << "\n";
    break;
  case QtWarningMsg:
    stream << "Warning:" << localMsg.constData() << "\n";
    break;
  case QtCriticalMsg:
    stream << "Critical:" << localMsg.constData() << "\n";
    break;
  case QtFatalMsg:
    stream << "Fatal:" << localMsg.constData() << "\n";
    abort();
    break;
  default:
    stream << "Default:" << localMsg.constData() << "\n";
    break;
  }
  stream.flush();
  output.close();
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
