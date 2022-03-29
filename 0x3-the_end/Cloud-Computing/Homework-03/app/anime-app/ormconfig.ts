export default {
  "type": "mysql",
  "host": "localhost",
  "port": 3306,
  "username": "root",
  "password": "",
  "database": "cc_hw2",
  "synchronize": true,
  "logging": true,
  "entities": [
    "entity/**/*.dto{.ts,.js}"
  ]
}