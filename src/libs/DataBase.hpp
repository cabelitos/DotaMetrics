#pragma once

#include "Match.hpp"
#include "Player.hpp"
#include "Hero.hpp"
#include "MatchDetails.hpp"
#include <QList>
#include <QSqlDatabase>
#include <QString>

class DataBase {

private:
  QSqlDatabase _db;

  void _openDB();
  void _createDB();

  bool _insertMatch(const Match &match);
  bool _insertMatchDetails(const MatchDetails &details);
  bool _insertPlayersAndHeroInfo(const QList<Player*> &players,
				 const QString &match_id);
  bool _insertHero(const Hero &hero);
  bool _insertMatchPlayerHeroes(const Player &player,
				const Hero &hero,
				const QString &match_id);

public:
  DataBase();
  virtual ~DataBase();

  void insertValues(const QList<Match*> &matches);
};
