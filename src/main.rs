use opencv::{core::*, highgui::*, imgcodecs, prelude::*, videoio};
use salvo::prelude::*;
use std::{borrow::Borrow, error::Error, io::Write, thread, time::Duration};
use std::process::{self, Command, Stdio};
#[handler]
async fn getT(res: &mut Response){
    res.render(String::from_utf8(Command::new("./bmeT").output().expect("执行异常，提示").stdout).unwrap());
}
#[handler]
async fn getP(res: &mut Response){
    res.render(String::from_utf8(Command::new("./bmeP").output().expect("执行异常，提示").stdout).unwrap());
}
#[handler]
async fn getH(res: &mut Response){
    res.render(String::from_utf8(Command::new("./bmeH").output().expect("执行异常，提示").stdout).unwrap());
}
#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    println!("success");
    let output = Command::new("ls").output().expect("执行异常，提示");
    let out = String::from_utf8(output.stdout).unwrap();
    println!("{}", out);
    let mut router = Router::new().push(
        Router::new()
            .path("getT")
            .get(getT)
    ).push(
        Router::new()
            .path("getP")
            .get(getP)
    ).push(
        Router::new()
            .path("getH")
            .get(getH)
    ).path("/").get(getT);
    // let mut cam = videoio::VideoCapture::from_file("http://192.168.35.158:8081/0/stream", videoio::CAP_ANY).expect("无法打开摄像头");
    // let mut frame = Mat::default();
    let acceptor = TcpListener::new("127.0.0.1:5800").bind().await;
    Server::new(acceptor).serve(router).await;
    Ok(())
}
