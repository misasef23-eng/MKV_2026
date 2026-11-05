// Chuyển ảnh <-> chuỗi hex (Rust, chỉ dùng thư viện chuẩn)
//
// Biên dịch:
//     rustc -O image_hex.rs
//
// Cách dùng:
//     ./image_hex encode anh.png anh.txt
//     ./image_hex encode anh.png anh.txt --width 32   (mỗi dòng 32 byte)
//     ./image_hex encode anh.png                      (in ra màn hình)
//     ./image_hex decode anh.txt anh_moi.png
//     ./image_hex decode anh.txt                      (tự đặt tên + đoán đuôi file)

use std::env;
use std::fs;
use std::path::Path;
use std::process;

// ---------- Ảnh -> hex ----------
// width = số byte mỗi dòng (0 = tất cả trên 1 dòng)
fn bytes_to_hex(data: &[u8], width: usize) -> String {
    const DIGITS: &[u8; 16] = b"0123456789abcdef";
    let mut out = String::with_capacity(data.len() * 2 + if width > 0 { data.len() / width } else { 0 });
    for (i, &b) in data.iter().enumerate() {
        out.push(DIGITS[(b >> 4) as usize] as char);
        out.push(DIGITS[(b & 0x0F) as usize] as char);
        if width > 0 && (i + 1) % width == 0 && i + 1 != data.len() {
            out.push('\n');
        }
    }
    out
}

// ---------- Hex -> bytes ----------
fn hex_value(c: u8) -> Option<u8> {
    match c {
        b'0'..=b'9' => Some(c - b'0'),
        b'a'..=b'f' => Some(c - b'a' + 10),
        b'A'..=b'F' => Some(c - b'A' + 10),
        _ => None,
    }
}

fn hex_to_bytes(text: &str) -> Result<Vec<u8>, String> {
    let bytes = text.as_bytes();
    let mut out = Vec::with_capacity(bytes.len() / 2);
    let mut high: Option<u8> = None; // nửa byte cao đang chờ nửa thấp
    let mut i = 0;

    while i < bytes.len() {
        let c = bytes[i];
        // Bỏ khoảng trắng, xuống dòng, dấu phẩy
        if c.is_ascii_whitespace() || c == b',' {
            i += 1;
            continue;
        }
        // Bỏ tiền tố 0x / 0X
        if c == b'0' && i + 1 < bytes.len() && (bytes[i + 1] == b'x' || bytes[i + 1] == b'X') {
            i += 2;
            continue;
        }
        let v = hex_value(c).ok_or_else(|| format!("Ký tự hex không hợp lệ: '{}'", c as char))?;
        match high.take() {
            None => high = Some(v),
            Some(h) => out.push((h << 4) | v),
        }
        i += 1;
    }

    if high.is_some() {
        return Err("Chuỗi hex có số ký tự lẻ - dữ liệu bị thiếu hoặc hỏng.".to_string());
    }
    Ok(out)
}

// ---------- Đoán đuôi file từ magic bytes ----------
fn detect_extension(d: &[u8]) -> &'static str {
    if d.starts_with(b"\x89PNG\r\n\x1a\n") {
        ".png"
    } else if d.starts_with(b"\xff\xd8\xff") {
        ".jpg"
    } else if d.starts_with(b"GIF87a") || d.starts_with(b"GIF89a") {
        ".gif"
    } else if d.starts_with(b"BM") {
        ".bmp"
    } else if d.len() >= 12 && &d[0..4] == b"RIFF" && &d[8..12] == b"WEBP" {
        ".webp"
    } else if d.starts_with(b"II*\x00") || d.starts_with(b"MM\x00*") {
        ".tiff"
    } else if d.starts_with(b"\x00\x00\x01\x00") {
        ".ico"
    } else {
        ".bin"
    }
}

// Bỏ đuôi file: "a/b/anh.txt" -> "a/b/anh"
fn strip_extension(path: &str) -> String {
    let p = Path::new(path);
    match (p.parent(), p.file_stem()) {
        (Some(parent), Some(stem)) => parent.join(stem).to_string_lossy().into_owned(),
        _ => path.to_string(),
    }
}

// ---------- main ----------
fn usage(prog: &str) {
    eprintln!("Cách dùng:");
    eprintln!("  {} encode <anh> [file_hex] [--width N]", prog);
    eprintln!("  {} decode <file_hex> [anh_ra]", prog);
}

fn run(args: &[String]) -> Result<(), String> {
    if args.len() < 3 {
        usage(&args[0]);
        return Err(String::new());
    }

    let cmd = args[1].as_str();
    let mut pos: Vec<&str> = Vec::new(); // tham số vị trí
    let mut width: usize = 0;

    let mut i = 2;
    while i < args.len() {
        if args[i] == "--width" {
            i += 1;
            width = args
                .get(i)
                .and_then(|s| s.parse().ok())
                .ok_or("--width cần một con số hợp lệ.")?;
        } else {
            pos.push(&args[i]);
        }
        i += 1;
    }

    match cmd {
        "encode" if !pos.is_empty() => {
            let data = fs::read(pos[0]).map_err(|e| format!("Không đọc được '{}': {}", pos[0], e))?;
            let hex = bytes_to_hex(&data, width);
            if pos.len() >= 2 {
                fs::write(pos[1], &hex).map_err(|e| format!("Không ghi được '{}': {}", pos[1], e))?;
                println!("Đã ghi hex của {} byte vào: {}", data.len(), pos[1]);
            } else {
                println!("{}", hex);
            }
            Ok(())
        }
        "decode" if !pos.is_empty() => {
            let text = fs::read_to_string(pos[0]).map_err(|e| format!("Không đọc được '{}': {}", pos[0], e))?;
            let data = hex_to_bytes(&text)?;
            let out = if pos.len() >= 2 {
                pos[1].to_string()
            } else {
                format!("{}_decoded{}", strip_extension(pos[0]), detect_extension(&data))
            };
            fs::write(&out, &data).map_err(|e| format!("Không ghi được '{}': {}", out, e))?;
            println!("Đã khôi phục ảnh ({} byte) vào: {}", data.len(), out);
            Ok(())
        }
        _ => {
            usage(&args[0]);
            Err(String::new())
        }
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if let Err(msg) = run(&args) {
        if !msg.is_empty() {
            eprintln!("Lỗi: {}", msg);
        }
        process::exit(1);
    }
}
