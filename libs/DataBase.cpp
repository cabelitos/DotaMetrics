#include "DataBase.hpp"
#include <QtGlobal>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QResource>
#include <QStringBuilder>

DataBase::DataBase(const QString &connName, const QString &dbPath) :
  _connName(connName),
  _dbPath(dbPath) {
  Q_INIT_RESOURCE(resources);
}

void DataBase::setConnNameAndPath(const QString &connName,
				  const QString &dbPath) {
  _dbPath = dbPath;
  _connName = connName;
}

DataBase::~DataBase() {
  _db.close();
  //Avoid silly warning.
  _db = QSqlDatabase();
  QSqlDatabase::removeDatabase(_connName);
}

void DataBase::_createDB() {
  QResource sqlFile(":/sql/database.sql");
  QFile f(sqlFile.absoluteFilePath());

  if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    qFatal("Could not open the sql file");

  QTextStream stream(&f);
  QStringList commands = stream.readAll().split(";");

  foreach (const QString &command, commands) {
    if (command.trimmed().isEmpty())
      continue;
    QSqlQuery query(_db);
    query.prepare(command);
    if (!query.exec())
      qFatal("Could not execute the command: %s \n",
	     command.toStdString().c_str());
  } 

  f.close();
}

bool DataBase::open() {
  if (_connName.isEmpty()) {
    qCritical() << "Connection name is empty";
    return false;
  }

  if (_dbPath.isEmpty()) {
    qCritical() << "DB path is empty";
    return false;
  }

  if (QSqlDatabase::contains(_connName)) {
    _db = QSqlDatabase::database(_connName);
    return true;
  }

  _db = QSqlDatabase::addDatabase("QSQLITE", _connName);
  if (!_db.isValid())
    qFatal("Could not create a Sqlite database!\n");

  QFile file(_dbPath);
  qDebug() << "Database path:" << _dbPath << "\n";

  _db.setDatabaseName(_dbPath);
  bool mustCreate = !file.exists();

  if (!_db.open())
    qFatal("Could not open the database\n");

  if (mustCreate) {
    qDebug() << "Database does not exists. Creating\n";
    _createDB();
  }

  QSqlQuery query(_db);
  query.prepare("PRAGMA foreign_keys = ON");
  if (!query.exec())
    qCritical() << "Could not turn foriegn keys on";
  return true;
}

bool DataBase::_insertMatchDetails(const MatchDetails &details) {
  QSqlQuery query(_db);
  if (!details._isValid) {
    qCritical() << "Match details is not valid! details" << details << "\n";
    return false;
  }
  query.prepare("insert into MatchDetails (match_id, radiant_win,"\
		" human_players, duration)"\
		" values(?, ?, ?, ?)");
  query.addBindValue(details._matchId);
  query.addBindValue(details._radiantWin ? 1 : 0);
  query.addBindValue(details._humanPlayers);
  query.addBindValue(details._duration);
  return query.exec();
}

bool DataBase::_insertMatch(const Match &match) {
  QSqlQuery query(_db);
  query.prepare("insert into Match (match_id, time_stamp, game_type)"\
		" values(?, ?, ?)");
  query.addBindValue(match.getId());
  query.addBindValue(match.getDate().toTime_t());
  query.addBindValue(static_cast<int>(match.getMatchType()));
  if (!query.exec()) {
    qCritical() << "Could not insert match!\n";
    return false;
  }
  return _insertMatchDetails(match.getDetails());
}

bool DataBase::_insertHero(const Hero &hero) {
  QSqlQuery query(_db);
  query.prepare("insert into Heroes (hero_id, name) values(?, ?)");
  query.addBindValue(hero.getId());
  query.addBindValue(hero.getName());
  return query.exec();
}

bool DataBase::_insertMatchPlayerHeroes(const Player &player,
					const Hero &hero,
					const QString &match_id) {
  QSqlQuery query(_db);
  query.prepare("insert into MatchPlayerHeroes "\
		" (match_id, hero_id, player_id, player_slot) "\
		" values(?, ?, ?, ?)");
  query.addBindValue(match_id);
  query.addBindValue(hero.getId());
  query.addBindValue(player.getPlayerId());
  query.addBindValue(player.getSlot());
  return query.exec();
}

bool DataBase::_insertPlayersAndHeroInfo(const QList<Player*> &players,
					 const QString &match_id) {
  foreach (const Player *player, players) {
    QSqlQuery query(_db);
    query.prepare("insert into Player (player_id) values(?)");
    query.addBindValue(player->getPlayerId());
    if (!query.exec()) {
      qCritical() << "Could not insert player:" << player << "\n";
      return false;
    }
    Hero hero = player->getHero();
    if (!_insertHero(hero)) {
      qCritical() << "Could not insert hero:" << hero << "\n";
      return false;
    }
    if (!_insertMatchPlayerHeroes(*player, hero, match_id)) {
      qCritical() << "Could not insert to Match Player Hero" << player
		  << " hero:" << hero << "\n";
      return false;
    }
  }
  return true;
}

void DataBase::insertValues(const QList<Match*> &matches) {
  foreach (const Match *match, matches) {
    if (!_db.transaction())
      qFatal("Could not start a transaction!\n");
    if (!_insertMatch(*match)) {
      qCritical() << "Could not insert the match:" << *match << "\n";
      _db.rollback();
      continue;
    }

    if (!_insertPlayersAndHeroInfo(match->getPlayers(), match->getId())) {
      qCritical() << "Could not insert players for match:" << match << "\n";
      _db.rollback();
      continue;
    }

    _db.commit();
  }
}

QHash<int, QList<QString> *> DataBase::_getHeroes(const QString &table, bool *err) {
  QHash<int, QList<QString> *> result;
  bool r = false;
  QList<QString> *names;
  int currentMatch, newMatch;

  QSqlQuery query(_db);
  query.prepare("select _id, name from " + table);

  if (!query.exec()) {
    r = true;
    qCritical() << "Could not fetch the heroes!";
  } else {
    int match = query.record().indexOf("_id");
    int hero_name = query.record().indexOf("name");
    currentMatch = -1;
    while (query.next()) {
      newMatch = query.value(match).toInt();
      if (currentMatch != newMatch) {
	names = new QList<QString>();
	result.insert(newMatch, names);
	currentMatch = newMatch;
      }
      names->push_back(query.value(hero_name).toString());
    }
  }

  if (err)
    *err = r;
  return result;
}

QHash<int, QList<QString> *> DataBase::getWinningHeroes(bool *err) {
  return _getHeroes("WinningHeroes", err);
}

QHash<int, QList<QString> *> DataBase::getLosingHeroes(bool *err) {
  return _getHeroes("LosingHeroes", err);
}

QList<int> DataBase::getValidMatchesIds(bool *err) {
  QList<int> res;
  bool r = true;
  QSqlQuery query(_db);

  query.prepare("select _id from WinningHeroes group by _id");
  if (!query.exec()) {
    qCritical() << "Could not execute the count query";
    goto exit;
  }
  while (query.next())
    res.push_back(query.value(0).toInt());
  r = false;
 exit:
  if (err)
    *err = r;
  return res;
}

bool DataBase::deleteMatchesById(const QList<int> &matches) {
  QString str = "";
  QSqlQuery query(_db);
  foreach (int i, matches)
    str = str % "," % QString::number(i);
  str = str.remove(0, 1);
  //Oh boy...
  query.prepare("delete from Match where _id in (" + str +")");
  if (!query.exec()) {
    qCritical() << "Could not delete the matches!";
    return false;
  }
  _db.commit();
  return true;
}
