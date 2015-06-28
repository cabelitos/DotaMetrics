#pragma once

#include <QHash>
#include <QList>
#include <QString>
#include <QPair>
#include <QStringList>

class NaiveBayes {

private:

  class NaiveBayesFeature {
  private:
    QHash<QString, int *> _occurrences;
    QString _name;

  public:
    NaiveBayesFeature(const QString &name);
    virtual ~NaiveBayesFeature();
    void addObservation(const QString &klass);
    float probability(const QString &klass, int classTotalOccurences,
		      int numberOfTokens, float class_prob);
  };

  QHash<QString, int *> _classes;
  QHash<QString, NaiveBayesFeature *> _tokens;
  int _totalSamples;

  float _probability(const QString &token, const QString &klass,
		     int classTotalOccurences, int numberOfTokens,
		     float class_prob);

public:
  NaiveBayes();
  virtual ~NaiveBayes();
  bool addClass(const QString &klass);
  bool addObservation(const QStringList &tokens, const QString &klass);
  QList<QPair<QString, float> > classify(const QStringList &tokens);
  void clearObservations();
};
