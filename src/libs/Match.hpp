#pragma once

#include "ApiRequester.hpp"
#include "Player.hpp"
#include "MatchDetails.hpp"
#include "Hero.hpp"
#include <QObject>
#include <QList>
#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QHash>
#include <QDebug>

typedef enum _MatchType {
  MATCH_TYPE_INVALID = -1,
  MATCH_TYPE_PUBLIC_MATCH_MAKING,
  MATCH_TYPE_PRACTICE,
  MATCH_TYPE_TOURNAMENT,
  MATCH_TYPE_TUTORIAL,
  MATCH_TYPE_COOP_WITH_BOTS,
  MATCH_TYPE_TEAM_MATCH,
  MATCH_TYPE_SOLO_QUEUE,
  MATCH_TYPE_COMPETITIVE,
  MATCH_TYPE_CASUAL_1v1,
} MatchType;

class Match {

private:
  QString _id;
  QDateTime _date;
  MatchType _gameType;
  QList<Player *> _players;
  MatchDetails _details;

public:
  Match(const QString &id,
	uint unitT,
	MatchType gameType,
	QList<Player *> players);
  virtual ~Match();

  static QList<Match *> fromJson(const QByteArray &json,
				 const QHash<qint32, QString> &heroNames);

  MatchType getMatchType() const;
  const QDateTime &getDate() const;
  const QString &getId() const;
  const QList<Player *> &getPlayers() const;
  void setMatchDetails(const MatchDetails &details);
  const MatchDetails &getDetails() const;
};

class MatchRequester : public QObject,
		       public ApiRequester::IReplyReadyListener {
Q_OBJECT

private:
  QList<Match *> _matches;
  QHash <qint32, QString> _heroNames;
  int _detailsReceived;

  void _fetchMatches(const QString &matchId = QString());

private slots:
  void _heroesReady(HeroRequester *heroRequester, QHash<qint32, QString> names);
  void _heroesFetchError(HeroRequester *heroRequester, QString reason);
  void _matchDetailsReady(MatchDetailsRequester *detailsRequester,
			  MatchDetails details);
  void _matchDetailsFetchError(MatchDetailsRequester *detailsRequester,
			       QString reason);

public:
  MatchRequester(QObject *parent = 0);
  virtual ~MatchRequester();

  void getAllMatches();

  void replyReady(const QByteArray &json);

signals:
  void matchesReady(MatchRequester *self, QList<Match *> matches);
  void matchFetchError(MatchRequester *self);
};

QDebug operator<<(QDebug dbg, const Match &match);
QDebug operator<<(QDebug dbg, const Match *match);
