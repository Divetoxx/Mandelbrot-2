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
#include <iostream>
#include <iomanip>
#include <gmp.h>
#include <mpfr.h>

const int WIDTH = 1000;
const int HEIGHT = 1000;
const int SS_W = 2000;
const int SS_H = 2000;
const int PALETTE_SIZE = 1024;
const mpfr_prec_t MPFR_BITS = 5000;

struct FractalParams { 
    double step_d;            
    std::string center_re_str; 
    std::string center_im_str;
    std::string size_str;
    uint32_t iter_max; 
};

struct ComplexDouble {
    double re;
    double im;
};

std::mutex g_params_mutex;
FractalParams g_params;
std::atomic<bool> g_abort{false};
HANDLE g_render_event;
uint32_t g_ss_buffer[SS_W * SS_H];

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



void thread_mandelbrot_calc() {
    std::vector<ComplexDouble> ref_orbit_double;

    while (true) {
        WaitForSingleObject(g_render_event, INFINITE);
        ResetEvent(g_render_event);
        g_abort = false;

        FractalParams p;
        { std::lock_guard<std::mutex> lock(g_params_mutex); p = g_params; }

        mpfr_t rx, ry, zr, zi, zr2, zi2, tmp;
        mpfr_inits2(MPFR_BITS, rx, ry, zr, zi, zr2, zi2, tmp, NULL);

        mpfr_set_str(rx, p.center_re_str.c_str(), 10, MPFR_RNDN);
        mpfr_set_str(ry, p.center_im_str.c_str(), 10, MPFR_RNDN);

        ref_orbit_double.resize(p.iter_max + 5);

        mpfr_set_ui(zr, 0, MPFR_RNDN);
        mpfr_set_ui(zi, 0, MPFR_RNDN);
        mpfr_set_ui(zr2, 0, MPFR_RNDN);
        mpfr_set_ui(zi2, 0, MPFR_RNDN);
        uint32_t ref_i = 0;

        bool escaped = false;
        while (ref_i < p.iter_max) {
            ref_orbit_double[ref_i].re = mpfr_get_d(zr, MPFR_RNDN);
            ref_orbit_double[ref_i].im = mpfr_get_d(zi, MPFR_RNDN);

            mpfr_mul(tmp, zr, zi, MPFR_RNDN);
            mpfr_mul_ui(zi, tmp, 2, MPFR_RNDN);
            mpfr_add(zi, zi, ry, MPFR_RNDN);

            mpfr_sub(zr, zr2, zi2, MPFR_RNDN);
            mpfr_add(zr, zr, rx, MPFR_RNDN);

            mpfr_mul(zr2, zr, zr, MPFR_RNDN);
            mpfr_mul(zi2, zi, zi, MPFR_RNDN);

            if (escaped) {
                ref_i++;
                break;
            }

            mpfr_add(tmp, zr2, zi2, MPFR_RNDN);
            if (mpfr_cmp_d(tmp, 4000.0) >= 0) {
                escaped = true; 
            }
            ref_i++;
        }
        ref_orbit_double[ref_i].re = mpfr_get_d(zr, MPFR_RNDN);
        ref_orbit_double[ref_i].im = mpfr_get_d(zi, MPFR_RNDN);
        uint32_t max_valid_ref_iter = ref_i; 

        double ref_rec_d = mpfr_get_d(rx, MPFR_RNDN);
        double ref_imc_d = mpfr_get_d(ry, MPFR_RNDN);
        double ss_step_d = p.step_d;

        mpfr_clears(rx, ry, zr, zi, zr2, zi2, tmp, NULL);

        #pragma omp parallel for schedule(dynamic)
        for (int ss_y = 0; ss_y < SS_H; ++ss_y) {
            if (g_abort) continue;
            for (int ss_x = 0; ss_x < SS_W; ++ss_x) {
                
                double delta_rec = (double)(ss_x - (SS_W / 2)) * ss_step_d;
                double delta_imc = (double)((SS_H / 2) - ss_y) * ss_step_d;

                uint32_t index = 0;    
                double delta_re = 0.0; 
                double delta_im = 0.0;
                double z_re = 0.0;     
                double z_im = 0.0;

                uint32_t i = 0;
                const ComplexDouble* ref_ptr = ref_orbit_double.data();

                bool has_re_based = false; 

                while (i < p.iter_max) {
                    
                    if ((z_re * z_re + z_im * z_im) >= 4000.0) {
                        break;
                    }

                    if (index >= max_valid_ref_iter) {
                        if (!has_re_based) {
                            break;
                        } else {
                            double ld_cx = ref_rec_d + delta_rec;
                            double ld_cy = ref_imc_d - delta_imc;
                            while (i < p.iter_max && (z_re * z_re + z_im * z_im) < 4000.0) {
                                double old_re = z_re;
                                double old_im = z_im;
                                z_re = old_re * old_re - old_im * old_im + ld_cx;
                                z_im = 2.0 * old_re * old_im + ld_cy;
                                i++;
                            }
                            break;
                        }
                    }

                    if ((z_re * z_re + z_im * z_im) < (delta_re * delta_re + delta_im * delta_im)) {
                        index = 0; 
                        delta_re = z_re;
                        delta_im = z_im;
                        has_re_based = true;
                    }

                    for (int step = 0; step < 2; ++step) {
                        double Ur = ref_ptr[index].re;
                        double Ui = ref_ptr[index].im;

                        double next_delta_im = 2.0 * Ur * delta_im + 2.0 * Ui * delta_re + 2.0 * delta_re * delta_im + delta_imc;
                        delta_re = 2.0 * Ur * delta_re - 2.0 * Ui * delta_im + delta_re * delta_re - delta_im * delta_im + delta_rec;
                        delta_im = next_delta_im;

                        index++;
                    }

                    z_re = ref_ptr[index].re + delta_re;
                    z_im = ref_ptr[index].im + delta_im;
                    
                    i += 2; 
                }

                g_ss_buffer[ss_y * SS_W + ss_x] = i;
            }
        }
    }
}


LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN: {
        g_abort = true; 
        std::lock_guard<std::mutex> lock(g_params_mutex);

        mpfr_t cx, cy, sz, st, mx, my, clicked_x, clicked_y;
        mpfr_inits2(MPFR_BITS, cx, cy, sz, st, mx, my, clicked_x, clicked_y, NULL);

        mpfr_set_str(cx, g_params.center_re_str.c_str(), 10, MPFR_RNDN);
        mpfr_set_str(cy, g_params.center_im_str.c_str(), 10, MPFR_RNDN);
        mpfr_set_str(sz, g_params.size_str.c_str(), 10, MPFR_RNDN);

        mpfr_div_ui(st, sz, SS_W, MPFR_RNDN);

        double mouse_x_d = (double)((short)LOWORD(lp));
        double mouse_y_d = (double)((short)HIWORD(lp));

        double ss_mouse_x = mouse_x_d * 2.0;
        double ss_mouse_y = mouse_y_d * 2.0;

        mpfr_set_d(mx, ss_mouse_x - (double)(SS_W / 2), MPFR_RNDN);
        mpfr_set_d(my, (double)(SS_H / 2) - ss_mouse_y, MPFR_RNDN); 

        mpfr_mul(mx, mx, st, MPFR_RNDN);
        mpfr_mul(my, my, st, MPFR_RNDN);

        mpfr_add(clicked_x, cx, mx, MPFR_RNDN);
        mpfr_add(clicked_y, cy, my, MPFR_RNDN);

        if (msg == WM_LBUTTONDOWN) {
            mpfr_div_ui(sz, sz, 2, MPFR_RNDN);
        } else {
            mpfr_mul_ui(sz, sz, 2, MPFR_RNDN);
        }

        mpfr_div_ui(st, sz, SS_W, MPFR_RNDN);
        g_params.step_d = mpfr_get_d(st, MPFR_RNDN);

        char out_x[2048], out_y[2048], out_sz[2048];
        mpfr_snprintf(out_x, sizeof(out_x), "%.300Rf", clicked_x);
        mpfr_snprintf(out_y, sizeof(out_y), "%.300Rf", clicked_y);
        mpfr_snprintf(out_sz, sizeof(out_sz), "%.300Rf", sz);

        g_params.center_re_str = out_x;
        g_params.center_im_str = out_y;
        g_params.size_str = out_sz;

        mpfr_clears(cx, cy, sz, st, mx, my, clicked_x, clicked_y, NULL);
        SetEvent(g_render_event); 
        return 0;
    }

    case WM_KEYDOWN: {
        if (wp >= '1' && wp <= '5') {
            g_abort = true;
            std::lock_guard<std::mutex> lock(g_params_mutex);

            if (wp == '1') {
                g_params.center_re_str = "-1.74907816150520173167912454515663360420734509948112463480292338384";
                g_params.center_im_str = "-0.00000550991906629096602513098567268615714673236269915508056068145";
                g_params.size_str      = "1.53e-62";
            }
            if (wp == '2') {
                g_params.center_re_str = "-1.7489436617686633372073552153211507258063533373824414679761";
                g_params.center_im_str = "-0.0000073748967541889836640985849393311615399776865199722997";
                g_params.size_str      = "1.01e-55";
            }
            if (wp == '3') {
                g_params.center_re_str = "-1.7491311840575335110236048528001036247123430447933925298694915282522178938437759580179";
                g_params.center_im_str = "-0.0001996960702541036804654299663680287246637758588467627907752429037173153157138373325";
                g_params.size_str      = "2.84e-82";
            }
            if (wp == '4') {
                g_params.center_re_str = "-1.74994586497557459407526067070055710001";
                g_params.center_im_str = "-0.0000000852088539604644334731909824511";
                g_params.size_str      = "7.17e-36";
            }
            if (wp == '5') {
                g_params.center_re_str = "-1.267078059171397835210199054200436920994876769284288837862647";
                g_params.center_im_str = "-0.123788215196292957558264285607075473360968832625384429809391";
                g_params.size_str      = "2.4e-57";
            }

            mpfr_t sz, st;
            mpfr_inits2(MPFR_BITS, sz, st, NULL);
            mpfr_set_str(sz, g_params.size_str.c_str(), 10, MPFR_RNDN);
            mpfr_div_ui(st, sz, SS_W, MPFR_RNDN);
            g_params.step_d = mpfr_get_d(st, MPFR_RNDN);
            mpfr_clears(sz, st, NULL);

            SetEvent(g_render_event);
            return 0;
        }



        if (wp == VK_UP || wp == VK_DOWN) {
            g_abort = true;
            std::lock_guard<std::mutex> lock(g_params_mutex);
            mpfr_t sz, st;
            mpfr_inits2(MPFR_BITS, sz, st, NULL);
            mpfr_set_str(sz, g_params.size_str.c_str(), 10, MPFR_RNDN);
            if (wp == VK_UP) {
                mpfr_div_d(sz, sz, 1.05, MPFR_RNDN);
            }
            else if (wp == VK_DOWN) {
                mpfr_mul_d(sz, sz, 1.05, MPFR_RNDN);
            }
            mpfr_div_ui(st, sz, SS_W, MPFR_RNDN);
            g_params.step_d = mpfr_get_d(st, MPFR_RNDN);
            char out_sz[2048]; 
            mpfr_snprintf(out_sz, sizeof(out_sz), "%.300Rf", sz);
            g_params.size_str = out_sz;
            mpfr_clears(sz, st, NULL);
            SetEvent(g_render_event);
            return 0;
        }


        if (wp == VK_RETURN) {
            std::lock_guard<std::mutex> lock(g_params_mutex);
            std::ofstream file("Mandelbrot.txt");
            if (file.is_open()) {
                file << g_params.center_re_str << "\n" << g_params.center_im_str << "\n" << g_params.size_str << "\n";
                file.close();
            }
            return 0;
        }
        
        if (wp == VK_BACK) {
            std::ifstream file("Mandelbrot.txt");
            if (file.is_open()) {
                std::vector<std::string> lines; std::string line;
                while (std::getline(file, line)) {
                    if (!line.empty()) lines.push_back(line);
                    if (lines.size() == 3) break;
                }
                file.close();

                if (lines.size() == 3) {
                    g_abort = true;
                    std::lock_guard<std::mutex> lock(g_params_mutex);
                    g_params.center_re_str = lines[0];
                    g_params.center_im_str = lines[1];
                    g_params.size_str      = lines[2];

                    mpfr_t sz, st;
                    mpfr_inits2(MPFR_BITS, sz, st, NULL);
                    mpfr_set_str(sz, g_params.size_str.c_str(), 10, MPFR_RNDN);
                    mpfr_div_ui(st, sz, SS_W, MPFR_RNDN);
                    g_params.step_d = mpfr_get_d(st, MPFR_RNDN);
                    mpfr_clears(sz, st, NULL);
                    SetEvent(g_render_event);
                }
            }
            return 0;
        }
        break; 
    }
    case WM_DESTROY: PostQuitMessage(0); return 0;
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

HWND hwnd = CreateWindowEx(0, L"MandelClass", L"Mandelbrot set. MPFR + Perturbation. OpenMP. Supersampling 2x2",WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,WIDTH + 16, HEIGHT + 38, NULL, NULL, inst, NULL);
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

g_params.iter_max = 50000;
g_params.center_re_str = "-1.7491976289657893741942376816272921165326158557416159";
g_params.center_im_str = "-0.00000042530777152440422725855012159249401150956515248";
g_params.size_str      = "4.31e-51";
    
mpfr_t sz, st;
mpfr_inits2(MPFR_BITS, sz, st, NULL);
mpfr_set_str(sz, g_params.size_str.c_str(), 10, MPFR_RNDN);
mpfr_div_ui(st, sz, SS_W, MPFR_RNDN);
g_params.step_d = mpfr_get_d(st, MPFR_RNDN);
mpfr_clears(sz, st, NULL);

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
