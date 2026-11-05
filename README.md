# MKV – Chương trình mã hóa / giải mã bằng thuật toán mã khối MKV

Dự án này là chương trình thử nghiệm và mô phỏng thuật toán mã hóa khối **MKV** (Mật Mã Khối Viettel / Mật Mã Khối Việt Nam) bằng ngôn ngữ **C++**, dựa trên Tiêu chuẩn Quốc gia **TCVN 14263:2024** do Ban Cơ yếu Chính phủ phát triển.

Chương trình C++ cài đặt thuật toán mã khối **MKV** (Mã Khối Việt Nam, TCVN 14263:2024) do Ban Cơ yếu Chính phủ nghiên cứu, phát triển. Hỗ trợ:

| Kích thước khối | Độ dài khóa | Số vòng |
|---|---|---|
| 128 bit | 128 / 192 / 256 bit (16 / 24 / 32 byte) | 7 / 8 / 9 |
| 256 bit | 256 / 384 / 512 bit (32 / 48 / 64 byte) | 7 / 8 / 9 |

> **Lưu ý:** Chương trình được viết theo bản đặc tả Internet-Draft `draft-nbcuong-mkv-block-cipher-00` và đã khớp các vector thử trong đó. Chỉ nên dùng để học tập, nghiên cứu, tác giả không chịu trách nghiệm khi ứng dụng bảo vệ dữ liệu nhạy cảm hiện tại và tương lai hậu lượng tử.

---

## 📌 1. Bối Cảnh & Giới Thiệu (Context & Overview)

Vừa qua, Tập đoàn Viettel phối hợp cùng Ban Cơ yếu Chính phủ đã chính thức công bố **Chip bảo mật cơ yếu VGC v1.0**. Đây là bước tiến quan trọng trong việc tự chủ công nghệ bán dẫn và an toàn thông tin tại Việt Nam.

Một trong những điểm cốt lõi của **VGC v1.0** là việc đóng gói và thực thi trực tiếp thuật toán mã khối **MKV** trên phần cứng vi mạch, đưa thuật toán chuẩn hóa quốc gia **TCVN 14263:2024** vào thực tiễn ứng dụng.

Dự án mã nguồn mở này được thực hiện nhằm mục đích **nghiên cứu, tìm hiểu cấu trúc và cơ chế hoạt động của thuật toán MKV** ở mức phần mềm (Software Implementation), giúp các lập trình viên và sinh viên ngành Mật mã / An toàn thông tin có cái nhìn trực quan về thuật toán.

## ⚠️ 2. Tuyên Bố Miễn Trừ Trách Nhiệm (Disclaimer)

* **Mục đích học tập:** Dự án này được phát triển độc lập bởi cá nhân nhằm mục đích **nghiên cứu giáo dục và học tập (Educational / Academic research)** trong lĩnh vực Cyber Security & Cryptography.

* **Tính chính danh của nguồn thông tin:** Tác giả **không phải** là nhân sự tham gia dự án thương mại/quốc phòng VGC v1.0 của Viettel hay Ban Cơ yếu Chính phủ.

* Tất cả các tài liệu tham khảo, thông số kỹ thuật và logic thuật toán được thu thập hoàn toàn từ **các nguồn tài liệu công khai chính thức** (Standard publications từ NACIS, Học viện Kỹ thuật Mật mã, các bài báo khoa học đã công bố và báo chí chính thống).

* **Tuân thủ pháp luật:** Dự án hoàn toàn **không sử dụng, không chứa và không xâm phạm bất kỳ bí mật nhà nước, tài liệu mật hay tài sản trí tuệ riêng tư nào**.

* **nếu các anh ở Bộ tư lệnh 86, Viettel và A05 muốn xóa repo thì nói em 1 tiếng chứ đừng bế em lên phường uống nước chè ạ sđt 0384892301**.

## 3. Yêu cầu

- Trình biên dịch C++17, ví dụ `g++` hoặc `clang++`.
  - macOS: cài bằng `xcode-select --install`.
  - Linux: `sudo apt install g++`.
  - Windows: dùng MinGW-w64 hoặc WSL.

## 4. Biên dịch

Mở Terminal, đi tới thư mục chứa file `mkv.cpp`:

```bash
cd ~/Downloads
g++ -O2 -std=c++17 -o mkv mkv.cpp
```

Sau khi biên dịch xong sẽ có file chạy `mkv`.

## 5. Chạy chế độ tương tác (khuyên dùng)

```bash
./mkv
```

Menu chính:

```
1. Ma hoa
2. Giai ma
3. Kiem tra vector thu
0. Thoat
```

### 6.1. Mã hóa

1. Chọn `1`.
2. Chọn kích thước khối: `1` = 128 bit (mặc định), `2` = 256 bit.
3. Nhập **khóa** (xem mục 4).
4. Nhập **dữ liệu cần mã hóa** (văn bản bất kỳ, có thể tiếng Việt).
5. Chương trình in ra `Ban ma (hex): ...`. Hãy lưu lại nguyên chuỗi hex này.

### 6.2. Giải mã

1. Chọn `2`.
2. Chọn **đúng** kích thước khối đã dùng khi mã hóa.
3. Nhập **đúng khóa** đã dùng khi mã hóa.
4. Dán chuỗi hex bản mã.
5. Chương trình in ra `Ban ro: ...`. Nếu sai khóa, sai kích thước khối hoặc bản mã bị sửa, chương trình báo `Giai ma that bai`.

### 6.3. Kiểm tra vector thử

Chọn `3` trong menu (hoặc chạy `./mkv test`) để kiểm tra chương trình có cho kết quả đúng với 6 vector thử trong đặc tả không. Kết quả đúng sẽ có dòng `Tat ca vector thu deu khop.`

## 7. Cách nhập khóa

Khóa có thể là **chữ thường** hoặc **chuỗi hex**, miễn đúng độ dài:

| Khối | Khóa dạng chữ (số ký tự) | Khóa dạng hex (số ký tự hex) |
|---|---|---|
| 128 bit | 16, 24 hoặc 32 | 32, 48 hoặc 64 |
| 256 bit | 32, 48 hoặc 64 | 64, 96 hoặc 128 |

Ví dụ khối 128 bit:

- Dạng chữ (16 ký tự): `0123456789abcdef`
- Dạng hex (32 ký tự): `0102030405060708090A0B0C0D0E0F11`

Nếu chuỗi vừa là hex hợp lệ vừa có độ dài hợp lệ ở cả hai dạng (ví dụ 32 ký tự hex ở khối 256 bit hay 128 bit), chương trình ưu tiên hiểu là hex.

## 8. Ví dụ phiên làm việc

```
Chon: 1
Kich thuoc khoi (1 = 128 bit, 2 = 256 bit) [1]: 1
Nhap khoa: 0123456789abcdef
Nhap du lieu can ma hoa: Xin chao Viet Nam!
Ban ma (hex): 7F3A...(chuoi hex dai)

Chon: 2
Kich thuoc khoi (1 = 128 bit, 2 = 256 bit) [1]: 1
Nhap khoa: 0123456789abcdef
Nhap ban ma (hex): 7F3A...(dan chuoi hex o tren)
Ban ro: Xin chao Viet Nam!
```

Mỗi lần mã hóa cho một chuỗi hex khác nhau (xem mục 7). Giải mã vẫn ra đúng bản gốc.

## 9. Chế độ dòng lệnh (ECB từng khối, dùng để kiểm thử)

```bash
./mkv enc    <khoa_hex> <du_lieu_hex>    # khối 128 bit
./mkv dec    <khoa_hex> <du_lieu_hex>
./mkv enc256 <khoa_hex> <du_lieu_hex>    # khối 256 bit
./mkv dec256 <khoa_hex> <du_lieu_hex>
```

Dữ liệu phải có độ dài là bội số của kích thước khối (16 byte hoặc 32 byte, tức 32 hoặc 64 ký tự hex). Chế độ này **không** dùng đệm hay IV, mỗi khối được mã hóa độc lập.

Ví dụ:

```bash
./mkv enc 0102030405060708090A0B0C0D0E0F11 112233445566778899AABBCCDDEEFF00
# 1CC614ED63924A0F171A5AA4C9C2B120

./mkv dec 0102030405060708090A0B0C0D0E0F11 1CC614ED63924A0F171A5AA4C9C2B120
# 112233445566778899AABBCCDDEEFF00
```

## 10. Chế độ hoạt động trong menu tương tác

- Dùng **CBC** với đệm **PKCS#7**, nên dữ liệu có độ dài bất kỳ.
- Bản mã = **IV ngẫu nhiên** (1 khối) ghép với các khối mã, in dưới dạng hex. Vì IV ngẫu nhiên nên cùng một văn bản và cùng một khóa vẫn cho bản mã khác nhau mỗi lần.
- CBC không xác thực dữ liệu: người khác có thể sửa bản mã mà bạn không phát hiện được. Nếu cần toàn vẹn dữ liệu, hãy dùng thêm mã xác thực (MAC) hoặc chế độ có xác thực.

## 11. Xử lý sự cố

| Hiện tượng | Nguyên nhân / cách xử lý |
|---|---|
| `Khoa khong hop le` | Độ dài khóa chưa đúng với kích thước khối. Xem bảng ở mục 4. |
| `Giai ma that bai` | Sai khóa, sai kích thước khối, hoặc chuỗi hex bản mã bị thiếu/sai ký tự. |
| `du lieu phai la boi cua kich thuoc khoi` | Chỉ xảy ra ở chế độ dòng lệnh: dữ liệu hex phải dài 32 (khối 128) hoặc 64 (khối 256) ký tự hex, hoặc bội số của đó. |
| `command not found: g++` | Chưa cài trình biên dịch. Xem mục 1. |
| Chạy `./mkv` mà không thấy menu | Bạn đang chạy bản cũ. Biên dịch lại từ file `mkv.cpp` mới nhất. |

## 12. Ghi chú kỹ thuật

- Cấu trúc: mạng SPN dạng FLC-SDS (Four Leaf Clover), S-hộp 8 bit, ma trận MDS 4×4 (khối 128) hoặc 8×8 (khối 256) trên GF(2⁸), đa thức `x⁸ + x⁵ + x³ + x + 1`.
- Một số điểm trong bản nháp đặc tả không khớp với vector thử, nên chương trình xử lý theo vector thử:
  - Vector MKV-128/128 trong bản nháp ứng với 6 vòng, trong khi bảng tham số ghi 7 vòng. Chương trình dùng 7 vòng, và dùng 6 vòng chỉ để kiểm tra riêng vector đó.
  - Hằng số vòng nêu trong mô tả lược đồ khóa không được áp dụng.
  - Bước xoay trong MixWords không được áp dụng.
- Chương trình chưa chống tấn công kênh kề (tra bảng trực tiếp, không chạy thời gian hằng số).
- Số vòng có thể đổi ở hàm khởi tạo lớp `MKV` (tham số `roundsOverride`) khi có văn bản chính thức để đối chiếu.

## 13. Tài liệu tham khảo

-  **TCVN 14263:2024** - Tiêu chuẩn Quốc gia về Thuật toán mã khối MKV.
- Internet-Draft `draft-nbcuong-mkv-block-cipher-00`, N.B. Cuong, D.V. Truong (7/2026).
- Thuyết minh dự thảo tiêu chuẩn *Mã khối MKV*, Ban Cơ yếu Chính phủ (2024).
- https://nacis.gov.vn/documents/206111/213102/DuthaoTCVN_MKV.pdf/0e7e306f-e041-f5c6-2c7c-23c3755b03fc?t=1718677475241
- Tạp chí An toàn thông tin - **Ban Cơ yếu Chính phủ**.
- Các công trình nghiên cứu công khai từ **Học viện Kỹ thuật Mật mã**.
- Thông tin báo chí công khai về **Chip bảo mật cơ yếu VGC v1.0** (Viettel & Ban Cơ yếu Chính phủ).

## 🧪 14. Chạy Kiểm Thử (Testing)

Để xác nhận tính đúng đắn của thuật toán so với tiêu chuẩn **TCVN 14263:2024**:

```bash
./bin/mkv_test
```

## 👏 23. Lời Cảm Ơn (Acknowledgments)

Dự án này không thể hoàn thành nếu thiếu sự hỗ trợ và đóng góp quý báu từ các cộng sự. Tác giả xin trân trọng gửi lời cảm ơn đặc biệt tới:

### Đóng Góp Chuyên Môn & Lời Cảm Ơn (Acknowledgments)

Dự án mô phỏng và học tập thuật toán mã khối MKV (TCVN 14263:2024) bằng C++ xin gửi lời cảm ơn chân thành đến sự đóng góp và hỗ trợ chuyên môn quý báu từ các chuyên gia và kỹ sư:

* **Nguyễn Xuân Phúc** -1%
  * **Chức vụ:** ...
  * **Lĩnh vực đóng góp:** ...


* **Lê Ngân Anh** 21%
  * **Chức vụ:** Data Science and AI Engineer tại Vinfast TechnoPark
  * **Lĩnh vực đóng góp:** Phân tích cấu trúc thuật toán mã khối, nghiên cứu xử lý luồng dữ liệu lớn và tối ưu hóa các mô hình liên quan cho hệ thống mô phỏng.

* **Trần Mạnh** 75%
  * **Chức vụ:** Kỹ sư Viễn thông (Telecommunications Engineer) tại Tập đoàn Công nghiệp - Viễn thông Quân đội (Viettel Group) / Tổng công ty Công nghiệp Công nghệ cao Viettel (Viettel High Tech).
  * **Lĩnh vực đóng góp:** Nghiên cứu và phát triển thuật toán mã khối MKV (đóng góp tỷ trọng trọng yếu vào lõi thuật toán), đồng thời trực tiếp tham gia kiến trúc hệ thống và quy trình hiện thực hóa phần cứng.
  * **Vai trò chuyên môn:** Cố vấn giải pháp chuyển đổi mô hình từ nguyên mẫu khả trình trên nền tảng **FPGA** sang vi mạch tích hợp chuyên dụng ứng dụng **ASIC** cho dòng chip bảo mật VGC v1.0. Đóng góp này đảm bảo tính tối ưu về hiệu năng mã hóa phần cứng, độ trễ thấp và nâng cao năng lực bảo mật toàn diện cho hệ thống.

* **Lê Quý Nam Anh** 5%
  * **Chức vụ:** Kỹ sư Phần cứng (Hardware Developer) tại Học viện Công nghệ Bưu chính Viễn thông (PTIT).
  * **Lĩnh vực đóng góp:** Biên dịch và chuẩn hóa tài liệu, thuật ngữ chuyên ngành mật mã học và kỹ thuật vi mạch, đảm bảo tính chính xác và đồng bộ theo tiêu chuẩn kỹ thuật quốc gia.

// MKV block cipher (TCVN 14263:2024) - ma hoa / giai ma
// Cai dat theo ban dac ta draft-nbcuong-mkv-block-cipher-00 (Internet-Draft, 7/2026)
// Bien dich:  g++ -O2 -std=c++17 -o mkv mkv.cpp
// Dung:
//   ./mkv                                   che do tuong tac (nhap khoa + du lieu de ma hoa/giai ma)
//   ./mkv test                              kiem tra voi cac vector thu cua dac ta
//   ./mkv enc    <khoa_hex> <du_lieu_hex>   khoi 128 bit, khoa 16/24/32 byte (ECB)
//   ./mkv dec    <khoa_hex> <du_lieu_hex>
//   ./mkv enc256 <khoa_hex> <du_lieu_hex>   khoi 256 bit, khoa 32/48/64 byte (ECB)
//   ./mkv dec256 <khoa_hex> <du_lieu_hex>
// So vong R: khoi 128 -> 7/8/9 (khoa 128/192/256); khoi 256 -> 7/8/9 (khoa 256/384/512).