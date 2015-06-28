CREATE TABLE Match(_id integer primary key autoincrement,
       match_id text not null unique on conflict ignore,
       time_stamp integer not null,
       game_type integer not null);

CREATE TABLE Player(_id integer primary key autoincrement,
       player_id integer not null unique on conflict ignore);

CREATE TABLE Heroes(_id integer primary key autoincrement,
       hero_id integer not null unique on conflict ignore,
       name text not null unique on conflict ignore);

CREATE TABLE MatchDetails(_id integer primary key autoincrement,
       match_id text not null unique on conflict ignore,
       radiant_win integer not null,
       human_players integer not null,
       duration integer not null,
       FOREIGN KEY (match_id) REFERENCES Match(match_id));

CREATE TABLE MatchPlayerHeroes(_id integer primary key autoincrement,
       match_id text not null,
       hero_id integer not null,
       player_id integer not null,
       player_slot integer not null,
       UNIQUE (match_id, hero_id, player_id) on conflict ignore,
       FOREIGN KEY (match_id) REFERENCES Match(match_id),
       FOREIGN KEY (hero_id) REFERENCES Heroes(hero_id),
       FOREIGN KEY (player_id) REFERENCES Player(player_id));

CREATE VIEW WinningHeroes AS
       SELECT M._id, MPH.match_id, MPH.hero_id, H.name FROM MatchDetails AS MD
       INNER JOIN MatchPlayerHeroes AS MPH ON MPH.match_id = MD.match_id
       INNER JOIN Heroes as H on MPH.hero_id = H.hero_id
       INNER JOIN Match as M ON MPH.match_id = M.match_id
       WHERE MPH.match_id IN (
SELECT match_id FROM MatchPlayerHeroes
WHERE hero_id != 0
GROUP BY match_id HAVING COUNT(match_id) = 10) AND
       MD.radiant_win = (((MPH.player_slot & 128) >> 0) = 0) AND
       MPH.hero_id != 0
       ORDER BY MPH.match_id, MPH.hero_id;

CREATE VIEW LosingHeroes AS
       SELECT M._id, MPH.match_id, MPH.hero_id, H.name FROM MatchDetails AS MD
       INNER JOIN MatchPlayerHeroes AS MPH ON MPH.match_id = MD.match_id
       INNER JOIN Heroes as H on MPH.hero_id = H.hero_id
       INNER JOIN Match as M ON MPH.match_id = M.match_id
       WHERE MPH.match_id IN (
SELECT match_id FROM MatchPlayerHeroes
WHERE hero_id != 0
GROUP BY match_id HAVING COUNT(match_id) = 10) AND
       MD.radiant_win = (((MPH.player_slot & 128) >> 0) != 0) AND
       MPH.hero_id != 0
       ORDER BY MPH.match_id, MPH.hero_id;

CREATE TRIGGER DeleteMatch BEFORE DELETE
ON Match
FOR EACH ROW
BEGIN
DELETE FROM MatchDetails WHERE MatchDetails.match_id = old.match_id;
DELETE FROM MatchPlayerHeroes WHERE MatchPlayerHeroes.match_id = old.match_id;
END;
