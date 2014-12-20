#pragma once

#include "ApiRequester.hpp"
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QDebug>

class MatchDetails {
public:
  MatchDetails();
  MatchDetails(const QString &matchId, bool radiantWin, int humanPlayers, int duration);
  virtual ~MatchDetails();

  QString _matchId;
  bool _radiantWin;
  int _humanPlayers;
  int _duration;
  bool _isValid;
};

class MatchDetailsRequester : public QObject,
			      public ApiRequester::IReplyReadyListener {
Q_OBJECT
public:
  MatchDetailsRequester(QObject *parent = 0);
  virtual ~MatchDetailsRequester();

  void fetchMatchDetails(const QString &matchId);
  void replyReady(const QByteArray &json);

signals:
  void matchDetailsReady(MatchDetailsRequester *self, MatchDetails details);
  void fetchError(MatchDetailsRequester *self, QString reason);
};

QDebug operator<<(QDebug dbg, const MatchDetails &details);
