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

const long double PRESETS[9][3] = {
    {-1.749949182103598356L, -0.000000005697456381L, 0.0000000000000082L},
    {-0.1544283964364377L, -1.03085800754665175L, 0.000000000000027L},
    {-1.749675773048651182L, -0.000001140170813768L, 0.0000000000000021L},
    {-1.74907816150389628L, 0.00000550988750089L, 0.0000000000000015L},
    {-1.785772653736032933L, 0.000000500077787345L, 0.0000000000000077L},
    {-1.26707805914812303L, -0.12378821520962631L, 0.000000000000001L},
    {-1.78577278039667471L, -0.00000075696313293L, 0.0000000000000022L},
    {-1.47907765132343401L, -0.01074925010269163L, 0.000000000000033L},
    {-0.840953329790493429L, -0.230995969905604638L, 0.0000000000000019L}
};

void generate_full_palette(RGBQUAD* pal) {
    const double pi = 3.141592653589793;
    for (int i = 0; i < PALETTE_SIZE; i++) {
        double angle = (2.0 * pi * i) / (double)PALETTE_SIZE;
        pal[i].rgbRed = (uint8_t)(127.0 + 127.0 * std::sin(angle * 4));
        pal[i].rgbBlue = (uint8_t)(127.0 + 127.0 * std::cos(angle * 4));
        pal[i].rgbGreen = (uint8_t)(127.0 + 127.0 * std::sin(angle * 4));
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

                while (i < p.iter_max && (re2 + im2) < 1000000.0L) {
                    im = (re + re) * im + imc;
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
    
    std::vector<RGBQUAD> color_cache(50001);
    float offset = 0;

    while (true) {
        for (int i = 0; i <= 50000; ++i) {
            if (i >= 50000) {
                color_cache[i] = {255, 255, 255, 0};
            } else {
                int idx = (int)(50000 - i + (int)offset) % PALETTE_SIZE;
                if (idx < 0) idx += PALETTE_SIZE;
                color_cache[i] = pal[idx];
            }
        }

        #pragma omp parallel for schedule(static, 128)
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                uint32_t i0 = g_ss_buffer[(y * 2 + 0) * SS_W + (x * 2 + 0)];
                uint32_t i1 = g_ss_buffer[(y * 2 + 0) * SS_W + (x * 2 + 1)];
                uint32_t i2 = g_ss_buffer[(y * 2 + 1) * SS_W + (x * 2 + 0)];
                uint32_t i3 = g_ss_buffer[(y * 2 + 1) * SS_W + (x * 2 + 1)];

                RGBQUAD c0 = color_cache[i0];
                RGBQUAD c1 = color_cache[i1];
                RGBQUAD c2 = color_cache[i2];
                RGBQUAD c3 = color_cache[i3];

                uint32_t r = (uint32_t)c0.rgbRed   + c1.rgbRed   + c2.rgbRed   + c3.rgbRed;
                uint32_t g = (uint32_t)c0.rgbGreen + c1.rgbGreen + c2.rgbGreen + c3.rgbGreen;
                uint32_t b = (uint32_t)c0.rgbBlue  + c1.rgbBlue  + c2.rgbBlue  + c3.rgbBlue;

                int pix_idx = y * WIDTH + x;
                pixels[pix_idx].rgbRed   = (uint8_t)(r >> 2);
                pixels[pix_idx].rgbGreen = (uint8_t)(g >> 2);
                pixels[pix_idx].rgbBlue  = (uint8_t)(b >> 2);
                pixels[pix_idx].rgbReserved = 0;
            }
        }

        offset -= 1.0f;
        if (offset < 0) offset += PALETTE_SIZE;

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

if (wp >= '1' && wp <= '9') {
    int idx = (int)(wp - '1');
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

    g_params.size = 0.00000000000016L;
    g_params.iter_max = 50000;
    g_params.step = g_params.size / (long double)WIDTH;
    g_params.labsc = -1.7485462508265219L - (g_params.size / 2.0L);
    g_params.bordi = 0.000002213770706L + (g_params.size / 2.0L);
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
