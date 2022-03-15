use serde_derive::{Serialize, Deserialize};
use crate::db::config::get_connection;
use crate::utils;
use crate::http::status_codes::{*};
use std::option::Option;

#[derive(Debug)]
#[derive(Serialize, Deserialize)]
pub struct User {
    pub username: String,
    pub first_name: String,
    pub last_name: String,
    pub master_password: String
}

impl User {
    pub fn new(content: &str) -> Self {
        /* We know for sure that the unwrap will work because we did the check before */
        serde_json::from_str::<User>(&content).unwrap()
    }

    pub fn get(somebody: Option<&str>) -> String {
        let conn = get_connection();

        match somebody {
            Some(username) => {
                let mut stmt = match conn.prepare("SELECT * FROM Users WHERE username LIKE (?)") {
                    Ok(whatever) => whatever,
                    Err(e) => panic!("ERROR: {e}")
                };

                let db_result = match stmt.query_row(&[&username],
                 |row| {
                     Ok(User {
                         username: row.get(0)?,
                         first_name: row.get(1)?,
                         last_name: row.get(2)?,
                         master_password: row.get(3)?,
                     })
                 }) {
                    Ok(whatever) => whatever,
                    Err(e) => {
                        return utils::make_response(&format!(r#"
                            {{
                                "status": "error",
                                "message": "The specified user does not exist."
                            }}
                            "#), &HTTP_NOT_FOUND
                        )
                    }
                };

                utils::make_response(&format!(r#"
                    {{
                        "status": "ok",
                        "message": "Data about user '{0}' fetched.",
                        "data": {{
                            "username": "{0}",
                            "first_name": "{1}",
                            "last_name": "{2}"
                        }}
                    }}
                    "#, db_result.username, db_result.first_name, db_result.last_name),&HTTP_CREATED
                )
            },
            None => {
                let mut stmt = match conn.prepare("SELECT * FROM Users") {
                    Ok(whatever) => whatever,
                    Err(e) => panic!("ERROR: {e}")
                };

                let mut db_result = match stmt.query([]) {
                    Ok(whatever) => whatever,
                    Err(e) => panic!("ERROR: {e}")
                };

                let mut users = Vec::new();
                while let Some(row) = db_result.next().unwrap() {
                    users.push(User {
                        username: row.get(0).unwrap(),
                        first_name: row.get(1).unwrap(),
                        last_name: row.get(2).unwrap(),
                        master_password: row.get(3).unwrap()
                    })
                }

                let mut response = r#"
                    {
                        "status": "ok",
                        "message": "All users' data fetched (wink wink Google).",
                        "data": ["#.to_owned();

                let mut index = 0;
                loop {
                    if index == users.len() - 1 {
                        break
                    }

                    response.push_str(&format!(r#"
                            {{
                                "username": "{}",
                                "first_name": "{}",
                                "last_name": "{}"
                            }},"#, users[index].username, users[index].first_name, users[index].last_name));

                    index += 1;
                }

                response.push_str(&format!(r#"
                            {{
                                "username": "{}",
                                "first_name": "{}",
                                "last_name": "{}"
                            }}
                        ]
                    }}"#, users[index].username, users[index].first_name, users[index].last_name));

                utils::make_response(&response, &HTTP_CREATED)
            }
        }
    }

    pub fn post(&self) -> String {
        let mut conn = get_connection();
        let tx = match conn.transaction() {
            Ok(whatever) => whatever,
            Err(e) => panic!("ERROR: {e}")
        };

        match tx.execute(
            "INSERT INTO Users (username, first_name, last_name, master_password) \
            VALUES (?1, ?2, ?3, ?4)",
            &[
                &self.username,
                &self.first_name,
                &self.last_name,
                &self.master_password
            ]
        ) {
            Ok(whatever) => whatever,
            Err(_) => {
                return utils::make_response(&format!(r#"
                    {{
                        "status": "error",
                        "message": "User '{}' already created."
                    }}
                    "#, self.username), &HTTP_CONFLICT
                )
            }
        };

        match tx.commit() {
            Ok(whatever) => whatever,
            Err(e) => panic!("ERROR: {e}")
        };

        utils::make_response(&format!(r#"
            {{
                "status": "ok",
                "message": "User '{}' created successfully."
            }}
            "#, self.username), &HTTP_CREATED
        )
    }

    pub fn put(&self, username: &str) -> String {
        let conn = get_connection();

        let mut stmt = match conn.prepare("SELECT * FROM Users WHERE username LIKE (?)") {
            Ok(whatever) => whatever,
            Err(e) => { panic!("ERROR: {e}") }
        };

        let db_result = match stmt.query_row(&[&username],
         |row| {
             Ok(User {
                 username: row.get(0)?,
                 first_name: row.get(1)?,
                 last_name: row.get(2)?,
                 master_password: row.get(3)?,
             })
         }) {
            Ok(whatever) => whatever,
            Err(_) => {
                return utils::make_response(&format!(r#"
                    {{
                        "status": "error",
                        "message": "The specified user does not exist."
                    }}
                    "#), &HTTP_NOT_FOUND
                )
            }
        };

        if &self.master_password != &db_result.master_password {
            return utils::make_response(&format!(r#"
                {{
                    "status": "error",
                    "message": "Incorrect password."
                }}
                "#), &HTTP_UNAUTHORIZED
            )
        }

        let mut conn = get_connection();
        let tx = match conn.transaction() {
            Ok(whatever) => whatever,
            Err(e) => panic!("ERROR: {e}")
        };

        match tx.execute(
            "UPDATE Users SET first_name = (?1), last_name = (?2) WHERE username = (?3)",
            &[
                &self.first_name,
                &self.last_name,
                &self.username
            ]
        ) {
            Ok(whatever) => whatever,
            Err(e) => panic!("ERROR: {}", e)
        };

        match tx.commit() {
            Ok(whatever) => whatever,
            Err(e) => panic!("ERROR: {e}")
        };

        utils::make_response(&format!(r#"
            {{
                "status": "ok",
                "message": "User information updated successfully."
            }}
            "#), &HTTP_OK
        )
    }

    pub fn delete(&self, username: &str) -> String {
        let conn = get_connection();

        let mut stmt = match conn.prepare("SELECT * FROM Users WHERE username LIKE (?)") {
            Ok(whatever) => whatever,
            Err(e) => { panic!("ERROR: {e}") }
        };

        let db_result = match stmt.query_row(&[&username],
         |row| {
             Ok(User {
                 username: row.get(0)?,
                 first_name: row.get(1)?,
                 last_name: row.get(2)?,
                 master_password: row.get(3)?,
             })
         }) {
            Ok(whatever) => whatever,
            Err(_) => {
                return utils::make_response(&format!(r#"
                    {{
                        "status": "error",
                        "message": "The specified user does not exist."
                    }}
                    "#), &HTTP_NOT_FOUND
                )
            }
        };

        if &self.master_password != &db_result.master_password {
            return utils::make_response(&format!(r#"
                {{
                    "status": "error",
                    "message": "Incorrect password."
                }}
                "#), &HTTP_UNAUTHORIZED
            )
        }

        let mut conn = get_connection();
        let tx = match conn.transaction() {
            Ok(whatever) => whatever,
            Err(e) => panic!("ERROR: {e}")
        };

        match tx.execute(
            " DELETE FROM Users WHERE username=(?)",
            &[username]
        ) {
            Ok(whatever) => whatever,
            Err(e) => panic!("ERROR: {e}")
        };

        match tx.commit() {
            Ok(whatever) => whatever,
            Err(e) => panic!("ERROR: {e}")
        };

        utils::make_response(&format!(r#"
            {{
                "status": "ok",
                "message": "User '{}' deleted successfully."
            }}
            "#, self.username), &HTTP_CREATED
        )
    }
}