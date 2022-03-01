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
use std::fs;
use std::path::Path;

use crate::constants;

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

pub fn route(request: HttpRequest) -> String {
    let mut response:String = "".to_string();

    if request.method == "GET" || request.method == "POST" {
        let filename = if request.url == "/" || request.url == "/index" {
            "index.html".to_string()
        } else {
            request.url
        };

        let path = format!("{}/{}", constants::WEBSITE_BASE_DIRECTORY, filename);
        if Path::new(&path).exists() {
            let content = fs::read_to_string(&path).unwrap();

            let mime_type = Path::new(&path).extension().unwrap().to_string_lossy();
            let mime_type = if mime_type == "js" {
                "javascript".to_string()
            } else {
                mime_type.to_string()
            };

            let content_type = format!("text/{}", mime_type);
            response = format!(
                "HTTP/1.1 200 OK\r\nContent-Length: {content_length}\r\nContent-Type: \
                {content_type}\r\n\r\n{body}",
                content_length=content.len(),
                content_type=content_type,
                body=content
            );
        } else {
            let content = fs::read_to_string(constants::NOT_FOUND_FILE).unwrap();

            response = format!(
                "HTTP/1.1 200 OK\r\nContent-Length: {content_length}\r\nContent-Type: \
                text/html\r\n\r\n{body}",
                content_length=content.len(),
                body=content
            );
        }
    } else {
        response = "HTTP/1.1 405 Method Not Allowed\r\n\r\nOnly GET is allowed!".to_string();
    }

    response
}