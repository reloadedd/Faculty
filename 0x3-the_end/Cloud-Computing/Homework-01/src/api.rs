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

use aws_config::meta::region::RegionProviderChain;
use aws_sdk_secretsmanager::{Client, Error};
use aws_sdk_secretsmanager::output::GetSecretValueOutput;
use reqwest::Response;

pub async fn get_api_key_for_service(service: &str) -> Result<GetSecretValueOutput, Error> {
    let region_provider = RegionProviderChain::default_provider()
        .or_else("us-east-1");
    let config = aws_config::from_env()
        .region(region_provider)
        .load()
        .await;
    let client = Client::new(&config);

    let api_key = client
        .get_secret_value()
        .secret_id(service)
        .send()
        .await?;

    Ok(api_key)
}

pub async fn urlscan_request(api_key: &str, url: &str) -> Result<Response, reqwest::Error> {
    let client = reqwest::Client::new();

    let mut data = json::JsonValue::new_object();
    data["url"] = url.into();
    data["visibility"] = "unlisted".into();


    let result = client.post("https://urlscan.io/api/v1/scan/")
        .header("API-Key", api_key)
        .header("Content-Type", "application/json")
        .body(data.dump())
        .send()
        .await;

    result
}
