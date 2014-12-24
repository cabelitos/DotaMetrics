#include "MatchDetails.hpp"
#include "ApiRequester.hpp"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QtGlobal>


MatchDetails::MatchDetails() : 
  _matchId("Unknown"),
  _radiantWin(false),
  _humanPlayers(0),
  _duration(0), 
  _isValid(false) {
}

MatchDetails::MatchDetails(const QString &matchId,
			   bool radiantWin, int humanPlayers, int duration) : 
  _matchId(matchId),
  _radiantWin(radiantWin),
  _humanPlayers(humanPlayers),
  _duration(duration), 
  _isValid(true) {
}

MatchDetails::~MatchDetails() {
}

QDebug operator<<(QDebug dbg, const MatchDetails &details) {
  dbg.nospace() << "Match details - [id:" << details._matchId << " _radiantWin:"
		<< details._radiantWin << " humanPlayers:" << details._humanPlayers
		<< " valid:" << details._isValid << " duration:"
		<< details._duration << "]";
  return dbg.space();
}

MatchDetailsRequester::MatchDetailsRequester(QObject *parent) :
  QObject(parent) {
}

MatchDetailsRequester::~MatchDetailsRequester() {
}

void MatchDetailsRequester::fetchMatchDetails(const QString &matchId) {
  ApiRequester *requester = ApiRequester::getInstance();
  QString address = "https://api.steampowered.com/IDOTA2Match_570" \
    "/GetMatchDetails/v001/?key=" + requester->_key + "&match_id="+matchId;
  requester->makeRequest(QUrl(address), this);
}

void MatchDetailsRequester::replyReady(const QByteArray &json) {
  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(json, &err);

  if (err.error != QJsonParseError::NoError) {
    qCritical() << "Could not parse the json (Match details). Reason:"
		<< err.errorString() << "\n";
    emit fetchError(this, err.errorString());
  } else {
    QJsonObject result = doc.object()["result"].toObject();
    MatchDetails details = MatchDetails(QString::number(result["match_id"].toInt()),
					result["radiant_win"].toBool(),
					result["human_players"].toInt(),
					result["duration"].toInt());
    emit matchDetailsReady(this, details);
  }
}
