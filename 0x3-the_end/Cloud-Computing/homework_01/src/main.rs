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

mod api;
mod constants;
mod server;

use std::net::TcpListener;
use std::io::prelude::*;
use std::{thread};
use std::sync::{Arc, mpsc, Mutex};

#[tokio::main]
async fn main() {
    let alien_vault_api_key = api::get_api_key_for_service(
        constants::AWS_SM_ALIEN_VAULT
    ).await.unwrap();
    let urlscan_api_key = api::get_api_key_for_service(
        constants::AWS_SM_URL_SCAN
    ).await.unwrap();
    let ipinfo_api_key = api::get_api_key_for_service(
        constants::AWS_SM_IP_INFO
    ).await.unwrap();

    let alien_vault_api_key = alien_vault_api_key.secret_string().unwrap();

    // println!("{}", alien_vault_api_key);

    println!("{:?}", api::urlscan_request(&alien_vault_api_key, "example.com").await);

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

        pool.push(thread::spawn(move || loop {
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
            let request = server::HttpRequest::new(request_data.to_string());

            println!("LOG\t\tClient {} requested {} ({})",
                     session.peer_addr().unwrap(),
                     request.url,
                     request.method
            );

            let response = server::route(request);
            session.write(response.as_bytes()).unwrap();
            session.flush().unwrap();
        });
        sender.send(job).unwrap();
    }
}
