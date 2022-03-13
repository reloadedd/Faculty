/*********************************************************************************
 *          Copyright (c) 2022 Ionut Rosca <ionut.rosca@info.uaic.ro>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

use std::collections::HashMap;
use crate::constants;
use crate::utils;
use crate::http::status_codes::{*};

#[allow(dead_code)]
#[derive(Debug)]
pub struct HttpRequest {
    pub(crate) method: String,
    pub(crate) url: String,
    pub(crate) version: String,
    pub(crate) headers: HashMap<String, String>,
    pub(crate) body: String
}

impl HttpRequest {
    pub fn new(request_data: String) -> Self {
        let request: Vec<&str> = request_data.splitn(2, "\r\n\r\n").collect();
        let request_data = request[0];

        let body: String = if request.len() == 2 {
            request[1].trim_matches(char::from(0)).to_string()
        } else {
            "".to_string()
        };

        let headers: Vec<&str> = request_data.splitn(2, "\r\n").collect();
        let status_line = headers[0];

        let s: Vec<&str> = status_line.split(" ").collect();
        println!("DEBUG\ts = {:?}", s);
        let method = s[0].to_string();  /* For instance, GET */
        let url = s[1].to_string();     /* For instance, /metrics */
        let version = s[2].to_string(); /* For instance, HTTP/1.1 */

        let header_raw_data = headers[1];
        let header_data: Vec<&str> = header_raw_data.split("\r\n").collect();
        let mut headers: HashMap<String, String> = HashMap::new();

        for header in header_data {
            let key_value: Vec<&str> = header.splitn(2,":").collect();
            headers.insert(key_value[0].to_string(), key_value[1].to_string());
        }

        HttpRequest { method, url, version, headers, body }
    }
}

#[derive(Debug)]
pub struct Router {
    pub method: String
}

impl Router {
    pub fn get(path: &str) -> String {
        utils::make_response("Hello", &HTTP_OK)
    }

    pub fn post(path: &str) -> String {
        utils::make_response("Hello", &HTTP_OK)
    }

    pub fn put(path: &str) -> String {
        utils::make_response("Hello", &HTTP_OK)
    }

    pub fn delete(path: &str) -> String {
        utils::make_response("Hello", &HTTP_OK)
    }
}

