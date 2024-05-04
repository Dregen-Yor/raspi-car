use opencv::{core::*, highgui::*, imgcodecs, prelude::*, videoio};
use salvo::prelude::*;
use std::{borrow::Borrow, error::Error, io::Write, thread, time::Duration};
use 
#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let router = Router::new();
    let mut cam = videoio::VideoCapture::from_file("http://192.168.35.158:8081/0/stream", videoio::CAP_ANY).expect("无法打开摄像头");
    let mut frame = Mat::default();
    let acceptor = TcpListener::new("127.0.0.1:5800").bind().await;
    Server::new(acceptor).serve(router).await;
    Ok(())
}
