2// bộ sinh ngẫu nhiên kí tự 
// Không dùng để tạo key mã hóa nhé ! 
// Nếu dùng để tạo KEY hoặc mật khẩu mã hóa AES, không nên dùng mt19937_64 vì nó không phải CSPRNG. Khi đó nên dùng OpenSSL RAND_bytes() để đảm bảo tính an toàn mật mã nhé
#include <iostream>
#include <string>
#include <random>

using namespace std;

string generateRandom(int length, const string& charset) {
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<> dist(0, charset.size() - 1);

    string result;

    for (int i = 0; i < length; i++) {
        result += charset[dist(gen)];
    }

    return result;
}

int main() {
    cout << "=== RANDOM STRING GENERATOR ===\n";
    cout << "1. Generate 128 HEX characters\n";
    cout << "2. Generate 64 alphabetic characters\n";
    cout << "Choose: ";

    int choice;
    cin >> choice;

    string result;

    if (choice == 1) {
        result = generateRandom(128, "0123456789abcdef");
    }
    else if (choice == 2) {
        result = generateRandom(64,
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    }
    else {
        cout << "Invalid choice!\n";
        return 1;
    }

    cout << "\nGenerated string:\n";
    cout << result << '\n';

    return 0;
}
