DROP SCHEMA public CASCADE;
CREATE SCHEMA public;

GRANT
ALL
ON SCHEMA public TO postgres;
GRANT ALL
ON SCHEMA public TO public;

DROP TABLE IF EXISTS files;

CREATE TABLE files
(
    path      text PRIMARY KEY,
    processed BOOLEAN   NOT NULL DEFAULT false,
    stat_uid  text      NOT NULL,
    stat_gid  text      NOT NULL,
    stat_atim TIMESTAMP NOT NULL,
    stat_mtim TIMESTAMP NOT NULL,
    stat_ctim TIMESTAMP NOT NULL,
    stat_size BIGINT    NOT NULL
);
