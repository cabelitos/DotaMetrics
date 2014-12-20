#pragma once

#include "Hero.hpp"
#include <QtGlobal>
#include <QDebug>

class Player {
private:
  qint32 _id;
  qint8 _slot;
  Hero _hero;

public:
  Player(qint32 id, qint8 slot, const Hero &hero);
  virtual ~Player();

  qint32 getPlayerId() const;
  qint8 getSlot() const;
  const Hero &getHero() const;
};

QDebug operator<<(QDebug dbg, const Player &player);
QDebug operator<<(QDebug dbg, const Player *player);
