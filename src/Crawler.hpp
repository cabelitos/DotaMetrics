#pragma once

#include "Match.hpp"
#include <QObject>
#include <QString>
#include <QList>
#include <QTimer>

class Crawler : public QObject {
Q_OBJECT

public:
  Crawler(QObject *parent = 0);
  virtual ~Crawler();

  void start();

private:
  QTimer _timer;

private slots:
  void _matchesReady(MatchRequester *self, QList<Match *> matches);
  void _timerTimeout();
};
