use opencv::{core::*, highgui::*, imgcodecs, prelude::*, videoio};
use salvo::prelude::*;
use std::{borrow::Borrow, error::Error, io::Write, thread, time::Duration};
fn stream(mut cam: videoio::VideoCapture) {
    println!("建立连接");
    use std::net::TcpListener;
    let mut frame = Mat::default();
    let mut buf = Vector::new();
    loop {
        // let (mut stream, _) = listener.accept().expect("无法接受连接");
        let listener = TcpListener::bind("127.0.0.1:8081").unwrap();
        for stream in listener.incoming() {
            let mut stream = stream.unwrap();
            cam.read(&mut frame).expect("捕获帧失败");
            buf.clear();
            let _ = imgcodecs::imencode(".jpg", &frame, &mut buf, &Vector::new());
            let response = format!(
            "HTTP/1.1 200 OK\r\nContent-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n"
        );
            stream.write_all(response.as_bytes()).unwrap();
            loop {
                cam.read(&mut frame).expect("捕获帧失败");
                buf.clear();
                let _ = imgcodecs::imencode(".jpg", &frame, &mut buf, &Vector::new());
                let image_data = format!(
                    "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: {}\r\n\r\n",
                    buf.len()
                );
                stream.write_all(image_data.as_bytes()).unwrap();
                stream.write_all(buf.as_slice()).unwrap();
                stream.write_all(b"\r\n").unwrap();
                stream.flush().unwrap();
            }
        }
    }
}
#[handler]
async fn video(res: &mut Response) {
    res.render(Redirect::found("127.0.0.1:8081"));
    res.add_header("Content-Type", "multipart/x-mixed-replace", true);
}
#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let router = Router::new();
    let mut cam = videoio::VideoCapture::new(0, videoio::CAP_ANY).expect("无法打开摄像头");
    thread::spawn(|| stream(cam));
    let acceptor = TcpListener::new("127.0.0.1:5800").bind().await;
    Server::new(acceptor).serve(router).await;
    Ok(())
}
