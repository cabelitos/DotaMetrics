#include "ApiRequester.hpp"
#include <QNetworkRequest>
#include <QtGlobal>
#include <QDebug>

ApiRequester *ApiRequester::_instance = nullptr;

ApiRequester::ApiRequester(QObject *parent, const QString &key) :
  QObject(parent), _key(key) {
  connect(&_networkManager,
	  SIGNAL(finished(QNetworkReply*)),
	  this,
	  SLOT(_finishedSlot(QNetworkReply*)));
}

ApiRequester::~ApiRequester() {
}

ApiRequester *ApiRequester::getInstance(const QString &key) {
  if (!ApiRequester::_instance)
    ApiRequester::_instance = new ApiRequester(0, key);
  return ApiRequester::_instance;
}

void ApiRequester::_finishedSlot(QNetworkReply *reply) {
  qDebug() << "Received reply";

  ApiRequester::IReplyReadyListener *listener = _requests.take(reply);

  if (!listener)
    qCritical() << "Listener is null!\n";
  else
    listener->replyReady(reply->readAll());

  reply->deleteLater();
}

void ApiRequester::makeRequest(QUrl url,
			       ApiRequester::IReplyReadyListener *listener) {
  QNetworkReply *reply = _networkManager.get(QNetworkRequest(url));
  _requests.insert(reply, listener);
  qDebug() << "Making request to:" << url;
}
