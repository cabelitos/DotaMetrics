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
