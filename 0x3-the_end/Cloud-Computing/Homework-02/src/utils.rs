pub fn make_response(content: &str, status_code: &i32) -> String {
    let content_type = "text/json";

    format!(
        "HTTP/1.1 {status_code} OK\r\nContent-Length: {content_length}\r\nContent-Type: \
                {content_type}\r\n\r\n{body}",
        status_code=status_code,
        content_length=content.len(),
        content_type=content_type,
        body=content
    )
}
