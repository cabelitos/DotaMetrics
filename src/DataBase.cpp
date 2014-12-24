#include "DataBase.hpp"
#include <QtGlobal>
#include <QtDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStringList>
#include <QSqlQuery>

DataBase::DataBase() {
}

DataBase::~DataBase() {
  _db.close();
}

void DataBase::_createDB(const QString &basePath) {
  QString sqlFile(basePath);
  sqlFile.append(QDir::separator()).append("database.sql");

  qDebug() << "Openning:" << sqlFile << "\n";
  QFile f(sqlFile);

  if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    qFatal("Could not open the sql file");

  QTextStream stream(&f);
  QStringList commands = stream.readAll().split(";");

  foreach (const QString &command, commands) {
    if (command.trimmed().isEmpty())
      continue;
    QSqlQuery query;
    query.prepare(command);
    if (!query.exec())
      qFatal("Could not execute the command: %s \n",
	     command.toStdString().c_str());
  } 

  f.close();
}

void DataBase::_openDB() {
  _db = QSqlDatabase::addDatabase("QSQLITE");
  if (!_db.isValid())
    qFatal("Could not create a Sqlite database!\n");

  QString basePath(QDir::currentPath());
  basePath.append(QDir::separator()).append("res");

  QString dbPath(basePath);
  dbPath.append(QDir::separator()).append("dota_metrics.db");

  QFile file(dbPath);
  qDebug() << "Database path:" << dbPath << "\n";

  _db.setDatabaseName(dbPath);
  bool mustCreate = !file.exists();

  if (!_db.open())
    qFatal("Could not open the database\n");

  if (mustCreate) {
    qDebug() << "Database does not exists. Creating\n";
    _createDB(basePath);
  }
  QSqlQuery query;
  query.prepare("PRAGMA foreign_keys = ON");
  if (!query.exec())
    qCritical() << "Could not turn foriegn keys on";
}

bool DataBase::_insertMatchDetails(const MatchDetails &details) {
  QSqlQuery query;
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
  QSqlQuery query;
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
  QSqlQuery query;
  query.prepare("insert into Heroes (hero_id, name) values(?, ?)");
  query.addBindValue(hero.getId());
  query.addBindValue(hero.getName());
  return query.exec();
}

bool DataBase::_insertMatchPlayerHeroes(const Player &player,
					const Hero &hero,
					const QString &match_id) {
  QSqlQuery query;
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
    QSqlQuery query;
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
  if (!_db.isOpen())
    _openDB();

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
