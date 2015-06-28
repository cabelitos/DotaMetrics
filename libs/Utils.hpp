#pragma once

#include <QString>
#include <QList>

class Utils {

private:
  Utils();

  static bool _deleteMatches(const QString &path,
			     const QList<int> &matches,
			     int start, int end);
public:
  virtual ~Utils();

  static const QString DATABASE_NAME;
  static const QString DOTA_ANALYTICS_DIR_NAME;

  static QString getAppDir();
  static QString getDatabasePath();
  static bool createTestAndTrainDatabase(QString &trainFile, QString &testFile);
};
