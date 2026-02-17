#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <dwmapi.h>
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include <omp.h>
#include <fstream>
#include <string>
#include <iomanip>

const int WIDTH = 1000;
const int HEIGHT = 1000;
const int SS_W = 2000;
const int SS_H = 2000;
const int PALETTE_SIZE = 1024;

struct FractalParams { 
    long double step; 
    long double labsc; 
    long double bordi; 
    uint32_t iter_max; 
    long double size;
};

std::mutex g_params_mutex;
FractalParams g_params;
std::atomic<bool> g_abort{false};
HANDLE g_render_event;
uint32_t g_ss_buffer[SS_W * SS_H];


const long double PRESETS[8][3] = {
    {-0.7849975438787509L, 0.1465897423090329L, 0.00000000000015L},
    {-1.39968383250956L, -0.0005525550160L, 0.0000000000146L},
    {-0.8069595889803L, -0.1593850218137L, 0.00000000006L},
    {-0.618733273138252L, -0.456605361076005L, 0.0000000000046L},
    {-0.5503321035926785L, 0.62593891595488L, 0.00000000000026L},
    {-0.55033233469975L, 0.62593882612931L, 0.0000000000023L},
    {-1.3996669964593604L, 0.0005429083913L, 0.000000000000026L},
    {-0.5503493176297569L, 0.6259309572825709L, 0.00000000000031L}
};

uint8_t asm_red_palette[256] = {
    0x00, 0x00, 0x08, 0xDC, 0x33, 0x45, 0xAB, 0x51, 0x29, 0x5F, 0x6C, 0x14, 0x79, 0x12, 0xD7, 0x0F,
    0x4B, 0xEA, 0x5E, 0xC6, 0x53, 0xB2, 0xD8, 0xB7, 0x4E, 0x29, 0x54, 0x77, 0x3F, 0xD3, 0x47, 0x7B,
    0x26, 0xDF, 0x49, 0xC5, 0xF9, 0x7E, 0xE3, 0xD3, 0x05, 0x24, 0x24, 0x80, 0x05, 0x97, 0x02, 0xC6,
    0xA6, 0xC5, 0xB5, 0x8E, 0x34, 0xAE, 0x97, 0xF4, 0xA4, 0xFF, 0x3E, 0xAD, 0x4B, 0x15, 0xC5, 0x7E,
    0xE1, 0x82, 0x92, 0xF4, 0xAF, 0x56, 0x01, 0xB4, 0xFD, 0xC6, 0xBF, 0x32, 0x24, 0xE9, 0xC8, 0x96,
    0xDD, 0xB0, 0x49, 0x17, 0xA1, 0x47, 0xE0, 0x29, 0x45, 0x8B, 0xF5, 0x2C, 0x28, 0x44, 0x2D, 0x93,
    0x7F, 0x49, 0x27, 0x9C, 0xBD, 0xBF, 0x14, 0x13, 0x0F, 0x1B, 0xCB, 0xCE, 0x61, 0x9C, 0x6F, 0xBD,
    0x7E, 0x57, 0xD1, 0xD1, 0x24, 0x17, 0x11, 0x0F, 0x31, 0xAC, 0x3A, 0x92, 0x41, 0x5D, 0xD6, 0x50,
    0x50, 0xA8, 0xB1, 0xE7, 0x51, 0x7A, 0x4B, 0xC4, 0xBD, 0x8D, 0xB9, 0x69, 0x8A, 0x98, 0xE1, 0xAB,
    0x1C, 0x78, 0x67, 0x1D, 0x0D, 0x8C, 0xAC, 0x14, 0xEB, 0xD6, 0xAE, 0xED, 0x46, 0xB3, 0xBC, 0x7F,
    0xA9, 0x28, 0x3A, 0xF2, 0x5D, 0x20, 0xFC, 0x4E, 0x0D, 0x1A, 0xDD, 0x8D, 0xB2, 0x19, 0xAB, 0x04,
    0x4F, 0xE7, 0x89, 0x53, 0x71, 0xE6, 0x58, 0x5A, 0x7C, 0x12, 0xD9, 0xC2, 0x2E, 0xEC, 0x7E, 0x20,
    0xE4, 0x67, 0x35, 0xCE, 0x96, 0x1C, 0x9F, 0xED, 0x8A, 0x53, 0x72, 0x3A, 0x2F, 0xB0, 0xFE, 0xA1,
    0xB1, 0x8A, 0x18, 0xBF, 0x26, 0x3B, 0xE1, 0xB6, 0x70, 0xFA, 0x27, 0x0A, 0x2F, 0x02, 0x60, 0x65,
    0x5D, 0x15, 0x74, 0x81, 0x75, 0xAC, 0xD1, 0x91, 0x40, 0x5F, 0x95, 0xDF, 0x9B, 0x41, 0xB2, 0x8E,
    0xE0, 0x5C, 0x5D, 0x9F, 0xC7, 0x74, 0x34, 0xB2, 0xD6, 0xC0, 0xE6, 0x2F, 0xC6, 0x44, 0x4B, 0xB3
};

void generate_full_palette(RGBQUAD* pal) {
    const double pi = 3.141592653589793;
    for (int i = 0; i < PALETTE_SIZE; i++) {
        double angle = (2.0 * pi * i) / (double)PALETTE_SIZE;
        pal[i].rgbRed = asm_red_palette[i % 256];
        pal[i].rgbGreen = (uint8_t)(127.0 + 127.0 * std::cos(angle * 4));
        pal[i].rgbBlue = (uint8_t)(127.0 + 127.0 * std::sin(angle * 4));
        pal[i].rgbReserved = 0;
    }
}

void thread_mandelbrot_calc() {
    while (true) {
        WaitForSingleObject(g_render_event, INFINITE);
        ResetEvent(g_render_event);
        g_abort = false;

        FractalParams p;
        { std::lock_guard<std::mutex> lock(g_params_mutex); p = g_params; }

        long double ss_step = p.step / 2.0L;

        #pragma omp parallel for schedule(dynamic)
        for (int ss_y = 0; ss_y < SS_H; ++ss_y) {
            if (g_abort) continue;
            for (int ss_x = 0; ss_x < SS_W; ++ss_x) {
                long double rec = p.labsc + (ss_x * ss_step);
                long double imc = p.bordi - (ss_y * ss_step);
                long double re = 0, im = 0, re2 = 0, im2 = 0;
                uint32_t i = 0;

                while (i < p.iter_max && (re2 + im2) < 100.0L) {
                    im = 2.0L * re * im + imc;
                    re = re2 - im2 + rec;
                    re2 = re * re; im2 = im * im;
                    i++;
                }
                g_ss_buffer[ss_y * SS_W + ss_x] = i;
            }
        }
    }
}


void thread_palette_rotator(HDC hdc_win, HDC hdc_m, RGBQUAD* pixels) {
    RGBQUAD pal[PALETTE_SIZE];
    generate_full_palette(pal);
    double offset = 0;

    while (true) {
        #pragma omp parallel for schedule(static, 256)
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                uint32_t sumR = 0, sumG = 0, sumB = 0;

                for (int sy = 0; sy < 2; ++sy) {
                    for (int sx = 0; sx < 2; ++sx) {
                        int ss_idx = (y * 2 + sy) * SS_W + (x * 2 + sx);
                        uint32_t it = g_ss_buffer[ss_idx];

                        if (it >= 50000) {
                            sumR += 255; sumG += 255; sumB += 255;
                        } else {
                            int idx = (int)(it + offset) % PALETTE_SIZE;
                            if (idx < 0) idx += PALETTE_SIZE;

                            sumR += pal[idx].rgbRed;
                            sumG += pal[idx].rgbGreen;
                            sumB += pal[idx].rgbBlue;
                        }
                    }
                }

                int pix_idx = y * WIDTH + x;
                pixels[pix_idx].rgbRed   = (uint8_t)(sumR >> 2);
                pixels[pix_idx].rgbGreen = (uint8_t)(sumG >> 2);
                pixels[pix_idx].rgbBlue  = (uint8_t)(sumB >> 2);
                pixels[pix_idx].rgbReserved = 0;
            }
        }

        offset = fmod(offset + 1.0, (double)PALETTE_SIZE);
        BitBlt(hdc_win, 0, 0, WIDTH, HEIGHT, hdc_m, 0, 0, SRCCOPY);
        DwmFlush();
    }
}



LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN: {
        g_abort = true; 
        std::lock_guard<std::mutex> lock(g_params_mutex);
        long double mouse_x = (long double)((short)LOWORD(lp));
        long double mouse_y = (long double)((short)HIWORD(lp));
        long double clicked_re = g_params.labsc + (mouse_x * g_params.step);
        long double clicked_im = g_params.bordi - (mouse_y * g_params.step);
        if (msg == WM_LBUTTONDOWN) {
            g_params.size /= 2.0L;
        } else {
            g_params.size *= 2.0L;
        }
        g_params.step = g_params.size / (long double)WIDTH;
        g_params.labsc = clicked_re - (g_params.size / 2.0L);
        g_params.bordi = clicked_im + (g_params.size / 2.0L);
        SetEvent(g_render_event); 
        return 0;
    }

    case WM_KEYDOWN: {
        if (wp == VK_LEFT || wp == VK_RIGHT) {
            g_abort = true; 
            std::lock_guard<std::mutex> lock(g_params_mutex);
            long double center_re = g_params.labsc + (g_params.size / 2.0L);
            long double center_im = g_params.bordi - (g_params.size / 2.0L);
            if (wp == VK_LEFT) g_params.size /= 1.1L;
            else               g_params.size *= 1.1L;
            g_params.step = g_params.size / (long double)WIDTH;
            g_params.labsc = center_re - (g_params.size / 2.0L);
            g_params.bordi = center_im + (g_params.size / 2.0L);
            SetEvent(g_render_event);
            return 0;
        }

        if (wp >= VK_F1 && wp <= VK_F8) {
            int idx = (int)(wp - VK_F1);
            g_abort = true;
            std::lock_guard<std::mutex> lock(g_params_mutex);
            g_params.size = PRESETS[idx][2];
            g_params.step = g_params.size / (long double)WIDTH;
            g_params.labsc = PRESETS[idx][0] - (g_params.size / 2.0L);
            g_params.bordi = PRESETS[idx][1] + (g_params.size / 2.0L);
            SetEvent(g_render_event);
            return 0;
        }

        if (wp == VK_BACK) {
            std::ifstream file("Mandelbrot.txt");
            if (file.is_open()) {
                std::vector<long double> coords;
                long double val;
                while (file >> val) {
                    coords.push_back(val);
                    if (coords.size() == 3) break;
                }
                file.close();
                if (coords.size() == 3) {
                    g_abort = true;
                    std::lock_guard<std::mutex> lock(g_params_mutex);
                    long double c_re = coords[0];
                    long double c_im = coords[1];
                    long double new_size = coords[2];
                    g_params.size = new_size;
                    g_params.step = new_size / (long double)WIDTH;
                    g_params.labsc = c_re - (new_size / 2.0L);
                    g_params.bordi = c_im + (new_size / 2.0L);
                    SetEvent(g_render_event);
                }
            }
            return 0;
        }

        if (wp == VK_RETURN) {
            std::lock_guard<std::mutex> lock(g_params_mutex);
            long double center_re = g_params.labsc + (g_params.size / 2.0L);
            long double center_im = g_params.bordi - (g_params.size / 2.0L);            
            std::ofstream file("Mandelbrot.txt");
            if (file.is_open()) {
                file << std::fixed << std::setprecision(20);
                file << center_re << "\n";
                file << center_im << "\n";
                file << g_params.size << "\n";
                file.close();
            }
            return 0;
        }
        break;
    }

    case WM_DESTROY: 
        PostQuitMessage(0); 
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}      


int main() {
    HINSTANCE inst = GetModuleHandle(NULL);
    WNDCLASS wc = {0};
    wc.lpfnWndProc = wnd_proc;
    wc.hInstance = inst;
    wc.hIcon = LoadIcon(inst, MAKEINTRESOURCE(1));
    wc.lpszClassName = L"MandelClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, L"MandelClass", L"Mandelbrot set. 32-bit TrueColor. 60 FPS. 80-bit long double. OpenMP. Supersampling 2x2 (4 passes). Color rotation", 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 
        WIDTH + 16, HEIGHT + 38, NULL, NULL, inst, NULL);

    HDC hdc_win = GetDC(hwnd);
    HDC hdc_mem = CreateCompatibleDC(hdc_win);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = WIDTH;
    bmi.bmiHeader.biHeight = -HEIGHT;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; 
    bmi.bmiHeader.biCompression = BI_RGB;

    RGBQUAD* screen_pixels = nullptr;
    HBITMAP h_bmp = CreateDIBSection(hdc_mem, &bmi, DIB_RGB_COLORS, (void**)&screen_pixels, NULL, 0);
    SelectObject(hdc_mem, h_bmp);

    g_params.size = 0.00000000000015L;
    g_params.iter_max = 50000;
    g_params.step = g_params.size / (long double)WIDTH;
    g_params.labsc = -0.7849975438787509L - (g_params.size / 2.0L);
    g_params.bordi = 0.1465897423090329L + (g_params.size / 2.0L);
    g_render_event = CreateEvent(NULL, TRUE, TRUE, NULL);

    std::thread(thread_mandelbrot_calc).detach();
    std::thread(thread_palette_rotator, hdc_win, hdc_mem, screen_pixels).detach();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
