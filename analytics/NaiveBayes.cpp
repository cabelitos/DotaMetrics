#include "NaiveBayes.hpp"
#include <QDebug>
#include <cmath>
#include <algorithm>

NaiveBayes::NaiveBayes() : _totalSamples(0) {
}

NaiveBayes::~NaiveBayes() {
  clearObservations();
}

void NaiveBayes::clearObservations() {
  qDeleteAll(_tokens);
  qDeleteAll(_classes);
  _tokens.clear();
  _classes.clear();
}

bool NaiveBayes::addClass(const QString &klass) {
  qDebug() << "Adding class:" << klass;

  if (_classes.contains(klass)) {
    qWarning() << "The class:" << klass << " is already present!";
    return false;
  }
  _classes.insert(klass, new int(0));
  return true;
}

bool NaiveBayes::addObservation(const QStringList &tokens, const QString &klass) {
  NaiveBayesFeature *feature;
  int *count;

  if (!_classes.contains(klass)) {
    qWarning() << "The class:" << klass << " is not present!";
    return false;
  }

  count = _classes.value(klass);
  (*count)++;
  _totalSamples++;
  foreach (const QString &token, tokens) {
    if (!_tokens.contains(token)) {
      feature = new NaiveBayesFeature(token);
      _tokens.insert(token, feature);
    } else
      feature = _tokens.value(token);
    feature->addObservation(klass);
  }
  return true;
}

float NaiveBayes::_probability(const QString &token, const QString &klass,
			       int classTotalOccurences,
			       int numberOfTokens,
			       float class_prob) {
  NaiveBayesFeature *feature;

  if (!_tokens.contains(token)) {
    return (numberOfTokens * class_prob) /
    (classTotalOccurences +  numberOfTokens);
  }

  feature = _tokens.value(token);
  return feature->probability(klass, classTotalOccurences, numberOfTokens,
			      class_prob);
}

static bool _results_sort(const QPair<QString, float> &p1,
			  const QPair<QString, float> &p2) {
  if (p1.second > p2.second)
    return true;
  return false;
}

QList<QPair<QString, float> > NaiveBayes::classify(const QStringList &tokens) {
  QHash<QString, float> posteriors;
  QList<QPair<QString, float> > results;
  int *count;
  float p, posterior;

  foreach(const QString &klass, _classes.keys()) {
    count = _classes.value(klass);
    p = (float)*count / _totalSamples;
    qDebug() << "Probability of being:" << klass << " is:" << p;
    posteriors.insert(klass, p);
  }

  foreach(const QString &klass, _classes.keys()) {
    count = _classes.value(klass);
    p = posterior = posteriors.value(klass);
    p = std::log(p);
    foreach(const QString &token, tokens)
      p += std::log(_probability(token, klass, *count, tokens.size(),
				 posterior));
    results.push_back(qMakePair(klass, p));
  }
  std::sort(results.begin(), results.end(), _results_sort);
  qDebug() << results;
  return results;
}

NaiveBayes::NaiveBayesFeature::NaiveBayesFeature(const QString &name) :
  _name(name) {
}

NaiveBayes::NaiveBayesFeature::~NaiveBayesFeature() {
  qDeleteAll(_occurrences);
}

void NaiveBayes::NaiveBayesFeature::addObservation(const QString &klass) {
  int *count;
  if (!_occurrences.contains(klass)) {
    count = new int(0);
    _occurrences.insert(klass, count);
  } else
    count = _occurrences.value(klass);
  (*count)++;
  qDebug() << "Token:" << _name << " has feature:" << klass << " count:"
	   << *count;
}

float NaiveBayes::NaiveBayesFeature::probability(const QString &klass,
						 int classTotalOccurences,
						 int numberOfTokens,
						 float class_prob) {
  int *count = _occurrences.value(klass);
  return (float)(*count + (numberOfTokens * class_prob)) /
    (classTotalOccurences +  numberOfTokens);
}
