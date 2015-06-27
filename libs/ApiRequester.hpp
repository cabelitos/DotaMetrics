#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QHash>
#include <QByteArray>

class ApiRequester : public QObject {

Q_OBJECT

public:

  class IReplyReadyListener {
  public:
    virtual ~IReplyReadyListener() {}
    virtual void replyReady(const QByteArray &json) = 0;
  };

  virtual ~ApiRequester();
  static ApiRequester *getInstance(const QString &key = QString());
  void makeRequest(QUrl url, ApiRequester::IReplyReadyListener *listener);
  void removeListener(ApiRequester::IReplyReadyListener *listener);

  const QString _key;

private:
  ApiRequester(QObject *parent = 0, const QString &key = QString());
  ApiRequester(ApiRequester const& copy);
  ApiRequester& operator=(ApiRequester const& copy);

  static ApiRequester *_instance;

  QNetworkAccessManager _networkManager;
  QHash<QNetworkReply *, IReplyReadyListener*> _requests;

private slots:
  void _finishedSlot(QNetworkReply *reply);
};
