CREATE
USER explorer WITH PASSWORD 'password';

GRANT CONNECT
ON DATABASE postgres TO explorer;

GRANT USAGE ON SCHEMA
public TO explorer;

GRANT
SELECT
ON ALL TABLES IN SCHEMA public TO explorer;
