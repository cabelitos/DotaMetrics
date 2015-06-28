
#include "Analytics.hpp"
#include "Utils.hpp"

#define WON_CLASS ("Won")
#define LOST_CLASS ("Lost")

Analytics::Analytics() {
}

Analytics::~Analytics() {
}

bool Analytics::learn(const QString &train_db_path) {
  bool err, r;

  DataBase db("Analytics", train_db_path);
  if (!db.open()) {
    qCritical() << "Could not open the DB!";
    return false;
  }
  _nb.clearObservations();

  _nb.addClass(WON_CLASS);
  _nb.addClass(LOST_CLASS);
  QHash<int, QList<QString> *> winners = db.getWinningHeroes(&err);

  if (err) {
    qCritical() << "Could not load the winning heroes";
    return false;
  }

  QHash<int, QList<QString> *> losers = db.getLosingHeroes(&err);
  if (err) {
    qCritical() << "Could not load the losing heroes";
    qDeleteAll(winners);
    return false;
  }

  r = true;
  foreach(int match, winners.keys()) {
    QList<QString> *winner_ids = winners.value(match);
    QList<QString> *losers_ids = losers.value(match);
    if (!losers_ids) {
      qCritical() << "Could not find the match:" << match
		  << " in the losers list";
      r = false;
      _nb.clearObservations();
      break;
    }
    _nb.addObservation(*winner_ids, WON_CLASS);
    _nb.addObservation(*losers_ids, LOST_CLASS);
  }

  qDeleteAll(winners);
  qDeleteAll(losers);
  return r;
}

bool Analytics::_testNaiveBayes(const QString &test_db_path,
				bool wins,
				int *rightGuesses,
				int *wrongGuesses,
				int *total) {
  DataBase db("Analytics", test_db_path);
  QHash<int, QList<QString> *> heroes;
  bool err;
  int right, wrong;

  if (!db.open()) {
    qCritical() << "Could not open the DB!";
    return false;
  }

  if (wins)
    heroes = db.getWinningHeroes(&err);
  else
    heroes = db.getLosingHeroes(&err);

  if (err) {
    qCritical() << "Could not fetch the heroes";
    return false;
  }
  right = wrong = 0;
  foreach(const QList<QString> *team, heroes) {
    QList<QPair<QString, float> > result = _nb.classify(*team);
    qDebug() << "result:" << result;
    if ((wins && result.at(0).first == QLatin1String(WON_CLASS)) ||
	(!wins && result.at(0).first == QLatin1String(LOST_CLASS)))
      right++;
    else
      wrong++;
  }
  *rightGuesses = right;
  *wrongGuesses = wrong;
  *total = heroes.size();
  qDeleteAll(heroes);
  return true;
}

bool Analytics::test(const QString &test_db_path) {
  int rightWin, wrongWin, totalWin, rightLost, wrongLost, totalLost;

  qDebug() << "Testing naive bayes approach 1";
  qDebug() << "Testing with winning heroes";
  if (!_testNaiveBayes(test_db_path, true, &rightWin, &wrongWin, &totalWin)) {
    qCritical() << "Could not test the winning heroes";
    return false;
  }
  qDebug() << "Testing with losing heroes";
  if (!_testNaiveBayes(test_db_path, false, &rightLost, &wrongLost,
		       &totalLost)) {
    qCritical() << "Could not test the losing heroes";
    return false;
  }
  qDebug() << "=== RESULTS (Naive bayes approuch 1) ===";
  qDebug() << "Total guesses:" << totalWin + totalLost;
  qDebug() << "Right guesses for winning heroes:" << rightWin;
  qDebug() << "Wrong guesses for winning heroes:" << wrongWin;

  qDebug() << "Right winning heroes %:" << (rightWin * 100) / totalWin << "%";
  qDebug() << "Wrong winning heroes %:" << (wrongWin * 100) / totalWin << "%";

  qDebug() << "Right guesses for losing heroes:" << rightLost;
  qDebug() << "Wrong guesses for losing heroes:" << wrongLost;

  qDebug() << "Right losing heroes %:" << (rightLost * 100) / totalLost << "%";
  qDebug() << "Wrong losing heroes %:" << (wrongLost * 100) / totalLost << "%";

  qDebug() << "Right guesses (total):" << rightWin + rightLost;
  qDebug() << "Wrong guesses (total):" << wrongWin + wrongLost;

  qDebug() << "Right total %:"
	   << ((rightLost + rightWin) * 100) / (totalLost + totalWin) << "%";
  qDebug() << "Wrong total heroes %:"
	   << ((wrongLost + wrongWin) * 100) / (totalLost + totalWin) << "%";
  return true;
}
