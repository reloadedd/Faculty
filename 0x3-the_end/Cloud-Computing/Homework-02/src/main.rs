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

mod constants;
mod network;
mod utils;
mod http;
mod db;

#[macro_use]
extern crate serde;
extern crate serde_json;
extern crate serde_derive;
extern crate core;

use std::net::TcpListener;
use std::io::prelude::*;
use std::thread;
use std::sync::{Arc, mpsc, Mutex};
use crate::network::Router;
use crate::http::status_codes::{*};


fn main() {
    /* Setup the database */
    db::config::setup();

    let socket = TcpListener::bind(
        format!("{}:{}", constants::IP_ADDRESS, constants::PORT)
    ).unwrap();
    println!("{}\nINFO\tServer running at http://{}:{}",
             constants::BANNER,
             constants::IP_ADDRESS,
             constants::PORT);

    let mut pool = Vec::new();

    let (sender, receiver) = mpsc::channel::<Box<dyn FnOnce() + Send>>();
    let receiver = Arc::new(Mutex::new(receiver));

    for thread_id in 0 .. constants::THREADS_COUNT {
        let receiver = Arc::clone(&receiver);

        pool.push(thread::spawn(move ||
            loop {
                let job = receiver.lock().unwrap().recv().unwrap();
                println!("INFO\tCaporalul [ {} ] va executa aceasta sarcina.", thread_id);
                job();
            })
        );
    }

    for session in socket.incoming() {
        let job = Box::new(|| {
            let mut session = session.unwrap();
            let mut buffer = [0; 2048];

            session.read(&mut buffer).unwrap();
            let request_data = String::from_utf8_lossy(&buffer);
            let request = network::HttpRequest::new(request_data.to_string());

            println!("LOG\t\tClient {} requested {} ({})",
                     session.peer_addr().unwrap(),
                     request.url,
                     request.method
            );

            let method = request.method.as_str();
            let router = Router::new(request.body);

            let response = match method {
                "GET" => router.get(&request.url),
                "POST" => router.post(&request.url),
                "PUT" => router.put(&request.url),
                "DELETE" => router.delete(&request.url),
                _ => format!("HTTP/1.1 {} {}\r\n\r\nNope.",
                             HTTP_METHOD_NOT_ALLOWED.0, HTTP_METHOD_NOT_ALLOWED.1)
            };

            session.write(response.as_bytes()).unwrap();
            session.flush().unwrap();
        });
        sender.send(job).unwrap();
    }
}