use std::os::unix::net::{UnixListener, UnixStream};
use std::io::{Read, Write};

fn main() -> std::io::Result<()> {
    let frontend_path = "../tmp/frontend_pyroxene.sock";
    let listener = UnixListener::bind(frontend_path)?;

    for stream in listener.incoming() {
        match stream {
            Ok(mut stream) => {
                let mut buffer = [0u8; 1024];
                let bytes_read = stream.read(&mut buffer)?;

                if bytes_read > 0 {
                    let data = &buffer[..bytes_read];

                    if let Err(e) = {
                        let mut stream = UnixStream::connect("../tmp/backend_pyroxene.sock")?;
                        stream.write_all(data)?;
                        Ok::<(), std::io::Error>(())
                    } {
                        eprintln!("Failed to send to compiler backend socket: {}", e);
                    }
                    break;
                }
            }
            Err(e) => eprintln!("Rust socket creation error: {}", e),
        }
    }

    Ok(())
}
