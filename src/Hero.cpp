#include "Hero.hpp"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QUrl>

Hero::Hero(qint32 id, const QString &name) : _id(id), _name(name) {
}

Hero::~Hero() {
}

qint32 Hero::getId() const {
  return _id;
}

const QString & Hero::getName() const {
  return _name;
}

HeroRequester::HeroRequester(QObject *parent) : QObject(parent) {
}

HeroRequester::~HeroRequester() {
}

void HeroRequester::getHeroes() {
  ApiRequester *requester = ApiRequester::getInstance();
  QString address = "https://api.steampowered.com/IEconDOTA2_570/" \
    "GetHeroes/v001/?key=" + requester->_key + "&language=en-US";
  requester->makeRequest(QUrl(address), this);
}

void HeroRequester::replyReady(const QByteArray &json) {
  QJsonParseError err;
  QJsonDocument doc = QJsonDocument::fromJson(json, &err);
  QHash<qint32, QString> names;
  if (err.error != QJsonParseError::NoError) {
    qCritical() << "Could not parse the json array. Reason:"
		<< err.errorString() << "\n";
    emit fetchError(this, err.errorString());
  } else {
    QJsonObject result = doc.object()["result"].toObject();
    foreach (const QJsonValue &value, result["heroes"].toArray()) {
      QJsonObject hero = value.toObject();
      names.insert(hero["id"].toInt(), hero["localized_name"].toString());
    }
    emit heroesReady(this, names);
  }
}

QDebug operator<<(QDebug dbg, const Hero &hero) {
  dbg.nospace() << "Hero - [id:" << hero.getId() << " name:" << hero.getName() << "]";
  return dbg.space();
}

