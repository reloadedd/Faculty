use rusqlite::{Connection};
use super::super::constants;

pub fn setup() {
    let conn = Connection::open(constants::DB_PATH).unwrap();

    match conn.execute(r#"
        CREATE TABLE IF NOT EXISTS Users (
            username TEXT NOT NULL UNIQUE PRIMARY KEY,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            master_password TEXT NOT NULL
        )
    "#, []) {
        Err(err) => panic!("ERROR: {}", err),
        _ => ()
    }
}

pub fn get_connection() -> Connection {
    Connection::open(constants::DB_PATH).unwrap()
}