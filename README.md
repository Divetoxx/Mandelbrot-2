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

This project implements advanced orbit phase management paradigms and perturbation algorithms 
developed by the fractal research community. Special thanks to the following authors and pioneers 
from the Fractal Forums, whose collaborative work made this engine possible:
* **Kevin Martin** - for pioneering loop optimization and edge-case escape techniques.
* **Zhuoran Yu** - for developing the dynamic orbit re-basing paradigm.
* **Claude Heiland-Allen** - for extensive deep zoom research and the creation of the MDZ project.

## Key Features:
* **High-Precision Reference:** The 1000-bit reference trajectory is computed exactly once per zoom layer.
* **Hardware-Native Performance:** Blazing-fast math for millions of pixels utilizing hardware-native double registers.
* **When** using double-precision floating-point numbers (on the order of $10^{-15}$), perturbation theory only allows you to zoom down to the **$10^{-308}$ level-no further.**
* **Dynamic Reference Relocalization** (Orbit Re-basing).
* **Bilinear approximation:** The calculation can be performed significantly faster by using bilinear approximation.
* **True 2x2 SSAA:** Pristine, anti-aliased image quality with 4 independent samples per pixel.
* **Pure C++11 Multi-threading:** High-speed parallel computing built directly into the language standard.
* **DwmFlush Synchronization:** Stutter-free, hardware-aligned frame presentation synchronized with Windows DWM.
* **Dynamic Palette Rotation:** Classic, ultra-smooth fractal color-cycling effects with zero performance overhead.

## Arbitrary Precision Arithmetic (Infinite Depth)

The engine is completely free from the hardware limitations of standard 64-bit (`double`) and 128-bit (`__float128`) data types, 
which inevitably lose significance and produce pixelated blocks at depths beyond $10^{-15}$ and $10^{-34}$ respectively.
* **MPFR/GMP Integration**: All high-precision navigation, mouse clicking, and keyboard arrow zooming are handled within a deep binary memory 
structure configured to **1000-bit precision**.
* **308-Digit Decimal:** Viewport coordinates are stored in `Mandelbrot.txt`. While the MPFR 
reference radar computes the center at 1000-bit precision, the hardware native `double` exponent limits the fast pixel engine to a maximum 
depth of $10^{-308}$, providing pristine clarity up to this physical silicon barrier.


## Blazing Fast Perturbation Theory

Deep fractal rendering no longer requires heavy "long-division-style" arbitrary precision math for every individual pixel, which 
historically slowed down deep zoom software by thousands of times.
* **Single-Pass Reference Calculation**: The ultra-heavy MPFR BigFloat radar calculates the precise reference trajectory 
for **only one central pixel per frame and strictly ONCE** at the beginning of the render.
* **Hardware-Accelerated Double Math**: The rest of the viewport (millions of super-sampled pixels) is processed concurrently at the 
native speed of the CPU's hardware `double` registers, calculating only tiny deviations (deltas) from the central axis. 
This optimization boosts rendering speeds by up to 1000x!

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

The computation can be performed significantly faster by utilizing Bilinear Approximation (BLA Skip-100). 
The code precomputes coefficient transformation matrices for steps of 100 iterations. If the current point is close 
enough to the reference orbit, the algorithm skips 100 iterations at once in a single mathematical step, minimizing the CPU load.


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


## True 2x2 SSAA (Direct RGB Integration)

This project implements "true" 2x2 Super-Sample Anti-Aliasing (SSAA). Each screen pixel is computed from four independent sub-pixel fractal coordinates.
### The Problem with the Standard Approach

A common mistake when anti-aliasing fractals is averaging the raw iteration counts (palette indices) before coloring:
* **Standard way:** `Color( (iter1 + iter2 + iter3 + iter4) / 4 )`

Mathematically, this is incorrect because fractal color palettes are **non-linear**. Averaging indices $A$ and $B$ often points to a completely unrelated color $C$. This causes severe visual noise, artifacts, and false chromatic contours.

### Our Solution: Integration in RGB Space

Instead of averaging algorithm iterations, we first compute the 32-bit color for each sub-pixel and then perform a weighted average of their RGB intensities:
* **Our way:** `(Color(iter1) + Color(iter2) + Color(iter3) + Color(iter4)) / 4`

Computing individual RGB components prior to downsampling guarantees that the resulting color is a true optical blend. This completely eliminates noise, restores micro-filaments smaller than a single pixel, and reveals the true structural geometry of the Mandelbrot set.

## Why Does Noise Occur Without Super-Sampling?

The noise seen in fractal rendering is not a bug—it is a fundamental phenomenon in digital graphics known as **aliasing**.
The Mandelbrot set is infinitely complex. At its boundaries, there are structures millions of times smaller than a physical monitor pixel.
* **Without Super-Sampling:** The algorithm samples exactly one point (the pixel center). If it hits a thin filament, the pixel turns bright; if it misses by a micron, the pixel turns black. As a result, adjacent pixels randomly "grab" disconnected pieces of micro-details, creating a noisy grid of pixels.
* **With 2x2 SSAA:** The engine takes four samples at different offsets within the pixel, computes their actual colors, and blends them. If an ultra-thin filament passes through a pixel, it neither disappears nor flickers—it blends into a smooth, semi-transparent line that accurately preserves the fractal's true shape.

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
* **Default (On Launch):** The program automatically renders a breathtaking, ultra-deep zoom location right away (scale depth ~ $10^{-113}$):
  * `Center Re:` `-1.74919762896578937419423768162729211653261585574161734756645570271259437106847945278504682550816934045282325615313706`
  * `Center Im:` `-0.0000004253077715244042272585501215924940115095651514128365051648578746521975467051762910002043554114332125868777767`
  * `Size:` `1.34e-113`

* **Keys `1` to `8`:** Press any of these keys while running to instantly teleport to one of six other pre-configured 
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

Example file content:

![Mandelbrot txt](Mandelbrot.png)

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

## Благодарности (Credits)

Этот проект использует передовые математические алгоритмы и идеи динамического управления фазой орбит, 
разработанные фрактальным сообществом. Особая благодарность авторам и исследователям с Fractal Forums, 
чей совместный труд лег в основу этого движка:
* **Kevin Martin** - автор фундаментальных методов векторизации и оптимизации циклов возмущений.
* **Zhuoran Yu** - разработчик концепции динамического сброса орбит.
* **Claude Heiland-Allen** - исследователь экстремального фрактального приближения и создатель проекта MDZ.


## Ключевые особенности:
* Расчёт опорной траектории на 1000 бит всего один раз.
* Реактивный расчёт миллионов пикселей на аппаратном double.
* При использовании чисел с плавающей запятой двойной точности (порядка $10^{-15}$) теория возмущений позволяет приблизиться к уровню **$10^{-308}$ - не дальше.**
* Динамическая релокализация опорной точки (Перебазирование орбиты).
* Расчет можно выполнить существенно быстрее, если использовать билинейную аппроксимацию.
* Настоящий SSAA 2x2 для идеально сглаженного изображения без алиасинга.
* Многопоточность на чистом C++11 (Pure C++11 Multi-threading).
* Синхронизация через DwmFlush для плавного вывода кадров без микрофризов.
* Динамическое вращение палитры для создания классического эффекта <переливающихся> цветов.


## Безграничная точность (Arbitrary Precision Arithmetic)

Движок полностью избавлен от аппаратных ограничений 64-битных (`double`) и 128-битных (`__float128`) чисел, которые неизбежно слепнут и выдают 
пиксельные квадраты на глубинах более $10^{-15}$ и $10^{-34}$.
* **Интеграция MPFR/GMP**: Вся высокоточная навигация, пересчёт масштаба при кликах мыши и движении стрелочками клавиатуры ведутся 
внутри сверхглубокой бинарной памяти с точностью **1000 бит**!
* **308 десятичных знаков**: Координаты кадра сохраняются и считываются из файла `Mandelbrot.txt`. Навигация и радар MPFR работают на глубине до 1000 бит, 
однако скоростной пиксельный дельта-движок ограничен аппаратной экспонентой `double`, что позволяет исследовать безупречно четкие 
структуры на запредельных масштабах **вплоть до $10^{-308}$ знаков**.


## Реактивный метод возмущений (Perturbation Theory)

Рендеринг глубоких фракталов больше не требует тяжелых вычислений <в столбик> для каждого пикселя, что обычно замедляло программы в тысячи раз.
* **Однократный расчёт опоры**: Сверхтяжелый BigFloat-радар MPFR вычисляет точную траекторию всего для **одной-единственной центральной точки 
кадра и строго ОДИН раз** в начале рендеринга.
* **Аппаратное ускорение на double**: Весь остальной массив экрана (миллионы супер-пикселей) рассчитывается параллельно на бешеной скорости 
чистых, аппаратных регистров `double` процессора, вычисляя лишь микроскопические отклонения (дельты) от центральной оси. 
Скорость генерации взлетела в 1000 раз!

## Динамическая релокализация опорной точки (Перебазирование орбиты)

Это предмет особой гордости: движок работает на тех же математических и архитектурных принципах, 
что и самые продвинутые рендереры Мандельброта в мире.
* Динамический сброс в ноль (Dynamic Reset to Zero): На каждом шаге для каждого пикселя отслеживается соотношение его абсолютных 
координат и текущего отклонения (дельты). Если дельта уходит слишком далеко или основная опорная орбита завершается, поток 
вычислений динамически сбрасывает индекс отслеживания в ноль. Это превращает накопленные координаты в новое автономное начало 
отсчета и полностью устраняет артефакты точности.
* Оптимизация цикла <На один шаг за предел ухода> (One-Step Beyond Escape Loop Optimization): Чтобы выжать максимум производительности 
из CPU, трекер опорной орбиты MPFR записывает ровно одну дополнительную точку в массив орбиты сразу после того, как она превышает 
радиус ухода (escape radius). Такое точное определение границы гарантирует безупречное перебазирование орбиты.
* Оптимизация ветвлений в глубоком цикле (Deep Loop Branch Optimization): Эта элегантная структура кода существенно упрощает 
условия внутри самого глубокого цикла итераций. Минимизируя тяжелые ветвления и избегая сложных вложенных условий там, где это 
возможно, код повышает эффективность предсказателя ветвлений процессора и позволяет конвейеру CPU выполнять базовые математические 
расчеты с максимальной пропускной способностью.


## Билинейную аппроксимацию

Расчет можно выполнить существенно быстрее, если использовать билинейную аппроксимацию (BLA Skip-100). 
Код предварительно рассчитывает матрицы трансформации коэффициентов для шагов по 100 итераций. 
Если текущая точка находится достаточно близко к опорной орбите, алгоритм <перепрыгивает> сразу 100 итераций 
за один математический шаг, минимизируя нагрузку на CPU.

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


## True SSAA 2x2 (Прямая интеграция в RGB)

В проекте реализовано «настоящее» сглаживание суперсэмплинга 2x2 (SSAA). Каждый пиксель экрана вычисляется из четырех 
независимых субпикселей фрактальных координат. 
### В чем проблема стандартного подхода?

Стандартный способ сглаживания фракталов часто выполняет усреднение количества итераций (индексов палитры):
* **Стандартный путь:** `Color( (iter1 + iter2 + iter3 + iter4) / 4 )`

Математически это неверно, так как фрактальные палитры **нелинейны**. Усреднение индексов цветов $A$ и $B$ часто 
указывает на совершенно несвязанный цвет $C$. Это создает сильный визуальный шум, артефакты и ложные хроматические 
контуры.
### Наше решение: интеграция в RGB

Вместо усреднения шагов алгоритма мы сначала вычисляем 32-битный цвет для каждого субпикселя, а затем выполняем 
средневзвешенное значение их интенсивностей:
* **Наш путь:** `(Color(iter1) + Color(iter2) + Color(iter3) + Color(iter4)) / 4`

Вычисление компонентов RGB перед субдискретизацией гарантирует, что полученный цвет — это настоящая оптическая смесь. 
Это полностью устраняет шум и раскрывает истинную структурную геометрию множества Мандельброта, восстанавливая 
микронити размером меньше одного пикселя.


## Почему без суперсэмплинга возникает шум?

Шум при рендеринге фрактала — это не баг, а фундаментальное явление цифровой графики: **алиасинг (aliasing)**. 

Множество Мандельброта бесконечно сложно. На его границах существуют детали, которые в миллионы раз меньше 
физического пикселя монитора.
* **Без суперсэмплинга:** Алгоритм берет пробу ровно в одной точке (в центре пикселя). Если луч попал в тонкую 
нить — пиксель становится ярким. Промахнулся на микрон — пиксель стал черным. Из-за этого соседние пиксели 
хаотично «цепляют» случайные куски микродеталей, создавая кашу из точек.
* **С SSAA 2x2:** Код берет 4 пробы в разных углах пикселя, вычисляет их реальные цвета и смешивает их. 
Если в пиксель попадает ультратонкая нить, она не исчезает и не мерцает, а превращается в мягкую, 
полупрозрачную линию, точно передающую форму фрактала.


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
* **По умолчанию (при запуске):** Программа автоматически стартует в точке экстремального приближения (глубина порядка $10^{-113}$):
  * `Center Re:` `-1.74919762896578937419423768162729211653261585574161734756645570271259437106847945278504682550816934045282325615313706`
  * `Center Im:` `-0.0000004253077715244042272585501215924940115095651514128365051648578746521975467051762910002043554114332125868777767`
  * `Size:` `1.34e-113`

* **Клавиши `1` - `8`:** Нажмите любую из этих клавиш во время работы программы, чтобы мгновенно переключиться на одну 
из шести других предустановленных точек множества Мандельброта.

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

Пример содержания файла:

![Mandelbrot txt](Mandelbrot.png)

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




