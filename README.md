Dota Metrics
===========

This project aims to statistically report which hero combination has the chance to win. 

Depends on
----------
[Qt5](http://qt-project.org/) [version 5.4]


Setup
----------
```shell
cmake . && make

```

If cmake didn't find Qt5 libs, try the following:
```shell
cmake . DCMAKE_PREFIX_PATH=/Path/To/Qt5 && make

```

Running
-------
Right now, the Crawler will collect data from the latest 500 matches every 5 minutes. The data collected will reside at a SQLite databse called dota_metrics.db inside the "res" folder

```shell
./dota_metrics YourSteamApiKey
```
