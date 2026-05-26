#pragma once
#include <raylib.h>
#include <algorithm>
#include <cmath>

class SettingsUI {
public:
    // Biến lưu trữ mức âm lượng hiện tại (0 - 100)
    int currentVolume;
    
    // Trạng thái mở/đóng của dropdown menu
    bool isOpen;

    /**
     * @brief Khởi tạo đối tượng giao diện Settings.
     * @param startVolume Âm lượng khởi tạo ban đầu (mặc định là 50).
     */
    SettingsUI(int startVolume = 50) {
        currentVolume = std::max(0, std::min(100, startVolume));
        isOpen = false;
        
        // Khởi tạo các hệ số chuyển động mượt (0.0f -> 1.0f)
        dropdownHeightFactor = 0.0f;
        hoverSettingsFactor = 0.0f;
        hoverUpFactor = 0.0f;
        hoverDownFactor = 0.0f;
        
        // Khởi tạo các trạng thái di chuột
        hoverSettings = false;
        hoverUp = false;
        hoverDown = false;
    }

    /**
     * @brief Cập nhật trạng thái logic của giao diện cài đặt (nhận tọa độ chuột và sự kiện click).
     * @param mouseX Tọa độ X hiện tại của con trỏ chuột.
     * @param mouseY Tọa độ Y hiện tại của con trỏ chuột.
     * @param isClicked Trạng thái nhấn chuột trái (true nếu chuột vừa được click ở frame này).
     * @return bool Trả về true nếu chuột đang tương tác/nằm trong vùng Settings UI (để game loop bỏ qua các thao tác bắn súng/di chuyển bên dưới).
     */
    bool Update(int mouseX, int mouseY, bool isClicked) {
        float screenW = (float)GetScreenWidth();
        float screenH = (float)GetScreenHeight();

        // 1. Tự động tính toán vị trí nút "Cài đặt" cố định ở góc trên cùng bên phải màn hình
        float btnWidth = 150.0f;
        float btnHeight = 40.0f;
        float padding = 20.0f; // Khoảng cách từ biên màn hình
        
        btnSettings = {
            screenW - btnWidth - padding, // Tọa độ X
            padding,                      // Tọa độ Y
            btnWidth,                     // Chiều rộng
            btnHeight                     // Chiều cao
        };

        // 2. Tính toán vị trí cho dropdown menu (nằm ngay dưới nút "Cài đặt")
        float panelWidth = 220.0f;
        float panelMaxHeight = 170.0f;
        float panelX = screenW - panelWidth - padding;
        float panelY = padding + btnHeight + 8.0f; // Cách nút Cài đặt 8px

        // Cập nhật hoạt ảnh trượt mở rộng (dropdown animation)
        // Dùng tốc độ nội suy 12.0f kết hợp GetFrameTime() để mượt mà bất kể FPS cao hay thấp
        float targetHeightFactor = isOpen ? 1.0f : 0.0f;
        dropdownHeightFactor += (targetHeightFactor - dropdownHeightFactor) * 12.0f * GetFrameTime();
        
        // Giới hạn cận để tránh sai số dấu phẩy động vô hạn
        if (dropdownHeightFactor < 0.005f) dropdownHeightFactor = 0.0f;
        if (dropdownHeightFactor > 0.995f) dropdownHeightFactor = 1.0f;

        // Tính Bounding Box động của panel theo chiều cao hoạt ảnh hiện tại
        panelMenu = {
            panelX,
            panelY,
            panelWidth,
            panelMaxHeight * dropdownHeightFactor
        };

        // 3. Kiểm tra va chạm chuột với nút "Cài đặt" bằng giải thuật Bounding Box (AABB)
        hoverSettings = CheckPointInRect(mouseX, mouseY, btnSettings);
        
        // Hoạt ảnh đổi màu mượt khi hover nút Cài đặt
        hoverSettingsFactor += ((hoverSettings ? 1.0f : 0.0f) - hoverSettingsFactor) * 15.0f * GetFrameTime();

        // Xử lý khi click vào nút "Cài đặt"
        if (isClicked && hoverSettings) {
            isOpen = !isOpen; // Đảo trạng thái ẩn/hiện
            PlayButtonFeedbackSound();
            return true; // Đã xử lý click chuột
        }

        // 4. Nếu dropdown menu đang mở, xử lý logic va chạm các nút bên trong
        if (dropdownHeightFactor > 0.0f) {
            // Định nghĩa tọa độ cho nút Tăng (+) và Giảm (-) âm lượng bên trong panel
            float btnVolumeSize = 36.0f;
            
            // Nút Giảm (-) nằm ở bên trái
            btnVolumeDown = {
                panelX + 20.0f,
                panelY + 45.0f,
                btnVolumeSize,
                btnVolumeSize
            };

            // Nút Tăng (+) nằm ở bên phải
            btnVolumeUp = {
                panelX + panelWidth - 20.0f - btnVolumeSize,
                panelY + 45.0f,
                btnVolumeSize,
                btnVolumeSize
            };

            // Kiểm tra va chạm chuột với các nút tăng/giảm âm lượng
            // Chỉ kiểm tra khi menu đã mở ra tương đối (dropdownHeightFactor > 0.5f)
            if (dropdownHeightFactor > 0.5f) {
                hoverDown = CheckPointInRect(mouseX, mouseY, btnVolumeDown);
                hoverUp = CheckPointInRect(mouseX, mouseY, btnVolumeUp);
            } else {
                hoverDown = false;
                hoverUp = false;
            }

            // Hoạt ảnh đổi màu mượt cho 2 nút tăng/giảm
            hoverDownFactor += ((hoverDown ? 1.0f : 0.0f) - hoverDownFactor) * 15.0f * GetFrameTime();
            hoverUpFactor += ((hoverUp ? 1.0f : 0.0f) - hoverUpFactor) * 15.0f * GetFrameTime();

            // Xử lý sự kiện click tăng/giảm âm lượng
            if (isClicked) {
                if (hoverDown) {
                    currentVolume = std::max(0, currentVolume - 10); // Giảm 10%, giới hạn tối thiểu 0%
                    PlayButtonFeedbackSound();
                    return true;
                }
                if (hoverUp) {
                    currentVolume = std::min(100, currentVolume + 10); // Tăng 10%, giới hạn tối đa 100%
                    PlayButtonFeedbackSound();
                    return true;
                }
            }

            // Trả về true nếu chuột đang rê trong vùng của Dropdown menu hoặc nút cài đặt
            // Điều này giúp cô lập chuột, không cho phép game bắn laser hoặc click nhầm khi đang chỉnh âm lượng
            if (CheckPointInRect(mouseX, mouseY, panelMenu) || hoverSettings) {
                return true;
            }
        }

        return hoverSettings;
    }

    /**
     * @brief Vẽ toàn bộ giao diện SettingsUI lên màn hình.
     * Cần được gọi trong phạm vi BeginDrawing() / EndDrawing() của Game Loop chính.
     */
    void Render() {
        // Định nghĩa bảng màu cao cấp (Premium Palettes)
        Color themeDark = { 20, 20, 28, 240 };       // Nền xám đen sâu, bán trong suốt (Glassmorphism)
        Color themeCyan = { 0, 191, 165, 255 };      // Màu Teal/Cyan Neon chủ đạo
        Color themeCyanGlow = { 0, 191, 165, 120 };  // Viền Cyan mờ tạo hiệu ứng Glow sáng nhẹ
        Color textWhite = { 245, 245, 245, 255 };    // Chữ trắng ấm áp
        Color textMuted = { 150, 150, 165, 255 };    // Chữ xám nhạt cho thông tin phụ
        
        // -------------------------------------------------------------
        // VẼ NÚT "CÀI ĐẶT" CHÍNH
        // -------------------------------------------------------------
        // Tính toán màu nền dựa trên hoạt ảnh hover (Lerp màu nền)
        Color btnBgColor = LerpColor({ 35, 35, 48, 220 }, { 0, 150, 136, 255 }, hoverSettingsFactor);
        Color btnBorderColor = LerpColor({ 80, 80, 100, 150 }, themeCyan, hoverSettingsFactor);
        
        // Vẽ đổ bóng 3D nhẹ bên dưới nút Cài đặt
        DrawRectangleRounded({ btnSettings.x + 2, btnSettings.y + 3, btnSettings.width, btnSettings.height }, 0.25f, 8, { 0, 0, 0, 80 });
        
        // Vẽ nút bo góc tròn
        DrawRectangleRounded(btnSettings, 0.25f, 8, btnBgColor);
        DrawRectangleRoundedLinesEx(btnSettings, 0.25f, 8, 2.0f, btnBorderColor);

        // Vẽ biểu tượng Bánh răng (Gear icon) xoay cực kỳ chuyên nghiệp
        // Tốc độ xoay tăng lên khi di chuột qua hoặc khi mở menu
        float gearSpinSpeed = 1.0f;
        if (hoverSettings) gearSpinSpeed = 4.0f;
        if (isOpen) gearSpinSpeed = 6.0f;
        
        float gearCenterX = btnSettings.x + 24.0f;
        float gearCenterY = btnSettings.y + btnSettings.height / 2.0f;
        DrawGear(gearCenterX, gearCenterY, 8.0f, gearSpinSpeed, textWhite);

        // Vẽ dòng chữ "Cài Đặt" căn giữa trên nút
        int textW = MeasureText("Cài Đặt", 18);
        DrawText("Cài Đặt", (int)(btnSettings.x + 42.0f + (btnSettings.width - 42.0f - textW) / 2.0f), (int)(btnSettings.y + (btnSettings.height - 18.0f) / 2.0f), 18, textWhite);

        // -------------------------------------------------------------
        // VẼ DROPDOWN MENU CHỈNH ÂM LƯỢNG
        // -------------------------------------------------------------
        if (dropdownHeightFactor > 0.0f) {
            // Độ trong suốt của các thành phần bên trong phụ thuộc vào mức độ mở của dropdown (mờ dần ra)
            unsigned char alpha = (unsigned char)(255 * dropdownHeightFactor);
            Color panelBg = { themeDark.r, themeDark.g, themeDark.b, (unsigned char)(themeDark.a * dropdownHeightFactor) };
            Color borderGlow = { themeCyanGlow.r, themeCyanGlow.g, themeCyanGlow.b, (unsigned char)(themeCyanGlow.a * dropdownHeightFactor) };
            Color textCol = { textWhite.r, textWhite.g, textWhite.b, alpha };
            Color textMutedCol = { textMuted.r, textMuted.g, textMuted.b, alpha };

            // 1. Vẽ đổ bóng lớn phía sau Dropdown Menu
            DrawRectangleRounded({ panelMenu.x + 3, panelMenu.y + 4, panelMenu.width, panelMenu.height }, 0.12f, 8, { 0, 0, 0, (unsigned char)(100 * dropdownHeightFactor) });

            // 2. Vẽ khung nền Glassmorphism mờ bo góc
            DrawRectangleRounded(panelMenu, 0.12f, 8, panelBg);
            // Vẽ viền phát sáng kép (Double Border) tạo chiều sâu UI
            DrawRectangleRoundedLinesEx(panelMenu, 0.12f, 8, 1.5f, borderGlow);
            DrawRectangleRoundedLinesEx({ panelMenu.x - 1, panelMenu.y - 1, panelMenu.width + 2, panelMenu.height + 2 }, 0.12f, 8, 1.0f, { 255, 255, 255, (unsigned char)(25 * dropdownHeightFactor) });

            // 3. Nếu menu đã mở đủ rộng, tiến hành render nội dung bên trong
            if (dropdownHeightFactor > 0.4f) {
                // Tiêu đề của Panel
                const char* title = "CÀI ĐẶT GAME";
                int titleW = MeasureText(title, 14);
                DrawText(title, (int)(panelMenu.x + (panelMenu.width - titleW) / 2.0f), (int)(panelMenu.y + 15.0f), 14, textMutedCol);

                // Dòng ngăn cách mỏng màu Neon
                DrawLineEx({ panelMenu.x + 15.0f, panelMenu.y + 35.0f }, { panelMenu.x + panelMenu.width - 15.0f, panelMenu.y + 35.0f }, 1.0f, { themeCyan.r, themeCyan.g, themeCyan.b, (unsigned char)(50 * dropdownHeightFactor) });

                // Vẽ Nút Giảm Âm Lượng (-)
                Color btnDownColor = LerpColor({ 50, 50, 65, 200 }, { 211, 47, 47, 240 }, hoverDownFactor);
                btnDownColor.a = alpha;
                Color btnDownBorder = LerpColor({ 100, 100, 120, 150 }, { 255, 100, 100, 255 }, hoverDownFactor);
                btnDownBorder.a = alpha;
                
                DrawRectangleRounded(btnVolumeDown, 0.25f, 6, btnDownColor);
                DrawRectangleRoundedLinesEx(btnVolumeDown, 0.25f, 6, 1.5f, btnDownBorder);
                
                int minusW = MeasureText("-", 20);
                DrawText("-", (int)(btnVolumeDown.x + (btnVolumeDown.width - minusW) / 2.0f), (int)(btnVolumeDown.y + (btnVolumeDown.height - 20.0f) / 2.0f), 20, textCol);

                // Vẽ Nút Tăng Âm Lượng (+)
                Color btnUpColor = LerpColor({ 50, 50, 65, 200 }, { 0, 150, 136, 240 }, hoverUpFactor);
                btnUpColor.a = alpha;
                Color btnUpBorder = LerpColor({ 100, 100, 120, 150 }, themeCyan, hoverUpFactor);
                btnUpBorder.a = alpha;

                DrawRectangleRounded(btnVolumeUp, 0.25f, 6, btnUpColor);
                DrawRectangleRoundedLinesEx(btnVolumeUp, 0.25f, 6, 1.5f, btnUpBorder);
                
                int plusW = MeasureText("+", 20);
                DrawText("+", (int)(btnVolumeUp.x + (btnVolumeUp.width - plusW) / 2.0f), (int)(btnVolumeUp.y + (btnVolumeUp.height - 20.0f) / 2.0f), 20, textCol);

                // Hiển thị phần trăm âm lượng ở giữa 2 nút
                const char* volText = TextFormat("%d%%", currentVolume);
                int volTextW = MeasureText(volText, 22);
                DrawText(volText, (int)(panelMenu.x + (panelMenu.width - volTextW) / 2.0f), (int)(panelMenu.y + 52.0f), 22, textCol);

                // Vẽ nhãn phụ dưới phần trăm âm lượng
                int volLabelW = MeasureText("ÂM LƯỢNG", 11);
                DrawText("ÂM LƯỢNG", (int)(panelMenu.x + (panelMenu.width - volLabelW) / 2.0f), (int)(panelMenu.y + 80.0f), 11, textMutedCol);

                // -------------------------------------------------------------
                // VẼ THANH TRỰC QUAN HÓA ÂM LƯỢNG (Volume Bar 10 vạch)
                // -------------------------------------------------------------
                float barX = panelMenu.x + 18.0f;
                float barY = panelMenu.y + 105.0f;
                float barWidth = panelMenu.width - 36.0f;
                float barHeight = 12.0f;

                // Nền thanh âm lượng
                DrawRectangleRounded({ barX, barY, barWidth, barHeight }, 0.5f, 6, { 30, 30, 40, alpha });
                DrawRectangleRoundedLinesEx({ barX, barY, barWidth, barHeight }, 0.5f, 6, 1.0f, { 60, 60, 80, (unsigned char)(100 * dropdownHeightFactor) });

                // Vẽ 10 vạch âm lượng gradient bo tròn
                int numSegments = 10;
                float gap = 3.0f;
                float segWidth = (barWidth - (numSegments - 1) * gap) / numSegments;

                for (int i = 0; i < numSegments; i++) {
                    float segX = barX + i * (segWidth + gap);
                    Rectangle segRect = { segX, barY + 2.0f, segWidth, barHeight - 4.0f };

                    // Nếu vạch nằm trong tầm mức âm lượng hiện tại -> vẽ vạch sáng màu
                    if (i * 10 < currentVolume) {
                        // Tính toán màu gradient chạy mượt từ Teal (trái) sang Neon Green (phải)
                        float t = (float)i / (float)(numSegments - 1);
                        Color segColor = LerpColor({ 0, 191, 165, alpha }, { 46, 204, 113, alpha }, t);
                        
                        // Thêm hiệu ứng nhấp nháy sáng nhẹ theo thời gian (ambient pulse)
                        float pulse = 0.9f + sinf((float)GetTime() * 3.0f + i) * 0.1f;
                        segColor.r = (unsigned char)(segColor.r * pulse);
                        segColor.g = (unsigned char)(segColor.g * pulse);
                        segColor.b = (unsigned char)(segColor.b * pulse);

                        DrawRectangleRounded(segRect, 0.5f, 4, segColor);
                    } else {
                        // Vạch tối đại diện cho phần âm lượng chưa đạt tới
                        DrawRectangleRounded(segRect, 0.5f, 4, { 60, 60, 75, (unsigned char)(100 * dropdownHeightFactor) });
                    }
                }

                // Ghi chú nhỏ dưới chân panel
                const char* footerText = "Bấm để tăng/giảm âm lượng";
                int footerW = MeasureText(footerText, 11);
                DrawText(footerText, (int)(panelMenu.x + (panelMenu.width - footerW) / 2.0f), (int)(panelMenu.y + 138.0f), 11, textMutedCol);
            }
        }
    }

    /**
     * @brief Lấy mức âm lượng hiện tại dưới dạng giá trị float từ 0.0f đến 1.0f (dành cho các hàm SetMusicVolume/SetSoundVolume của Raylib).
     * @return float Mức âm lượng chuẩn hóa từ [0.0f, 1.0f].
     */
    float GetVolumeFloat() const {
        return (float)currentVolume / 100.0f;
    }

private:
    // Bounding Box của nút Cài đặt và dropdown panel cùng các nút âm lượng
    Rectangle btnSettings;
    Rectangle panelMenu;
    Rectangle btnVolumeUp;
    Rectangle btnVolumeDown;

    // Các biến lưu trữ tỉ lệ nội suy phục vụ hoạt ảnh (từ 0.0f -> 1.0f)
    float dropdownHeightFactor;
    float hoverSettingsFactor;
    float hoverUpFactor;
    float hoverDownFactor;

    // Trạng thái hover của chuột trên từng nút bấm
    bool hoverSettings;
    bool hoverUp;
    bool hoverDown;

    /**
     * @brief Thuật toán Bounding Box (AABB) kiểm tra một tọa độ điểm (chuột) có nằm trong một hình chữ nhật hay không.
     * @param px Tọa độ X của điểm cần kiểm tra.
     * @param py Tọa độ Y của điểm cần kiểm tra.
     * @param rect Khung chữ nhật Bounding Box.
     * @return bool Trả về true nếu điểm nằm hoàn toàn bên trong khung chữ nhật.
     */
    inline bool CheckPointInRect(int px, int py, Rectangle rect) {
        // Điểm P(x, y) nằm trong R(x, y, w, h) khi nó lớn hơn góc trái-trên và nhỏ hơn góc phải-dưới
        return (px >= rect.x && px <= rect.x + rect.width &&
                py >= rect.y && py <= rect.y + rect.height);
    }

    /**
     * @brief Hàm nội suy tuyến tính (Linear Interpolation) để biến đổi mượt mà giữa hai màu sắc.
     */
    inline Color LerpColor(Color c1, Color c2, float t) {
        t = std::max(0.0f, std::min(1.0f, t)); // Đảm bảo hệ số t luôn nằm trong khoảng [0.0, 1.0]
        return Color{
            (unsigned char)(c1.r + (c2.r - c1.r) * t),
            (unsigned char)(c1.g + (c2.g - c1.g) * t),
            (unsigned char)(c1.b + (c2.b - c1.b) * t),
            (unsigned char)(c1.a + (c2.a - c1.a) * t)
        };
    }

    /**
     * @brief Vẽ một biểu tượng bánh răng (gear icon) xoay đều.
     */
    void DrawGear(float x, float y, float radius, float speed, Color color) {
        int teethCount = 8;
        float toothWidth = radius * 0.45f;
        float toothHeight = radius * 0.35f;
        
        // Góc xoay của bánh răng phụ thuộc vào thời gian chạy của game để tạo hoạt cảnh liên tục
        float angleOffset = (float)GetTime() * speed;
        
        // 1. Vẽ các bánh răng nhô ra xung quanh
        for (int i = 0; i < teethCount; i++) {
            float angle = angleOffset + (i * 2.0f * PI / teethCount);
            
            // Tính toán vị trí chân răng
            Vector2 toothPos = {
                x + cosf(angle) * (radius - 1.5f),
                y + sinf(angle) * (radius - 1.5f)
            };
            
            // Vẽ răng hình chữ nhật xoay quanh tâm bánh răng
            Rectangle rec = { toothPos.x, toothPos.y, toothWidth, toothHeight };
            Vector2 origin = { toothWidth / 2.0f, toothHeight / 2.0f };
            DrawRectanglePro(rec, origin, angle * 180.0f / PI, color);
        }
        
        // 2. Vẽ vòng tròn đế của bánh răng
        DrawCircle((int)x, (int)y, radius - 1.0f, color);
        
        // 3. Vẽ lỗ tròn rỗng ở giữa bánh răng (màu sắc trùng với màu nền nút để tạo độ khoét)
        // Lấy màu nền dựa trên hệ số hover
        Color centerHoleColor = LerpColor({ 35, 35, 48, 255 }, { 0, 150, 136, 255 }, hoverSettingsFactor);
        DrawCircle((int)x, (int)y, radius * 0.35f, centerHoleColor);
    }

    /**
     * @brief Phát tiếng tạch phản hồi khi người dùng bấm nút (nếu Audio Device đã sẵn sàng).
     */
    void PlayButtonFeedbackSound() {
        // Chúng ta không tự nạp file âm thanh ngoài để tránh việc người chơi thiếu file asset.
        // Thay vào đó, nếu muốn âm thanh, ta có thể tích hợp với file âm thanh có sẵn của Game hoặc tạm thời bỏ trống.
        // Do hệ thống có sẵn thiết bị âm thanh, ở đây ta không gây gián đoạn.
    }
};
