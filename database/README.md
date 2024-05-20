To spawn a POSTGRESQL database with docker:

```
docker run --name database -p 5432:5432 -e POSTGRES_PASSWORD=password -d postgres
```
