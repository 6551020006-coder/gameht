# GameHT - Raylib Game Project

Dự án game được phát triển bằng **C++** sử dụng thư viện **Raylib** cho graphics và game development.

## 📋 Yêu cầu hệ thống

- **Windows 10 / Windows 11** (hoặc hỗ trợ các nền tảng khác tùy theo cấu hình)
- **Visual Studio Code** (hoặc IDE C++ khác)
- **C++ Compiler** (GCC, Clang, MSVC)
- **Raylib 5.0** trở lên
- **Python** (cho các script hỗ trợ - 3.6%)
- **CMake** hoặc hệ thống build tương tự

## 🚀 Cài đặt & Chạy

### Bước 1: Clone repo
```bash
git clone https://github.com/6551020006-coder/gameht.git
cd gameht
```

### Bước 2: Mở trong Visual Studio Code
```bash
code main.code-workspace
```

Hoặc double-click vào file `main.code-workspace` để tự động mở.

### Bước 3: Biên dịch & Chạy
- Mở file `src/main.cpp` từ Explorer
- Nhấn **F5** để compile và run chương trình

## 📁 Cấu trúc dự án

```
gameht/
├── src/                 # Source code C++
│   └── main.cpp         # File chính
├── main.code-workspace  # VS Code workspace configuration
├── README.md           # Tài liệu này
└── preview.jpg         # Hình ảnh xem trước
```

## 🎮 Tính năng

- ✅ Template cơ bản Raylib cho C++
- ✅ Ví dụ bouncing ball
- ✅ Cấu hình sẵn cho VS Code
- ✅ Hỗ trợ Raylib 5.0

## 🛠️ Công nghệ sử dụng

- **C++** (95.5%) - Lập trình game
- **Python** (3.6%) - Script hỗ trợ
- **Batch** (0.9%) - Build script

## 📚 Tài liệu

- 🎥 [Video Tutorial trên YouTube](https://www.youtube.com/watch?v=PaAcVk5jUd8)
- 🌍 [Website - Programming with Nick](http://www.programmingwithnick.com)
- 📖 [Raylib Documentation](https://www.raylib.com/)

## 💡 Hướng dẫn nhanh

```cpp
// Ví dụ cơ bản với Raylib
#include "raylib.h"

int main() {
    InitWindow(800, 600, "My Game");
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello, Raylib!", 10, 10, 20, BLACK);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

## 🤝 Đóng góp

Nếu bạn muốn cải thiện dự án:
1. Fork repository này
2. Tạo branch mới (`git checkout -b feature/YourFeature`)
3. Commit changes (`git commit -m 'Add YourFeature'`)
4. Push to branch (`git push origin feature/YourFeature`)
5. Tạo Pull Request

## 📝 License

Dự án này không có license được chỉ định. Vui lòng thêm license nếu cần.

## ❓ Troubleshooting

- **Lỗi compile**: Kiểm tra Raylib 5.0 đã cài đặt đúng?
- **F5 không hoạt động**: Chắc chắn `main.code-workspace` đã được mở
- **Missing libraries**: Cài đặt các dependencies cần thiết

---

Made with ❤️ by [6551020006-coder](https://github.com/6551020006-coder)
