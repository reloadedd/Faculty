pub fn make_response(content: &str, status: &(i32, &str)) -> String {
    let content_type = "application/json";

    format!(
        "HTTP/1.1 {status_code} {status_message}\r\n\
        Content-Length: {content_length}\r\n\
        Content-Type: {content_type}\r\n\r\n\
        {body}",
        status_code=status.0,
        status_message=status.1,
        content_length=content.len(),
        content_type=content_type,
        body=content
    )
}
