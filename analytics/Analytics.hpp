#pragma once

#include "Database.hpp"
#include "NaiveBayes.hpp"
#include <QStringList>
#include <QString>

//TODO: Add more AI algorithms.

class Analytics {
private:
  NaiveBayes _nb;

  bool _testNaiveBayes(const QString &test_db_path,
		       bool wins,
		       int *rightGuesses,
		       int *wrongGuesses,
		       int *totalGuesses);
public:
  Analytics();
  virtual ~Analytics();

  bool learn(const QString &train_db_path);
  bool test(const QString &test_db_path);
};
