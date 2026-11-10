#include <iostream>
#include <cstdlib> 
#include <ctime>   
#include <unistd.h> 

using namespace std;

void drawSnow(int width, int height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (y == 0) {
                cout << "-";
            } else if (rand() % 10 == 0) {
                cout << "*";
            } else {
                cout << " ";
            }
        }
        cout << endl;
    }
}

void drawChristmasTree(int height) {
    srand(time(0)); 
    for (int i = 1; i <= height; ++i) {
        for (int j = 1; j <= height - i; ++j) {
            cout << " "; 
        }
        for (int k = 1; k <= (2 * i - 1); ++k) {
            int decor = rand() % 10; 
            if (decor < 2) {
                cout << "o"; 
            } else if (decor == 2) {
                cout << "x"; 
            } else {
                cout << "*"; 
            }
        }
        cout << endl;
    }
    int trunkWidth = height / 3; 
    if (trunkWidth < 2) trunkWidth = 2; 
    int trunkHeight = height / 4;
    for (int j = 1; j <= height - trunkWidth / 2; ++j) {
        cout << " ";
    }
    for (int k = 1; k <= trunkWidth; ++k) {
        cout << "|";
    }
    cout << endl;
} // Đã xóa dấu đóng ngoặc thừa ở đây

int main() {
    int height;
    int width = 40;

    cout << "nhap chieu cao di Ngan Anh <3: "; // Sửa lỗi ngắt chuỗi trên nhiều dòng
    cin >> height;
    if (height < 5) {
        cout << "chieu cao toi thieu la 5 pls" << endl;
        return 1;
    }
    drawSnow(width, height);
    drawChristmasTree(height);

    return 0;
}
