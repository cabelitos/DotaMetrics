#include "Utils.hpp"
#include "DataBase.hpp"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <algorithm>
#include <QFile>

const QString Utils::DATABASE_NAME = "dota_metrics.db";
const QString Utils::DOTA_ANALYTICS_DIR_NAME = "dota_analytics";
#define STEP (300)

Utils::Utils() {
}

Utils::~Utils() {
}

QString Utils::getAppDir() {
  QString path = QStandardPaths::
    writableLocation(QStandardPaths::DocumentsLocation)
    + QDir::separator() + DOTA_ANALYTICS_DIR_NAME;

  QDir myDir(path);
  if (!myDir.exists() && !myDir.mkpath(path))
    qWarning() << "Could not create the dota analytics dir at:" << path;
  return path;
}

QString Utils::getDatabasePath() {
  return Utils::getAppDir() + QDir::separator() + Utils::DATABASE_NAME;
}

bool Utils::_deleteMatches(const QString &path,
			   const QList<int> &matches,
			   int start, int total) {
  DataBase db("Utils", path);
  if (!db.open()) {
    qCritical() << "Could not open the databse at:" << path;
    return false;
  }

  int from = start;
  for (int i = 0; i < total; i += STEP) {
    int count = STEP;
    if (i + STEP >= total)
      count = total - i;
    if (!db.deleteMatchesById(matches.mid(from == start ?
					  from : from + 1, count)))
      return false;
    from += STEP;
  }
  return true;
}

bool Utils::createTestAndTrainDatabase(QString &trainFile, QString &testFile) {
  QString dbPath = Utils::getDatabasePath();
  QFile dbFile(dbPath);
  bool r = false, err;
  QList<int> match_ids;
  int totalTrain, totalTest;

  if (!dbFile.exists()) {
    qCritical() << "Dota database does not exists at:" << dbPath;
    return r;
  }
  testFile = Utils::getAppDir() + QDir::separator() + "dota_test.db";
  trainFile = Utils::getAppDir() + QDir::separator() + "dota_train.db";
  bool trainExists = QFile::exists(trainFile);
  bool testExists = QFile::exists(testFile);

  if (trainExists && testExists) {
    qDebug() << "Train and test file already present!";
    return true;
  }

  qDebug() << "Creating train db";
  if (!trainExists && !QFile::copy(dbPath, trainFile)) {
    qCritical() << "Could not create the train database";
    goto exit;
  }

  qDebug() << "Creating test db";
  if (!testExists && !QFile::copy(dbPath, testFile)) {
    qCritical() << "Could not create the test database";
    goto exit;
  }

  {
    DataBase mainDB("Utils", dbPath);
    if (!mainDB.open()) {
      qCritical() << "Could not open the DB at:" << dbPath;
      goto exit;
    }

    match_ids = mainDB.getValidMatchesIds(&err);
    if (err) {
      qCritical() << "Could not read the total valid matches";
      goto exit;
    }
  }

  totalTrain = (match_ids.size() * 70) / 100;
  totalTest = match_ids.size() - totalTrain;
  std::random_shuffle(match_ids.begin(), match_ids.end());

  qDebug() << "Total data:" << match_ids.size();
  qDebug() << "Data size used to train:" << totalTrain;
  qDebug() << "Data size used to test:" << totalTest;

  if (!Utils::_deleteMatches(trainFile, match_ids, 0,
			     totalTest))
    goto exit;
  qDebug() << match_ids.size();
  if (!Utils::_deleteMatches(testFile, match_ids, totalTest,
			     totalTrain))
    goto exit;

  r = true;
 exit:
  if (!r) {
    QFile::remove(trainFile);
    QFile::remove(testFile);
  }
  return r;
}
