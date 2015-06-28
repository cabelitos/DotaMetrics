#pragma once

#include "Match.hpp"
#include "Player.hpp"
#include "Hero.hpp"
#include "MatchDetails.hpp"
#include <QList>
#include <QSqlDatabase>
#include <QString>
#include <QHash>

class DataBase {

private:
  QString _connName;
  QString _dbPath;
  QSqlDatabase _db;

  void _createDB();
  void _setupDB();

  bool _insertMatch(const Match &match);
  bool _insertMatchDetails(const MatchDetails &details);
  bool _insertPlayersAndHeroInfo(const QList<Player*> &players,
				 const QString &match_id);
  bool _insertHero(const Hero &hero);
  bool _insertMatchPlayerHeroes(const Player &player,
				const Hero &hero,
				const QString &match_id);
  QHash<int, QList<QString> *> _getHeroes(const QString &table, bool *err);

public:
  DataBase(const QString &connName = QString(), const QString &dbPath = QString());
  virtual ~DataBase();

  void insertValues(const QList<Match*> &matches);
  QHash<int, QList<QString> *> getWinningHeroes(bool *err);
  QHash<int, QList<QString> *> getLosingHeroes(bool *eer);
  bool open();
  void setConnNameAndPath(const QString &connName, const QString &dbPath);
  QList<int> getValidMatchesIds(bool *err);
  bool deleteMatchesById(const QList<int> &matches);
};
