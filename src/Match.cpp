#include "Match.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonValue>
#include <QtGlobal>
#include <QUrl>

Match::Match(const QString &id,
	     uint unixT,
	     MatchType gameType,
	     QList<Player *> players) :
  _id(id),
  _date(QDateTime::fromTime_t(unixT)),
  _gameType(gameType),
  _players(players) {
}

Match::~Match() {
  qDeleteAll(_players);
}

QList<Match *> Match::fromJson(const QByteArray &json,
			       const QHash<qint32, QString> &heroNames) {
  QList <Match*> list;
  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(json, &err);

  if (err.error != QJsonParseError::NoError) {
    qCritical() << "Could not parse the json array. Reason:"
		<< err.errorString() << "\n";
    return list;
  }

  QJsonObject obj = doc.object();
  QJsonObject result = obj["result"].toObject();
  int status = result["status"].toInt();

  qDebug() << "Match query status:" << status << "\n";

  if (!status) {
    qCritical() << "Match query status is not 1\n";
    return list;
  }

  foreach (const QJsonValue &value, result["matches"].toArray()) {
    QJsonObject match = value.toObject();
    QList<Player *> players;

    foreach (const QJsonValue &pValue, match["players"].toArray()) {
      QJsonObject player = pValue.toObject();
      qint32 heroId = player["hero_id"].toInt();
      players.append(new Player(player["account_id"].toInt(),
				player["player_slot"].toInt(),
				Hero(heroId, heroNames.value(heroId))));
    }
    list.append(new Match(QString::number(match["match_id"].toInt()),
			  match["start_time"].toInt(),
			  static_cast<MatchType>(match["lobby_type"].toInt()),
			  players));
  }

  return list;
}

MatchType Match::getMatchType() const {
  return _gameType;
}

const QDateTime &Match::getDate() const {
  return _date;
}

const QString &Match::getId() const {
  return _id;
}

const QList<Player *> &Match::getPlayers() const {
  return _players;
}

void Match::setMatchDetails(const MatchDetails &details) {
  _details = details;
}

const MatchDetails &Match::getDetails() const {
  return _details;
}

QDebug operator<<(QDebug dbg, const Match &match) {
  dbg.nospace() << "Match - [id:" << match.getId() << " date:" << match.getDate()
		<< " gameType:" << match.getMatchType() << " players:" << match.getPlayers()
		<< " details:" << match.getDetails() << "]";
  return dbg.space();
}

QDebug operator<<(QDebug dbg, const Match *match) {
  dbg.nospace() << *match;
  return dbg.space();
}

/* ====== MATCH REQUESTER ====== */

MatchRequester::MatchRequester(QObject *parent) :
  QObject(parent), _detailsReceived(0)  {
}

MatchRequester::~MatchRequester() {
  qDeleteAll(_matches);
}

void MatchRequester::_heroesReady(HeroRequester *heroRequester,
				  QHash<qint32, QString> names) {
  _heroNames = names;
  _fetchMatches();
  heroRequester->deleteLater();
}

void MatchRequester::_heroesFetchError(HeroRequester *heroRequester,
				       QString reason) {
  qCritical() << "Could not fetch hero names. reason:" << reason << "\n";
  heroRequester->deleteLater();
  emit matchFetchError(this);
}

void MatchRequester::_fetchMatches(const QString &matchId) {
  ApiRequester *requester = ApiRequester::getInstance();
  QString address = "https://api.steampowered.com/IDOTA2Match_570/"	\
    "GetMatchHistory/V001/?%1&key=%2&format=json&min_players=10&matches_requested=100";
  if (!matchId.isEmpty())
    address = address.arg("start_at_match_id="+matchId);
  else
    address = address.arg("");
  address = address.arg(requester->_key);

  requester->makeRequest(QUrl(address), this);
}

void MatchRequester::getAllMatches() {
  HeroRequester *heroRequester = new HeroRequester(this);
  connect(heroRequester,
	  SIGNAL(heroesReady(HeroRequester *, QHash<qint32, QString>)),
	  this,
	  SLOT(_heroesReady(HeroRequester *, QHash<qint32, QString>)));
  connect(heroRequester,
	  SIGNAL(fetchError(HeroRequester *, QString)),
	  this,
	  SLOT(_heroesFetchError(HeroRequester *, QString)));
  heroRequester->getHeroes();
}

void MatchRequester::_matchDetailsReady(MatchDetailsRequester *detailsRequester,
			MatchDetails details) {
  detailsRequester->deleteLater();
  foreach (Match *match, _matches) {
    if (match->getId() == details._matchId) {
      match->setMatchDetails(details);
      break;
    }
  }
  if (++_detailsReceived == _matches.count())
    //we're ready
    emit matchesReady(this, _matches);
}

void MatchRequester::_matchDetailsFetchError(MatchDetailsRequester *detailsRequester,
			     QString reason) {
  qCritical() << "Could not fetch details. reason:" << reason << "\n";
  detailsRequester->deleteLater();
  emit matchFetchError(this);
}

void MatchRequester::replyReady(const QByteArray &json) {
  QList<Match *> list = Match::fromJson(json, _heroNames);

  //The first match will be present again when using "start_at_match_id"
  if (!_matches.isEmpty())
    list.removeAt(0);

  if (!list.isEmpty()) {
    _matches.append(list);
    const Match *match = _matches.last();
    if (match)
      _fetchMatches(match->getId());
  } else {
    foreach (Match *match , _matches) {
      MatchDetailsRequester *req = new MatchDetailsRequester(this);
      connect(req,
	      SIGNAL(matchDetailsReady(MatchDetailsRequester *, MatchDetails)),
	      this,
	      SLOT(_matchDetailsReady(MatchDetailsRequester *, MatchDetails)));
      connect(req,
	      SIGNAL(fetchError(MatchDetailsRequester *, QString)),
	      this,
	      SLOT(_matchDetailsFetchError(MatchDetailsRequester *,
					   QString)));
      req->fetchMatchDetails(match->getId());
    }
  }
}
