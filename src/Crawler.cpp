#include "Crawler.hpp"
#include "DataBase.hpp"
#include <QtGlobal>
#include <QDebug>
#include <QCoreApplication>

#define DEFAULT_TIMEOUT (600000)

Crawler::Crawler(QObject *parent) : QObject(parent) {
  connect(&_timer, SIGNAL(timeout()), this, SLOT(_timerTimeout()));
}

Crawler::~Crawler() {
}

void Crawler::_timerTimeout() {
  MatchRequester *mRequester = new MatchRequester(this);
  connect(mRequester,
	  SIGNAL(matchesReady(MatchRequester *, QList<Match*>)),
	  this,
	  SLOT(_matchesReady(MatchRequester *, QList<Match*>)));
  qDebug() << "Requesting matches\n";
  mRequester->getAllMatches();
}

void Crawler::start() {
  _timer.start(DEFAULT_TIMEOUT);
  _timerTimeout();
}

void Crawler::_matchesReady(MatchRequester *requester, QList<Match *> matches) {
  qDebug() << "Matches ready to be inspected!\n";
  DataBase db;
  db.insertValues(matches);
  requester->deleteLater();
  qDebug() << "All matches inserted to the database";
}


