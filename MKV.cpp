// MKV Block Cipher (TCVN 14263:2024) - Chương trình mã hóa và giải mã
//
// Chương trình được cài đặt dựa trên bản đặc tả dự thảo:
// draft-nbcuong-mkv-block-cipher-00 (Internet-Draft, 07/2026).
//
// Cách biên dịch:
//   g++ -O2 -std=c++17 -o mkv mkv.cpp
//
// Cách sử dụng:
//
//   ./mkv
//       Chạy chương trình ở chế độ tương tác.
//       Người dùng nhập khóa và dữ liệu để thực hiện mã hóa hoặc giải mã.
//
//   ./mkv test
//       Chạy kiểm thử bằng các bộ vector thử nghiệm trong bản đặc tả.
//
//   ./mkv enc <khoa_hex> <du_lieu_hex>
//       Mã hóa dữ liệu với kích thước khối 128 bit.
//       Hỗ trợ khóa có độ dài 16, 24 hoặc 32 byte (chế độ ECB).
//
//   ./mkv dec <khoa_hex> <du_lieu_hex>
//       Giải mã dữ liệu với kích thước khối 128 bit.
//
//   ./mkv enc256 <khoa_hex> <du_lieu_hex>
//       Mã hóa dữ liệu với kích thước khối 256 bit.
//       Hỗ trợ khóa có độ dài 32, 48 hoặc 64 byte (chế độ ECB).
//
//   ./mkv dec256 <khoa_hex> <du_lieu_hex>
//       Giải mã dữ liệu với kích thước khối 256 bit.
//
// Số vòng mã hóa (R):
//   - Khối 128 bit: 7 / 8 / 9 vòng tương ứng với khóa 128 / 192 / 256 bit.
//   - Khối 256 bit: 7 / 8 / 9 vòng tương ứng với khóa 256 / 384 / 512 bit.
//
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <random>

using Bytes = std::vector<uint8_t>;

// ---------------------------------------------------------------- GF(2^8)
// Da thuc toi thieu x^8 + x^5 + x^3 + x + 1 (0x12B)
static uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    while (b) {
        if (b & 1) p ^= a;
        bool hi = a & 0x80;
        a <<= 1;
        if (hi) a ^= 0x2B;
        b >>= 1;
    }
    return p;
}
static uint8_t ginv(uint8_t a) {
    for (int b = 1; b < 256; ++b) if (gmul(a, (uint8_t)b) == 1) return (uint8_t)b;
    return 0;
}

// ---------------------------------------------------------------- S-hop (Table 1)
static const uint8_t SBOX[256] = {
0x01,0x11,0x91,0xE1,0xD1,0xB1,0x71,0x61,0xF1,0x21,0xC1,0x51,0xA1,0x41,0x31,0x81,
0x00,0x10,0xE3,0x92,0xB5,0xD4,0x77,0x66,0x89,0x38,0xAB,0x4A,0xCD,0x5C,0x2F,0xFE,
0x08,0x5F,0x3E,0xB0,0x1C,0xC2,0x83,0xDD,0xE8,0xF6,0x47,0x79,0x95,0x2B,0xAA,0x64,
0x0F,0x48,0xD0,0x29,0xA3,0x1A,0xF2,0xBB,0x65,0xCC,0xE4,0x3D,0x57,0x7E,0x86,0x9F,
0x0C,0x2A,0xF4,0x1F,0x5B,0x90,0xEE,0xC5,0x36,0x6D,0x73,0x88,0xBC,0xA7,0x49,0xD2,
0x0A,0x3C,0x18,0x85,0xE0,0x4D,0x99,0xA4,0xB3,0x5E,0xDA,0xC7,0x72,0xFF,0x6B,0x26,
0x06,0x76,0xCF,0xA8,0x4E,0x59,0x60,0x17,0xDC,0x9B,0x32,0xF5,0x23,0x84,0xED,0xBA,
0x07,0x67,0x2D,0x3B,0xFA,0x8C,0x16,0x70,0x54,0xA2,0x98,0xBE,0xEF,0xD9,0xC3,0x45,
0x0E,0xA9,0x62,0x5A,0x27,0xBF,0x34,0x9C,0xFD,0xD5,0x8E,0xE6,0x1B,0x43,0x78,0xC0,
0x03,0xB2,0x87,0xC4,0x9D,0x6E,0x4B,0xF8,0x7A,0xE9,0x2C,0xAF,0xD6,0x15,0x50,0x33,
0x0D,0xFB,0x56,0xEC,0x3F,0x75,0xB8,0x42,0x1E,0x24,0xC9,0x93,0x80,0x6A,0xD7,0xAD,
0x04,0xE5,0xB9,0x7D,0x82,0xA6,0xCA,0x2E,0x97,0x13,0x6F,0xDB,0x44,0x30,0xFC,0x58,
0x0B,0x8D,0x9A,0x46,0x74,0x28,0xDF,0x53,0xCB,0xB7,0xF0,0x6C,0xAE,0xE2,0x35,0x19,
0x05,0x94,0x7B,0xDE,0xC6,0xF3,0xAC,0x39,0x4F,0x8A,0x55,0x20,0x68,0xBD,0x12,0xE7,
0x02,0xD3,0xA5,0xF7,0x69,0xEB,0x5D,0x8F,0x22,0x40,0xB6,0x14,0x3A,0xC8,0x9E,0x7C,
0x09,0xCE,0x4C,0x63,0xD8,0x37,0x25,0xEA,0xA0,0x7F,0x1D,0x52,0xF9,0x96,0xB4,0x8B};
static uint8_t INV_SBOX[256];

// ---------------------------------------------------------------- Tham so cua tung phien ban
struct Params {
    int n;      // kich thuoc khoi (bit)
    int t;      // so byte moi trang thai con (w/8)
    int keyLen; // do dai khoa (byte)
    int R;      // so vong
};

// Ma tran MDS M = A^t, A la ma tran dong hanh (companion) tren GF(2^8)
using Mat = std::vector<std::vector<uint8_t>>;

static Mat matMul(const Mat& A, const Mat& B) {
    size_t n = A.size();
    Mat C(n, std::vector<uint8_t>(n, 0));
    for (size_t i = 0; i < n; ++i)
        for (size_t k = 0; k < n; ++k)
            for (size_t j = 0; j < n; ++j) C[i][j] ^= gmul(A[i][k], B[k][j]);
    return C;
}
static Mat companion(int t) {
    Mat A(t, std::vector<uint8_t>(t, 0));
    for (int i = 0; i + 1 < t; ++i) A[i][i + 1] = 1;
    if (t == 4) { A[3] = {0x01, 0x02, 0x01, 0x03}; }
    else        { A[7] = {0x01, 0x04, 0xDB, 0x0C, 0x14, 0x0C, 0xDB, 0x04}; }
    return A;
}
static Mat matInverse(Mat M) {
    size_t n = M.size();
    Mat I(n, std::vector<uint8_t>(n, 0));
    for (size_t i = 0; i < n; ++i) I[i][i] = 1;
    for (size_t c = 0; c < n; ++c) {
        size_t p = c;
        while (p < n && M[p][c] == 0) ++p;
        if (p == n) throw std::runtime_error("ma tran suy bien");
        std::swap(M[p], M[c]); std::swap(I[p], I[c]);
        uint8_t iv = ginv(M[c][c]);
        for (size_t j = 0; j < n; ++j) { M[c][j] = gmul(M[c][j], iv); I[c][j] = gmul(I[c][j], iv); }
        for (size_t r = 0; r < n; ++r) if (r != c && M[r][c]) {
            uint8_t f = M[r][c];
            for (size_t j = 0; j < n; ++j) { M[r][j] ^= gmul(f, M[c][j]); I[r][j] ^= gmul(f, I[c][j]); }
        }
    }
    return I;
}

// ---------------------------------------------------------------- Tien ich hex
static Bytes fromHex(const std::string& s) {
    Bytes out; int hi = -1;
    for (char ch : s) {
        int v;
        if (ch >= '0' && ch <= '9') v = ch - '0';
        else if (ch >= 'a' && ch <= 'f') v = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F') v = ch - 'A' + 10;
        else continue;                       // bo qua khoang trang, dau cach
        if (hi < 0) hi = v; else { out.push_back((uint8_t)(hi << 4 | v)); hi = -1; }
    }
    if (hi >= 0) throw std::runtime_error("chuoi hex co so ky tu le");
    return out;
}
static std::string toHex(const Bytes& b) {
    static const char* d = "0123456789ABCDEF"; std::string s;
    for (uint8_t x : b) { s += d[x >> 4]; s += d[x & 15]; }
    return s;
}

// ---------------------------------------------------------------- Lop MKV
class MKV {
public:
    // blockBits: 128 hoac 256; key: 16/24/32 byte (khoi 128) hoac 32/48/64 byte (khoi 256)
    MKV(int blockBits, const Bytes& key, int roundsOverride = 0) {
        for (int i = 0; i < 256; ++i) INV_SBOX[SBOX[i]] = (uint8_t)i;
        p_.n = blockBits; p_.t = blockBits / 32; p_.keyLen = (int)key.size();
        int nb = blockBits / 8;
        if (blockBits == 128 && (key.size() == 16 || key.size() == 24 || key.size() == 32)) {
            p_.R = key.size() == 16 ? 7 : key.size() == 24 ? 8 : 9;
        } else if (blockBits == 256 && (key.size() == 32 || key.size() == 48 || key.size() == 64)) {
            p_.R = key.size() == 32 ? 7 : key.size() == 48 ? 8 : 9;
        } else throw std::runtime_error("kich thuoc khoi / do dai khoa khong hop le");
        (void)nb;
        if (roundsOverride > 0) p_.R = roundsOverride;
        M_ = matPow(p_.t);
        Minv_ = matInverse(M_);
        keySchedule(key);
    }
    int blockBytes() const { return p_.n / 8; }

    Bytes encrypt(const Bytes& P) const {
        check(P);
        Bytes X = P;
        for (int r = 0; r < p_.R; ++r) {
            xorInto(X, rk_[2 * r]);            // K_r^0
            subCells(X);
            mixWords(X, M_);
            xorInto(X, rk_[2 * r + 1]);        // K_r^1
            subCells(X);
            xWords(X);
        }
        xorInto(X, rk_[2 * p_.R]);             // K_post
        return X;
    }
    Bytes decrypt(const Bytes& C) const {
        check(C);
        Bytes X = C;
        xorInto(X, rk_[2 * p_.R]);
        for (int r = p_.R - 1; r >= 0; --r) {
            xWords(X);                          // tu nghich dao
            invSubCells(X);
            xorInto(X, rk_[2 * r + 1]);
            mixWords(X, Minv_);
            invSubCells(X);
            xorInto(X, rk_[2 * r]);
        }
        return X;
    }

private:
    Params p_;
    Mat M_, Minv_;
    std::vector<Bytes> rk_;   // RK_0 ... RK_2R (moi phan tu n bit)

    static Mat matPow(int t) {
        Mat A = companion(t), M = A;
        for (int i = 1; i < t; ++i) M = matMul(M, A);   // A^t
        return M;
    }
    void check(const Bytes& b) const {
        if ((int)b.size() != blockBytes()) throw std::runtime_error("sai kich thuoc khoi");
    }
    static void xorInto(Bytes& a, const Bytes& b) { for (size_t i = 0; i < a.size(); ++i) a[i] ^= b[i]; }
    static void subCells(Bytes& X)    { for (auto& x : X) x = SBOX[x]; }
    static void invSubCells(Bytes& X) { for (auto& x : X) x = INV_SBOX[x]; }

    // MixWords: nhan ma tran voi tung trang thai con (t byte)
    void mixWords(Bytes& X, const Mat& M) const {
        int t = p_.t;
        Bytes Y(X.size());
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < t; ++j) {
                uint8_t acc = 0;
                for (int k = 0; k < t; ++k) acc ^= gmul(M[j][k], X[i * t + k]);
                Y[i * t + j] = acc;
            }
        X.swap(Y);
    }
    // XWords: Y_i = XOR cua cac trang thai con con lai
    void xWords(Bytes& X) const {
        int t = p_.t;
        Bytes Y(X.size());
        for (int j = 0; j < t; ++j) {
            uint8_t a = X[j], b = X[t + j], c = X[2 * t + j], d = X[3 * t + j];
            Y[j] = b ^ c ^ d; Y[t + j] = a ^ c ^ d; Y[2 * t + j] = a ^ b ^ d; Y[3 * t + j] = a ^ b ^ c;
        }
        X.swap(Y);
    }
    // Ham cap nhat tren mot trang thai n bit: XWord(SubCells(MixWords(SubCells(.))))
    void keyF(Bytes& X) const { subCells(X); mixWords(X, M_); subCells(X); xWords(X); }

    // Bo dem (2i+1)_n, (2i+2)_n: so nguyen n bit, byte cao truoc (big-endian)
    static void addCounter(Bytes& X, uint32_t c) {
        X[X.size() - 1] ^= (uint8_t)c; X[X.size() - 2] ^= (uint8_t)(c >> 8);
    }

    void keySchedule(const Bytes& key) {
        int nb = p_.n / 8, w = nb / 4;     // w: so byte moi tu khoa
        Bytes K(2 * nb);
        auto neg = [&](size_t i) { return (uint8_t)~key[i]; };
        if ((int)key.size() == 2 * nb) {                       // khoa 2n bit
            std::copy(key.begin(), key.end(), K.begin());
        } else if ((int)key.size() == 3 * nb / 2) {            // khoa 3n/2 bit: k0..k5 || ~k2 || ~k3
            std::copy(key.begin(), key.end(), K.begin());
            for (int i = 0; i < 2 * w; ++i) K[6 * w + i] = neg(2 * w + i);
        } else {                                               // khoa n bit: k || ~k
            std::copy(key.begin(), key.end(), K.begin());
            for (int i = 0; i < nb; ++i) K[nb + i] = neg(i);
        }
        Bytes K0(K.begin(), K.begin() + nb), K1(K.begin() + nb, K.end());

        rk_.assign(2 * p_.R + 1, Bytes());
        rk_[0] = K0;
        for (int i = 0; i < p_.R; ++i) {
            Bytes t1 = K1; addCounter(t1, 2 * i + 2); keyF(t1);  // (3.1)
            keyF(t1);                                                              // (3.2)
            K1 = t1;
            Bytes t0 = K0; addCounter(t0, 2 * i + 1); keyF(t0);  // (3.3)
            keyF(t0); xorInto(t0, K1);                                             // (3.4)
            K0 = t0;
            std::swap(K0, K1);                                                     // (3.5)
            rk_[2 * i + 1] = K0;                                                   // (3.6)
            rk_[2 * i + 2] = K1;                                                   // (3.7)
        }
    }
};

// ---------------------------------------------------------------- Kiem tra bang vector thu
static bool testVec(int bits, const char* key, const char* pt, const char* ct, int rounds = 0) {
    MKV m(bits, fromHex(key), rounds);
    Bytes P = fromHex(pt), C = m.encrypt(P);
    bool ok = toHex(C) == toHex(fromHex(ct)) && m.decrypt(C) == P;
    printf("MKV-%d/%zu (R=%s): %s  (C = %s)\n", bits, fromHex(key).size() * 8, rounds ? std::to_string(rounds).c_str() : "bang", ok ? "DUNG" : "SAI", toHex(C).c_str());
    return ok;
}
static int selfTest() {
    const char* P128 = "112233445566778899AABBCCDDEEFF00";
    const char* K = "0102030405060708090A0B0C0D0E0F1112131415161718191A1B1C1D1E1F2223";
    const char* P256 = "112233445566778899AABBCCDDEEFF00112233445566778899AABBCCDDEEFF00";
    std::string k128 = std::string(K).substr(0, 30) + "11";           // 01..0F 11
    std::string k192 = k128 + "1213141516171819";
    std::string k256 = std::string(K);
    std::string k384 = k256 + "0102030405060708090A0B0C0D0E0F11";
    std::string k512 = k256 + std::string(K);
    bool ok = true;
    // Vector MKV-128/128 trong ban nao ung voi R=6 (bang tham so ghi R=7); xem ghi chu.
    ok &= testVec(128, k128.c_str(), P128, "B331228334C3F81A37206591498756A1", 6);
    ok &= testVec(128, k192.c_str(), P128, "A134C786F6E77485433B2D7CAAFD7A97");
    ok &= testVec(128, k256.c_str(), P128, "8A6F9BBC745BFEE7005F04054DD1FF8E");
    ok &= testVec(256, k256.c_str(), P256, "3E6359590DC566D8E79629607D5872A6A7C9ADF74BF0D2DF00EAF76FF7C129C3");
    ok &= testVec(256, k384.c_str(), P256, "515D487A2CC34D8A68BEFF4371BE4DF4AAF458B4AB540F394F72747BD9D9DFDB");
    ok &= testVec(256, k512.c_str(), P256, "5E6FD5F88B7924E0E24F070C8E3F3FBC264350BF06A169F55167C51521B7CFA4");
    puts(ok ? "Tat ca vector thu deu khop." : "CO vector thu KHONG khop!");
    return ok ? 0 : 1;
}


// ---------------------------------------------------------------- Che do tuong tac
// CBC + dem PKCS#7; ban ma = IV (1 khoi, ngau nhien) || cac khoi ma, in dang hex.
static Bytes cbcEncrypt(const MKV& m, const Bytes& plain) {
    size_t bs = m.blockBytes(), pad = bs - plain.size() % bs;
    Bytes data = plain; data.insert(data.end(), pad, (uint8_t)pad);
    Bytes iv(bs); std::random_device rd;
    for (auto& b : iv) b = (uint8_t)rd();
    Bytes out = iv, prev = iv;
    for (size_t off = 0; off < data.size(); off += bs) {
        Bytes blk(data.begin() + off, data.begin() + off + bs);
        for (size_t i = 0; i < bs; ++i) blk[i] ^= prev[i];
        prev = m.encrypt(blk);
        out.insert(out.end(), prev.begin(), prev.end());
    }
    return out;
}
static bool cbcDecrypt(const MKV& m, const Bytes& in, Bytes& plain) {
    size_t bs = m.blockBytes();
    if (in.size() < 2 * bs || in.size() % bs) return false;
    Bytes prev(in.begin(), in.begin() + bs);
    plain.clear();
    for (size_t off = bs; off < in.size(); off += bs) {
        Bytes blk(in.begin() + off, in.begin() + off + bs);
        Bytes d = m.decrypt(blk);
        for (size_t i = 0; i < bs; ++i) d[i] ^= prev[i];
        plain.insert(plain.end(), d.begin(), d.end());
        prev = blk;
    }
    uint8_t pad = plain.back();
    if (pad == 0 || pad > bs || pad > plain.size()) return false;
    for (size_t i = 0; i < pad; ++i) if (plain[plain.size() - 1 - i] != pad) return false;
    plain.resize(plain.size() - pad);
    return true;
}
static bool isHex(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) if (!isxdigit((unsigned char)c)) return false;
    return true;
}
static std::string prompt(const char* msg) {
    std::cout << msg << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) { std::cout << "\n"; exit(0); }
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();
    return line;
}
// Khoa: chuoi hex (vd 32 ky tu hex = 16 byte) HOAC van ban thuong co dung 16/24/32 (hoac 32/48/64) ky tu
static bool parseKey(const std::string& in, int bits, Bytes& key) {
    size_t a = bits == 128 ? 16 : 32;
    auto ok = [&](size_t n) { return n == a || n == a * 3 / 2 || n == a * 2; };
    if (isHex(in) && in.size() % 2 == 0 && ok(in.size() / 2)) { key = fromHex(in); return true; }
    if (ok(in.size())) { key.assign(in.begin(), in.end()); return true; }
    printf("Khoa khong hop le. Khoi %d bit can khoa dai %zu, %zu hoac %zu byte\n"
           "(nhap %zu/%zu/%zu ky tu thuong, hoac %zu/%zu/%zu ky tu hex).\n",
           bits, a, a * 3 / 2, a * 2, a, a * 3 / 2, a * 2, a * 2, a * 3, a * 4);
    return false;
}

static void interactive() {
    puts("=== MKV (TCVN 14263:2024) - ma hoa / giai ma ===");
    for (;;) {
        puts("\n1. Ma hoa\n2. Giai ma\n3. Kiem tra vector thu\n0. Thoat");
        std::string c = prompt("Chon: ");
        if (c == "0") return;
        if (c == "3") { selfTest(); continue; }
        if (c != "1" && c != "2") continue;
        std::string b = prompt("Kich thuoc khoi (1 = 128 bit, 2 = 256 bit) [1]: ");
        int bits = b == "2" ? 256 : 128;
        Bytes key;
        for (;;) {
            std::string k = prompt("Nhap khoa: ");
            if (parseKey(k, bits, key)) break;
        }
        try {
            MKV m(bits, key);
            if (c == "1") {
                std::string text = prompt("Nhap du lieu can ma hoa: ");
                Bytes out = cbcEncrypt(m, Bytes(text.begin(), text.end()));
                printf("Ban ma (hex): %s\n", toHex(out).c_str());
            } else {
                std::string h = prompt("Nhap ban ma (hex): ");
                Bytes in = fromHex(h), plain;
                if (!cbcDecrypt(m, in, plain)) puts("Giai ma that bai: sai khoa hoac ban ma khong hop le.");
                else printf("Du lieu giai ma: %s\n", std::string(plain.begin(), plain.end()).c_str());
            }
        } catch (const std::exception& e) { printf("Loi: %s\n", e.what()); }
    }
}

int main(int argc, char** argv) {
    try {
        if (argc == 1) { interactive(); return 0; }
        if (std::string(argv[1]) == "test") return selfTest();
        if (argc != 4) {
            fprintf(stderr, "Dung: %s [enc|dec|enc256|dec256] <khoa_hex> <du_lieu_hex>  (ECB tung khoi)\n"
                            "      %s            (che do tuong tac)\n      %s test\n", argv[0], argv[0], argv[0]);
            return 2;
        }
        std::string mode = argv[1];
        bool dec = mode.rfind("dec", 0) == 0;
        int bits = mode.size() > 3 && mode.substr(3) == "256" ? 256 : 128;
        MKV m(bits, fromHex(argv[2]));
        Bytes in = fromHex(argv[3]), out;
        size_t bs = m.blockBytes();
        if (in.empty() || in.size() % bs) throw std::runtime_error("du lieu phai la boi cua kich thuoc khoi");
        for (size_t off = 0; off < in.size(); off += bs) {
            Bytes blk(in.begin() + off, in.begin() + off + bs);
            Bytes r = dec ? m.decrypt(blk) : m.encrypt(blk);
            out.insert(out.end(), r.begin(), r.end());
        }
        std::cout << toHex(out) << "\n";
    } catch (const std::exception& e) {
        fprintf(stderr, "Loi: %s\n", e.what());
        return 1;
    }
    return 0;
}
