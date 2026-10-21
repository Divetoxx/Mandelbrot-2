[![Language](https://img.shields.io/badge/Language-English-blue)](#english)
[![Язык](https://img.shields.io/badge/Язык-Русский-red)](#russian)

[![Download SSE3](https://img.shields.io/badge/Download-Windows-blue?logo=windows)](https://github.com/Divetoxx/Mandelbrot-2/releases/latest/download/Mandelbrot_SSE3.exe)
[![Download AVX2](https://img.shields.io/badge/Download-Windows-orange?logo=windows)](https://github.com/Divetoxx/Mandelbrot-2/releases/latest/download/Mandelbrot_AVX2.exe)
[![Source Code](https://img.shields.io/badge/Source_Code-ZIP-orange?logo=github)](https://github.com/Divetoxx/Mandelbrot-2/archive/refs/heads/main.zip)
[![Source Code](https://img.shields.io/badge/Source_Code-TAR.GZ-lightgrey?logo=github)](https://github.com/Divetoxx/Mandelbrot-2/archive/refs/heads/main.tar.gz)


<a name="english"></a>

# 🇺🇸 English Version

# Mandelbrot GUI: Visualizer with Perturbation Theory and Bilinear approximation

## Credits & Acknowledgments

This project implements advanced orbit phase management paradigms and perturbation algorithms developed 
by the fractal research community. Special thanks to the following authors and pioneers from the Fractal Forums, 
whose collaborative work made this engine possible:

* **Kevin Martin** — for pioneering loop optimization and edge-case escape techniques.
* **Zhuoran Yu** — for developing the dynamic reference relocalization and orbit re-basing paradigm.
* **James Ward Martin (JWM)** — for the creation of the pioneering MDZ (Mandelbrot Deep Zoom) project utilizing MPFR/GMP libraries.
* **Claude Heiland-Allen** — for extensive deep zoom mathematical research, preservation of fractal software legacy, 
and advanced palette conversion algorithms.


## Key Features:

* **High-Precision Reference:** The 1000-bit reference trajectory is computed exactly once per zoom layer using GNU MPFR.
* **Hardware-Native Performance:** Blazing-fast math for millions of pixels utilizing hardware-native double registers.
* When using double-precision floating-point numbers (on the order of $10^{-15}$), perturbation theory only allows you to zoom down to the **$10^{-308}$ level—no further.**
* **Dynamic Reference Relocalization** (Orbit Re-basing).
* **Bilinear approximation (BLA Skip-100):** The calculation can be performed significantly faster by skipping 100 iterations at once on smooth areas.
* **True 2x2 SSAA (Direct RGB Integration):** Pristine, anti-aliased image quality with 4 independent samples per pixel blended directly in RGB color space to eliminate chromatic noise.
* **Pure C++11 Multi-threading:** High-speed parallel computing built directly into the language standard, scaling transparently across CPU architectures.
* **DwmFlush Synchronization:** Stutter-free, hardware-aligned frame presentation synchronized with Windows DWM.
* **Dynamic Palette Rotation:** Classic, ultra-smooth fractal color-cycling effects with zero performance overhead.


## Arbitrary Precision Arithmetic (Infinite Depth)

The engine is completely free from the hardware limitations of standard data types—such as the native 64-bit (`double`) 
and the extended 80-bit (`long double`) floats—which inevitably lose significance and produce pixelated 
blocks at precision depths beyond $10^{-15}$ and $10^{-19}$ respectively.

* **MPFR/GMP Integration:** All high-precision coordinate navigation, mouse-click viewport targeting, and grid step 
calculations are handled within an isolated binary memory structure configured to a pristine **1000-bit precision**.
* **308-Digit Decimal Silicon Barrier:** The dynamic exponent range of the hardware-native `double` register limits 
the high-speed delta pixel engine to a maximum theoretical depth of **$10^{-308}$ digits**. Current viewport coordinates 
are safely stored and retrieved via the `Mandelbrot.txt` configuration file, guaranteeing flawless structural clarity 
of mathematical micro-filaments right up to this physical silicon boundary.



## High-Speed Perturbation Theory

Rendering extreme close-ups no longer requires arbitrary-precision arithmetic for each individual pixel.

* **High-Precision Reference Orbit:** MPFR BigFloat computes the exact trajectory for **one central pixel once** per frame.
* **Hardware-Accelerated Computation:** The rest of the screen is processed via hardware `double` registers by 
calculating relative deviations (deltas), accelerating frame generation up to 1000x.



## Dynamic Reference Relocalization (Orbit Re-basing)

This is a tremendous point of pride: the engine operates under the exact same mathematical and architectural principles 
as the world's most advanced Mandelbrot renderers.
*   **Dynamic Reset to Zero**: At every step, each pixel monitors the ratio of its absolute coordinates against its current delta. 
If the delta drifts too far or the primary reference orbit terminates, the execution thread dynamically 
**resets its tracking index to zero**, establishing the accumulated coordinates as a new autonomous origin 
and eliminating precision artifacts.
*   **One-Step Beyond Escape Loop Optimization**: To squeeze maximum performance out of the CPU, the MPFR reference 
tracker records exactly **one additional point** into the orbit array immediately after exceeding the escape radius. 
This precise boundary definition guarantees flawless orbit re-basing.
*   **Deep Loop Branch Optimization**: This elegant layout significantly streamlines conditions within the deepest 
iteration loop. By minimizing heavy branching and avoiding complex nested conditions where possible, the code improves 
branch prediction efficiency and allows the CPU pipeline to execute the core mathematics with maximum throughput.


## Bilinear Approximation

The rendering engine achieves a massive performance leap by integrating the Bilinear Approximation 
method (**BLA Skip-100**).

* **Coefficient Matrix Precomputation:** The algorithm precomputes the transformation matrices for intervals of exactly 
100 iterations.
* **Massive Iteration Skipping:** If the current deviation (delta) point lies safely within the computed radius 
of convergence relative to the reference orbit, the execution thread skips 100 iterations at once in a single mathematical 
step. This drastically minimizes CPU clock cycles and delivers an exponential speedup across the smooth, 
uniform filaments of the Mandelbrot set.



## Synchronization with DwmFlush

The engine achieves perfect visual smoothness by synchronizing directly with the Windows Desktop Window Manager (DWM).
*   **Adaptive Refresh Rate:** The application uses DwmFlush. This pauses code execution until the DWM has finished compositing the screen.
*   **Monitor-Dependent FPS:** 
    - If your monitor is set to **60 Hz**, you get **60 FPS**.
    - If you are using a gaming monitor at **144 Hz**, the function triggers 144 times per second, delivering **144 FPS**.
    - On high-end **240 Hz** displays, you will see a buttery-smooth **240 FPS**.


## Pure C++11 Multi-threading

High-speed parallel computing built directly into the language standard. Scales transparently across architectures-delivering 
flawless 100% CPU utilization whether running on a 4-core laptop or a 128-core high-performance server without external dependencies.


## True 2x2 SSAA: Direct RGB Integration Space

The engine implements **True 2x2 Super-Sampling Anti-Aliasing (SSAA)** using four independent 
sub-pixel fractal coordinates per final pixel to eliminate aliasing along the Mandelbrot set's complex 
boundaries.

### Mathematical Fallacy of the Standard Approach
Traditional rendering methods incorrectly average raw palette indices before coloring:
* **Standard Way:** `Color( (iter1 + iter2 + iter3 + iter4) / 4 )`

Because fractal color palettes are strictly non-linear, averaging color indices \(A\) and \(B\) yields an unrelated 
color \(C\), resulting in chromatic contours and high-contrast noise.

### Engineering Solution: Optical Integration in RGB Space
Our engine computes exact 32-bit colors for each sub-pixel before merging their channel intensities:
* **Direct RGB Integration Algorithm:** `(Color(iter1) + Color(iter2) + Color(iter3) + Color(iter4)) / 4`

This computes independent RGB components prior to downsampling to ensure a true optical blend, eradicating 
visual noise and reconstructing sub-pixel microstructures into smooth lines.


## Visual Aesthetics. Color rotation

The Red, Green, and Blue channels are calculated using sine and cosine waves to create smooth color transitions:
```C++
        pal[a][0] = (uint8_t)round(127.0 + 127.0 * cos(2.0 * PI * a / 255.0)); // Blue
        pal[a][1] = (uint8_t)round(127.0 + 127.0 * sin(2.0 * PI * a / 255.0)); // Green
        pal[a][2] = (uint8_t)round(127.0 + 127.0 * sin(2.0 * PI * a / 255.0)); // Red
```

The app features a Color Cycling (palette rotation) technique. While the heavy mathematical lifting is done once, the color 
scheme "rotates" continuously in a dedicated background thread. This creates a "living" fractal effect, allowing for endless 
observation of shifting colors without additional CPU load.

## The Mandelbrot Set: A Mathematical Absolute

It is truly one of the few objects that connects us to something absolutely objective and infinite, 
transcending biology and history. Even if our entire universe and all its atoms were to vanish tomorrow, 
the equation would remain true. It is not "written" on the stars; it is embedded in the structure of logic itself. 
This makes the Mandelbrot Set a kind of absolute. 

The Mandelbrot set exists independently of our minds and technology. It is an infinite mathematical structure that 
has always existed.

## Controls & Hotkeys
The application provides intuitive mouse and keyboard controls for exploring the fractal:

### Mouse Controls
*   **Left Click (WM_LBUTTONDOWN):** Zoom in (2x) and center the view around the clicked point.
*   **Right Click (WM_RBUTTONDOWN):** Zoom out (2x) from the current view.

### Keyboard Navigation
* **Default (On Launch):** The program automatically renders a breathtaking, ultra-deep zoom location right away (scale depth ~ $10^{-113}$).
* **Keys `1` to `8`:** Press any of these keys while running to instantly teleport to one of eight other pre-configured 
scenic locations across the Mandelbrot set.
*   **UP / DOWN Arrows:** Fine-tuned zoom in/out by a factor of **1.05x**.

### Data Management Mandelbrot.txt and File Structure
*   **ENTER (VK_RETURN):** Export current coordinates. The program saves the exact `absc`, `ordi`, and `size_val` to **Mandelbrot.txt**.
*   **BACKSPACE (VK_BACK):** Import coordinates. Reads the three values from **Mandelbrot.txt** and instantly renders that location.

To load custom coordinates create a Mandelbrot.txt file in the application folder. 
The file must contain three numbers separated by a newline:
*    Abscissa (Center X coordinate)
*    Ordinate (Center Y coordinate)
*    Size (Zoom level/Area size)

Example file:
```C++
-1.758008286774384722
-0.012039512139649579
7.3e-15
```


### Controls & Hotkeys

| Action | Input | Description |
| :--- | :--- | :--- |
| **Zoom In** | `L-Mouse Click` | Zooms **2x** into the point under cursor. |
| **Zoom Out** | `R-Mouse Click` | Zooms **2x** out from the center. |
| **Fine Zoom** | `UP / DOWN` | Precision zoom by a factor of **1.05x**. |
| **Presets** | `1` - `8` | Jump to 8 iconic fractal locations. |
| **Save Loc** | `ENTER` | Exports coordinates to `Mandelbrot.txt`. |
| **Load Loc** | `BACKSPACE` | Imports coordinates and renders. |

## The videos - they show the program! 

```C++
g_params.center_re_str = "-1.74919762896578937419423768162729211653261585574161734756645570271259437106847945278504682550816934045282325615313706";
g_params.center_im_str = "-0.0000004253077715244042272585501215924940115095651514128365051648578746521975467051762910002043554114332125868777767";
g_params.size_str  = "1.34e-113";
```
https://github.com/user-attachments/assets/bf4c67c9-48e7-4b1e-b855-a6b27e449481


```C++
if (wp == '1') {
g_params.center_re_str = "-1.74907816150520173167912454515663360420734509948112463480292338384";
g_params.center_im_str = "-0.00000550991906629096602513098567268615714673236269915508056068145";
g_params.size_str      = "1.53e-62"; }
```
https://github.com/user-attachments/assets/019a873e-55bb-4616-a92a-cf9825672798


```C++
if (wp == '2') {
g_params.center_re_str = "-1.7489436617686633372073552153211507258063533373824414679761";
g_params.center_im_str = "-0.0000073748967541889836640985849393311615399776865199722997";
g_params.size_str      = "1.01e-55"; }
```
https://github.com/user-attachments/assets/1111a250-20d9-46f7-8d87-e4ea843c48d5


```C++
if (wp == '3') {
g_params.center_re_str = "-1.7491311840575335110236048528001036247123430447933925298694915282522178938437759580179";
g_params.center_im_str = "-0.0001996960702541036804654299663680287246637758588467627907752429037173153157138373325";
g_params.size_str      = "2.84e-82"; }
```
https://github.com/user-attachments/assets/ec5accfe-b53e-48d6-a971-10af14999527


```C++
if (wp == '4') {
g_params.center_re_str = "-1.74994586497557459407526067070055710001";
g_params.center_im_str = "-0.0000000852088539604644334731909824511";
g_params.size_str      = "7.17e-36"; }
```
https://github.com/user-attachments/assets/545b722a-a2a5-4ec2-ba5c-8a5c9d052a32


```C++
if (wp == '5') {
g_params.center_re_str = "-1.267078059171397835210199054200436920994876769284288837862647";
g_params.center_im_str = "-0.123788215196292957558264285607075473360968832625384429809391";
g_params.size_str      = "2.4e-57"; }
```
https://github.com/user-attachments/assets/37a76cb8-992b-4d08-84dd-c8b5138eec1a


```C++
if (wp == '6') {
g_params.center_re_str = "-1.99999543561201124623198345433951143502785679245726844745821388800402678499411681518036306219179273434395557574279985918047221291197081186140687781560831995";
g_params.center_im_str = "-0.00000000000000000000000026198152173811047783694060060607013913873144250985383083459221663448338433592617272786772587281530484110756597337683912309313885172";
g_params.size_str      = "1.15e-119"; }
```
https://github.com/user-attachments/assets/74aa5a47-8197-4c16-a7c5-4bffc1204ea0


```C++
if (wp == '7') {
g_params.center_re_str = "-0.843233764530751513324611730003844788663891317894291476942";
g_params.center_im_str = "0.223818041558069959139898295480311440395574821870538878074";
g_params.size_str  = "2.66e-52"; }
```
https://github.com/user-attachments/assets/14efdbea-797f-420d-9694-72625f2b0e0a


```C++
if (wp == '8') {
g_params.center_re_str = "-0.12702288604303376058504940552835636145639339878096842408417373730142";
g_params.center_im_str = "-0.98728474138900681012772182210671973643707041031463043446236865072278";
g_params.size_str  = "6.18e-64"; }
```
https://github.com/user-attachments/assets/d104f654-a498-4ff7-a9db-d48dd0f98c61


**[Download Latest Version Windows](https://github.com/Divetoxx/Mandelbrot-2/releases)**




<a name="russian"></a>
# 🇷🇺 Русская версия

# Графический интерфейс Мандельброта: Визуализатор с методом возмущений и билинейную аппроксимацию.

## Благодарности и признание авторства (Credits & Acknowledgments)

Этот проект использует передовые алгоритмы метода возмущений и концепции управления фазами орбит, 
разработанные мировым фрактальным сообществом. Особая благодарность следующим авторам и первопроходцам 
с Fractal Forums, чей совместный труд сделал создание этого движка возможным:

* **Kevin Martin** — за новаторские методы векторизации, оптимизацию глубоких циклов и точное определение границ ухода орбит.
* **Zhuoran Yu** — за разработку математической концепции динамической релокализации опорной точки и перебазирования орбит (Orbit Re-basing).
* **James Ward Martin (JWM)** — за создание фундаментального проекта MDZ (Mandelbrot Deep Zoom), заложившего основы применения 
библиотек MPFR/GMP для сверхглубокого погружения.
* **Claude Heiland-Allen** — за колоссальные математические исследования фрактального зума, сохранение исходного 
программного наследия сообщества и разработку продвинутых алгоритмов трансформации палитр.



## Ключевые особенности:

* **Высокоточная опорная орбита:** Опорная траектория с точностью 1000 бит вычисляется строго один раз для каждого слоя приближения 
с использованием библиотеки GNU MPFR.
* **Аппаратное ускорение:** Сверхбыстрые математические расчеты для миллионов пикселей за счет использования аппаратных регистров 
double процессора.
* При использовании чисел с плавающей запятой двойной точности (порядка $10^{-15}$) теория возмущений позволяет приблизиться 
к уровню **$10^{-308}$ — не дальше.**
* **Динамическая релокализация опорной точки** (Перебазирование орбиты / Orbit Re-basing).
* **Билинейная аппроксимация (BLA Skip-100):** Расчет выполняется существенно быстрее за счет одновременного «перепрыгивания» 
через 100 итераций на гладких участках фрактала.
* **Настоящий суперсэмплинг 2x2 SSAA (Прямая интеграция в RGB):** Безупречное, сглаженное качество изображения с 4 независимыми 
выборками на один пиксель. Смешивание происходит напрямую в цветовом пространстве RGB, что полностью устраняет хроматический шум.
* **Многопоточность на чистом C++11:** Высокоскоростные параллельные вычисления, встроенные напрямую в стандарт языка и прозрачно 
масштабируемые на любые архитектуры процессоров.
* **Синхронизация через DwmFlush:** Плавный вывод кадров без микрофризов и разрывов, аппаратно синхронизированный с диспетчером окон Windows DWM.
* **Динамическое вращение палитры:** Классический, невероятно плавный эффект переливающихся цветов фрактала с нулевой нагрузкой на процессор.



## Безграничная точность (Arbitrary Precision Arithmetic)

Движок полностью избавлен от аппаратных ограничений стандартных типов данных — нативного 64-битного (`double`) 
и расширенного 80-битного (`long double`), которые неизбежно теряют значащие разряды и приводят к пикселизации 
кадра («эффекту квадратов») на масштабах приближения глубже $10^{-15}$ и $10^{-19}$.

* **Интеграция MPFR/GMP:** Вся высокоточная координатная навигация, масштабирование при кликах мыши и расчет шага 
сетки осуществляются внутри изолированной бинарной памяти с точностью **1000 бит**.
* **Физический лимит в 308 знаков:** Экспоненциальный диапазон аппаратного регистра `double` ограничивает скоростной 
дельта-движок максимальной теоретической глубиной погружения в **$10^{-308}$ знаков**. Текущие координаты кадра 
сохраняются и считываются через файл `Mandelbrot.txt`, гарантируя безупречную четкость математических микроструктур 
на пороге кремниевого барьера.


## Реактивный метод возмущений (Perturbation Theory)

Рендеринг экстремальных приближений больше не требует выполнения ресурсоемких длинных вычислений произвольной точности 
для каждого отдельного пикселя, что исторически замедляло фрактальный софт в тысячи раз.

* **Высокоточная опорная орбита:** Сверхтяжелый BigFloat-радар MPFR рассчитывает абсолютную и точную траекторию всего 
для **одной центральной точки кадра и строго один раз** перед началом рендеринга.
* **Аппаратное ускорение вычислений:** Весь остальной массив экрана (миллионы пикселей) просчитывается параллельно на нативной 
скорости процессорных регистров `double`. Алгоритм вычисляет лишь микроскопические отклонения (дельты) от центральной опорной оси, 
что обеспечивает колоссальный выигрыш в производительности и ускоряет генерацию кадра до 1000 раз!


## Динамическая релокализация опорной точки (Перебазирование орбиты)

Это предмет особой гордости: движок работает на тех же математических и архитектурных принципах, 
что и самые продвинутые рендереры Мандельброта в мире.
* **Динамический сброс в ноль (Dynamic Reset to Zero)**: На каждом шаге для каждого пикселя отслеживается соотношение его абсолютных 
координат и текущего отклонения (дельты). Если дельта уходит слишком далеко или основная опорная орбита завершается, поток 
вычислений динамически сбрасывает индекс отслеживания в ноль. Это превращает накопленные координаты в новое автономное начало 
отсчета и полностью устраняет артефакты точности.
* **Оптимизация цикла <На один шаг за предел ухода> (One-Step Beyond Escape Loop Optimization)**: Чтобы выжать максимум производительности 
из CPU, трекер опорной орбиты MPFR записывает ровно одну дополнительную точку в массив орбиты сразу после того, как она превышает 
радиус ухода (escape radius). Такое точное определение границы гарантирует безупречное перебазирование орбиты.
* **Оптимизация ветвлений в глубоком цикле (Deep Loop Branch Optimization)**: Эта элегантная структура кода существенно упрощает 
условия внутри самого глубокого цикла итераций. Минимизируя тяжелые ветвления и избегая сложных вложенных условий там, где это 
возможно, код повышает эффективность предсказателя ветвлений процессора и позволяет конвейеру CPU выполнять базовые математические 
расчеты с максимальной пропускной способностью.



## Билинейная аппроксимация (Bilinear Approximation)

Расчет фрактала выполняется существенно быстрее за счет интеграции метода билинейной аппроксимации (**BLA Skip-100**). 

* **Предварительный расчет матриц:** Движок заранее вычисляет коэффициенты трансформации матриц для шагов 
по 100 итераций.
* **Масштабный пропуск итераций:** Если текущая точка дельты находится в пределах безопасного радиуса сходимости 
относительно опорной орбиты, алгоритм «перепрыгивает» сразу 100 итераций за один математический шаг. 
Это кардинально снижает нагрузку на процессор и обеспечивает колоссальный прирост скорости рендеринга 
на гладких участках множества Мандельброта.


## DwmFlush

Движок обеспечивает идеальную визуальную плавность за счет прямой синхронизации с диспетчером окон рабочего стола Windows (DWM). 
*   **Адаптивная частота обновления**: приложение использует DwmFlush. Это приостанавливает выполнение кода до тех пор, пока DWM не завершит композицию экрана. 
*   **Зависимый от монитора FPS**: 
    - Если ваш монитор настроен на 60 Гц, вы получите 60 кадров в секунду. 
    - Если вы используете игровой монитор с частотой 144 Гц, функция срабатывает 144 раза в секунду, обеспечивая 144 кадра в секунду. 
    - На высококачественных дисплеях с частотой 240 Гц вы увидите плавную картинку со скоростью 240 кадров в секунду.


## Многопоточность на чистом C++11 (Pure C++11 Multi-threading)

Высокоскоростные параллельные вычисления, встроенные напрямую в стандарт языка. 
Алгоритм прозрачно масштабируется на любые архитектуры, обеспечивая безупречную 
100%-ю загрузку процессора как на 4-ядерном ноутбуке, так и на 128-ядерном 
высокопроизводительном сервере - абсолютно без внешних зависимостей.


## Истинное сглаживание SSAA 2x2: Прямая интеграция в RGB-пространство

В движке реализован алгоритм честного суперсэмплинга избыточной выборки **True 2x2 Super-Sampling Anti-Aliasing (SSAA)**. 
Каждый финальный пиксель экрана формируется на основе вычисления четырех независимых субпиксельных фрактальных координат. 
Это позволяет полностью победить **алиасинг (aliasing)** — неизбежный цифровой шум, возникающий из-за того, что бесконечно 
сложные микронити множества Мандельброта на границах множества в миллионы раз меньше физического пикселя монитора.

### Математическая несостоятельность стандартного подхода
Классический способ сглаживания во фрактальных рендерерах ошибочно выполняет усреднение сырых значений итераций (индексов палитры) 
до этапа окрашивания:
* **Компиляция по умолчанию:** `Color( (iter1 + iter2 + iter3 + iter4) / 4 )`

Математически данный подход некорректен, поскольку фрактальные цветовые палитры **строго нелинейны**. 
Попытка вычислить среднее арифметическое между индексами цветов \(A\) и \(B\) проецирует результат на абсолютно 
случайный, несвязанный цвет \(C\). В результате на изображении лавинообразно возникают ложные хроматические контуры, 
пиксельные артефакты и цифровой контрастный шум.

### Инженерное решение: Оптическая интеграция в RGB
Вместо усреднения шагов алгоритма наш движок сначала вычисляет точный 32-битный цвет для каждого из 4 субпикселей, 
а затем выполняет корректное средневзвешенное слияние их канальных интенсивностей:
* **Алгоритм Direct RGB Integration:** `(Color(iter1) + Color(iter2) + Color(iter3) + Color(iter4)) / 4`

Прямая интеграция и вычисление независимых RGB-компонентов до этапа понижения разрешения гарантируют, 
что итоговый пиксель представляет собой чистую оптическую смесь. Данная технология полностью ликвидирует визуальный 
шум, исключает мерцание картинки и восстанавливает тончайшие фрактальные микроструктуры размером меньше одного физического 
пикселя, превращая их в мягкие, полупрозрачные и непрерывные линии.



## Визуальная эстетика. Смена цветов

Красный, зеленый и синий каналы рассчитываются с использованием синусоидальных и косинусоидальных волн для 
создания плавных цветовых переходов:
```C++
        pal[a][0] = (uint8_t)round(127.0 + 127.0 * cos(2.0 * PI * a / 255.0)); // Blue
        pal[a][1] = (uint8_t)round(127.0 + 127.0 * sin(2.0 * PI * a / 255.0)); // Green
        pal[a][2] = (uint8_t)round(127.0 + 127.0 * sin(2.0 * PI * a / 255.0)); // Red
```

Приложение использует технику циклической смены цветов (вращение палитры). Хотя основная математическая обработка 
выполняется один раз, цветовая схема непрерывно <вращается> в специальном фоновом потоке. Это создает <живой> фрактальный 
эффект, позволяющий бесконечно наблюдать за изменением цветов без дополнительной нагрузки на процессор.


## Множество Мандельброта: Математический абсолют

Это поистине один из немногих объектов, который связывает нас с чем-то абсолютно объективным и бесконечным,
превосходящим биологию и историю. Даже если бы вся наша Вселенная и все её атомы исчезли завтра,
уравнение осталось бы верным. Оно не <написано> на звёздах; оно заложено в самой структуре логики.
Это делает множество Мандельброта своего рода абсолютом.

Множество Мандельброта существует независимо от нашего разума и технологий. 
Это бесконечная математическая структура, которая существовала всегда.


## Управление и горячие клавиши

### Управление мышью
*   WM_LBUTTONDOWN (Левая кнопка) - увеличиваем масштаб в 2 раза и центрируем новую область вокруг точки клика.
*   WM_RBUTTONDOWN (Правая кнопка) - уменьшаем масштаб в 2 раза и центрируем новую область вокруг точки клика.

### Быстрая навигация по фракталу
* **По умолчанию (при запуске):** Программа автоматически стартует в точке экстремального приближения (глубина порядка $10^{-113}$).
* **Клавиши `1` - `8`:** Нажмите любую из этих клавиш во время работы программы, чтобы мгновенно переключиться на одну 
из восьми других предустановленных точек множества Мандельброта.
*   VK_UP (Стрелка ВВЕРХ) и VK_DOWN (Стрелка ВНИЗ) - увеличиваем и уменьшаем в 1.05 раза но без точки клика.


### Управление данными и структура файла Mandelbrot.txt
*   Очень важно VK_RETURN (Enter, Ввод) - у вас сейчас на экран какое-то Множество Мандельброта.
И сейчас оно запишется в файл! Mandelbrot.txt
*   А VK_BACK (это та самая клавиша НАД Enter, Backspace) - читает Mandelbrot.txt (читаем три строки из файла) и запускает на экран.

Для загрузки пользовательских координат создайте текстовый файл Mandelbrot.txt в папке с программой. 
Файл должен содержать три числа, разделенных переносом строки:
*    Abscissa (Координата X центра)
*    Ordinate (Координата Y центра)
*    Size (Масштаб/Размер области)

Вот так:
```C++
-1.758008286774384722
-0.012039512139649579
7.3e-15
```


### Горячие клавиши

| Действие | Ввод | Описание |
| :--- | :--- | :--- |
| **Приблизить** | `ЛКМ` | Увеличение в **2 раза** в точке под курсором мыши. |
| **Отдалить** | `ПКМ` | Уменьшение в **2 раза** от текущего центра. |
| **Точный зум** | `Стрелки вверх / вниз` | Плавное изменение масштаба с коэффициентом **1.05x**. |
| **Пресеты** | Клавиши `1` - `8` | Мгновенный переход к 8 предустановленным локациям. |
| **Сохранить** | `ENTER` | Экспорт текущих координат и масштаба в файл `Mandelbrot.txt`. |
| **Загрузить** | `BACKSPACE` | Импорт координат из файла и мгновенный переход к месту. |


## Видео - показывает программу!

```C++
g_params.center_re_str = "-1.74919762896578937419423768162729211653261585574161734756645570271259437106847945278504682550816934045282325615313706";
g_params.center_im_str = "-0.0000004253077715244042272585501215924940115095651514128365051648578746521975467051762910002043554114332125868777767";
g_params.size_str  = "1.34e-113";
```
https://github.com/user-attachments/assets/bf4c67c9-48e7-4b1e-b855-a6b27e449481


```C++
if (wp == '1') {
g_params.center_re_str = "-1.74907816150520173167912454515663360420734509948112463480292338384";
g_params.center_im_str = "-0.00000550991906629096602513098567268615714673236269915508056068145";
g_params.size_str      = "1.53e-62"; }
```
https://github.com/user-attachments/assets/019a873e-55bb-4616-a92a-cf9825672798


```C++
if (wp == '2') {
g_params.center_re_str = "-1.7489436617686633372073552153211507258063533373824414679761";
g_params.center_im_str = "-0.0000073748967541889836640985849393311615399776865199722997";
g_params.size_str      = "1.01e-55"; }
```
https://github.com/user-attachments/assets/1111a250-20d9-46f7-8d87-e4ea843c48d5


```C++
if (wp == '3') {
g_params.center_re_str = "-1.7491311840575335110236048528001036247123430447933925298694915282522178938437759580179";
g_params.center_im_str = "-0.0001996960702541036804654299663680287246637758588467627907752429037173153157138373325";
g_params.size_str      = "2.84e-82"; }
```
https://github.com/user-attachments/assets/ec5accfe-b53e-48d6-a971-10af14999527


```C++
if (wp == '4') {
g_params.center_re_str = "-1.74994586497557459407526067070055710001";
g_params.center_im_str = "-0.0000000852088539604644334731909824511";
g_params.size_str      = "7.17e-36"; }
```
https://github.com/user-attachments/assets/545b722a-a2a5-4ec2-ba5c-8a5c9d052a32


```C++
if (wp == '5') {
g_params.center_re_str = "-1.267078059171397835210199054200436920994876769284288837862647";
g_params.center_im_str = "-0.123788215196292957558264285607075473360968832625384429809391";
g_params.size_str      = "2.4e-57"; }
```
https://github.com/user-attachments/assets/37a76cb8-992b-4d08-84dd-c8b5138eec1a


```C++
if (wp == '6') {
g_params.center_re_str = "-1.99999543561201124623198345433951143502785679245726844745821388800402678499411681518036306219179273434395557574279985918047221291197081186140687781560831995";
g_params.center_im_str = "-0.00000000000000000000000026198152173811047783694060060607013913873144250985383083459221663448338433592617272786772587281530484110756597337683912309313885172";
g_params.size_str      = "1.15e-119"; }
```
https://github.com/user-attachments/assets/74aa5a47-8197-4c16-a7c5-4bffc1204ea0


```C++
if (wp == '7') {
g_params.center_re_str = "-0.843233764530751513324611730003844788663891317894291476942";
g_params.center_im_str = "0.223818041558069959139898295480311440395574821870538878074";
g_params.size_str  = "2.66e-52"; }
```
https://github.com/user-attachments/assets/14efdbea-797f-420d-9694-72625f2b0e0a


```C++
if (wp == '8') {
g_params.center_re_str = "-0.12702288604303376058504940552835636145639339878096842408417373730142";
g_params.center_im_str = "-0.98728474138900681012772182210671973643707041031463043446236865072278";
g_params.size_str  = "6.18e-64"; }
```
https://github.com/user-attachments/assets/d104f654-a498-4ff7-a9db-d48dd0f98c61




**[Скачать последнюю версию Windows](https://github.com/Divetoxx/Mandelbrot-2/releases)**




