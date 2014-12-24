#pragma once

#include "ApiRequester.hpp"
#include <QObject>
#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QHash>
#include <QDebug>

class Hero {

private:
  qint32 _id;
  QString _name;

public:
  Hero(qint32 id, const QString &name);
  virtual ~Hero();

  qint32 getId() const;
  const QString & getName() const;
};

class HeroRequester : public QObject, public ApiRequester::IReplyReadyListener {
Q_OBJECT
public:
  HeroRequester(QObject *parent = 0);
  virtual ~HeroRequester();
  void replyReady(const QByteArray &json);
  
  void getHeroes();

signals:
  void heroesReady(HeroRequester *self, QHash<qint32, QString> names);
  void fetchError(HeroRequester *self, QString reason);
};

QDebug operator<<(QDebug dbg, const Hero &hero);


