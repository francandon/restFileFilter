FROM postgres:latest

COPY ./* /docker-entrypoint-initdb.d/

# docker build -t lobis/postgres .
# docker run --name database -p 5432:5432 -e POSTGRES_PASSWORD=password -d lobis/postgres
