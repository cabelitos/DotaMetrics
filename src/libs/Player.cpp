#include "Player.hpp"

Player::Player(qint32 id, qint8 slot, const Hero &hero) :
  _id(id), _slot(slot), _hero(hero) {
}

Player::~Player() {
}

qint32 Player::getPlayerId() const {
  return _id;
}

qint8 Player::getSlot() const {
  return _slot;
}

const Hero &Player::getHero() const {
  return _hero;
}

QDebug operator<<(QDebug dbg, const Player &player) {
  dbg.nospace() << "Player - [ id:" << player.getPlayerId() << " slot:"
		<< player.getSlot() << " hero:" << player.getHero() << "]";
  return dbg.space();
}

QDebug operator<<(QDebug dbg, const Player *player) {
  dbg.nospace() << *player;
  return dbg.space();
}
