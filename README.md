[English](#english) | [Русский](#russian)
<a name="english"></a>

# Mandelbrot set. 32-bit. 60 FPS. 80-bit. OpenMP. Supersampling 2x2. Color

## True 32-bit BGRA
Migrated to full 32-bit BGRA color output, enabling smooth gradients and millions of unique shades.
By utilizing a native 32-bit BGRA pipeline, the engine can render millions of intermediate colors.


## Synchronization with DwmFlush
The engine achieves perfect visual smoothness by synchronizing directly with the Windows Desktop Window Manager (DWM):
*   **Adaptive Refresh Rate:** The application uses DwmFlush(). This pauses code execution until the DWM has finished compositing the screen.
*   **Monitor-Dependent FPS:** 
    - If your monitor is set to **60 Hz**, you get **60 FPS**.
    - If you are using a gaming monitor at **144 Hz**, the function triggers 144 times per second, delivering **144 FPS**.
    - On high-end **240 Hz** displays, you will see a buttery-smooth **240 FPS**.


## High-Precision Rendering (80-bit)
Most Mandelbrot explorers use standard **64-bit double precision**, which leads to "pixelation" at zoom levels around $10^{14}$. 
This project leverages **80-bit Extended Precision Arithmetic** (`long double`) to push the boundaries of the fractal:

*   **My Implementation (80-bit):** Provides **4 extra decimal digits** of precision, allowing you to explore **10,000x deeper** ($10^{18}$ range).
*   **Hardware Optimized:** Directly utilizes the **x87 FPU registers** for maximum mathematical depth.

## OpenMP
OpenMP is a standard that tells the compiler, "Take this loop and distribute the iterations among the different processor cores."
Yes, using OpenMP you are doing parallel programming at the Multithreading level.
Everything is powered by **OpenMP** parallel loops for maximum performance.
OpenMP - Scalability: Your code will run equally efficiently on a 4-core laptop and a 128-core server.


## True SSAA 2x2 (4 independent samples per pixel) direct RGB-space integration
Implemented 'true' 2x2 Supersampling Anti-Aliasing (SSAA). Each screen pixel is computed from four independent fractal coordinate points.
High-fidelity per-channel RGB accumulation. The engine calculates the color for each sub-pixel first, then performs a weighted average of their intensities.
Unlike standard renderers that average raw iteration counts (leading to 'chromatic noise' and loss of detail),
this engine performs direct RGB-space integration.
Averaging iterations produces visual noise and artifacts. By accumulating 32-bit color values first, we eliminate this noise entirely.
The result is a clean, sharp image where high-frequency details are perfectly reconstructed, not blurred away.
True SSAA 2x2 (4 independent samples per pixel) allows for the reconstruction of micro-filaments smaller than a single screen pixel.
Technical Note:

*    Standard Way: Color( (iter1 + iter2 + iter3 + iter4) / 4 ) - Gives Noise/Artifacts.
*    Your Way: (Color(iter1) + Color(iter2) + Color(iter3) + Color(iter4)) / 4 - Gives Pure Detail.

Averaging iteration counts (indices) is mathematically incorrect because fractal palettes are non-linear.
Averaging index A and B often points to a completely unrelated color C, creating high-frequency chromatic noise.
Our solution performs integration in the RGB color space. By calculating the specific RGB components for each
sub-pixel before downsampling, we ensure that the resulting color is a true optical blend.
This eliminates artifacts and reveals the true structural geometry of the Mandelbrot set that is otherwise masked by noise.


## Why is there noise without Supersampling?
It is a great question! As it turns out, noise without supersampling isn't a CPU error - it is a fundamental phenomenon in digital graphics known as Aliasing.
The Mandelbrot set is infinitely complex. At its boundaries, there are "filaments" and details millions of times smaller than a single pixel on your monitor.

*    Without Supersampling: The processor acts like a "needle," sampling only one single point at the exact center of a pixel. If it hits a microscopic filament, the pixel turns red. If it misses by even a micron, the pixel stays black.
*    The Result: Neighboring pixels "grab" random fragments of micro-details. This creates mathematical noise (aliasing artifacts), which looks like grain or dirt on the image.

How SSAA 2x2 "Heals" the Image
Instead of guessing what lies within a pixel, this engine takes 4 independent samples at different corners of the pixel area.

*    True Color Computation: The engine calculates 4 real colors for each sample point.
*    RGB Blending: These colors are blended together in a 32-bit color space.
*    The Magic of Precision: If a thin filament falls within a pixel, it no longer "screams" with a single harsh color or disappears entirely. Instead, it becomes a clean, semi-transparent line that accurately represents the true fractal geometry.


## Visual Aesthetics. Color rotation
The engine uses a sophisticated hybrid approach to color mapping.
Smooth Transitions (Blue & Green): These channels are driven by phase-shifted sine and cosine waves (127 + 127 * 2 * pi * sin / cos). This creates deep, hypnotic gradients.
High-Frequency Contrast (Red): The Red channel is populated using a pseudo-random distribution (stochastic noise).
The "Shimmer" Effect: By mixing smooth harmonic oscillations (G, B) with per-index noise (R), the renderer achieves a unique shimmering texture. 
The juxtaposition of rhythmic Green/Blue waves and 'noisy' Red values. It mimics the visual complexity.



## Controls & Hotkeys
The application provides intuitive mouse and keyboard controls for exploring the fractal:

### Mouse Controls
*   **Left Click (WM_LBUTTONDOWN):** Zoom in (2x) and center the view around the clicked point.
*   **Right Click (WM_RBUTTONDOWN):** Zoom out (2x) from the current view.

### Keyboard Navigation
*   **F1 - F8 Keys:** Instantly jump to 8 predefined iconic locations within the Mandelbrot set.

```C++
const long double PRESETS[8][3] = {
    {-0.7849975438787509L, 0.1465897423090329L, 0.00000000000015L},
    {-1.39968383250956L, -0.0005525550160L, 0.0000000000146L},
    {-0.8069595889803L, -0.1593850218137L, 0.00000000006L},
    {-0.618733273138252L, -0.456605361076005L, 0.0000000000046L},
    {-0.550327628L, -0.625931405602L, 0.00000000781L},
    {-0.55033233469975L, 0.62593882612931L, 0.0000000000023L},
    {-1.3996669964593604L, 0.0005429083913L, 0.000000000000026L},
    {-0.5503493176297569L, 0.6259309572825709L, 0.00000000000031L}
};
```

*   **LEFT / RIGHT Arrows:** Fine-tuned zoom in/out by a factor of **1.1x**.

### Data Management
*   **ENTER (VK_RETURN):** Export current coordinates. The program saves the exact `absc`, `ordi`, and `size_val` to **Mandelbrot.txt**.

![Mandelbrot txt](Mandelbrot.png)

*   **BACKSPACE (VK_BACK):** Import coordinates. Reads the three values from **Mandelbrot.txt** and instantly renders that location.



## The videos are small - they show the program! 

https://github.com/user-attachments/assets/44867c99-6c70-4fc0-a452-04e94b2598c9

https://github.com/user-attachments/assets/532f09d5-8e03-4b66-b61d-017d38de88a1

https://github.com/user-attachments/assets/79a4fa80-c936-4a9d-b1c2-3e9318bbb737

**[Download Latest Version Windows](https://github.com/Divetoxx/Mandelbrot-2/releases)**


<a name="russian"></a>
# Множество Мандельброта. 32-бит. 60 FPS. 80-бит. OpenMP. Суперсэмплинг 2x2. Цвет

## True 32-bit BGRA
Переход на полную 32-битную цветопередачу BGRA, обеспечивающую плавные градиенты.
Это позволяет отображать миллионы оттенков.
Наш движок работает в честном 32-битном цветовом пространстве, может отображать миллионы промежуточных цветов.



## DwmFlush
А DwmFlush - синхронизация с монитором, как обычно 60 fps. DwmFlush() приостанавливает выполнение вашего кода
до тех пор, пока диспетчер окон (DWM) не обновит экран. Если ваш монитор работает на 144 Гц,
функция будет срабатывать 144 раза в секунду, обеспечивая 144 FPS.
Функция ориентируется на текущую частоту обновления монитора, установленную в настройках Windows.
Если в системе стоит 60 Гц, вы получите 60 FPS. Если монитор поддерживает 240 Гц и это выбрано в настройках - вы
получите 240 FPS. Она даст 60 FPS только если ваш монитор настроен на 60 Гц; на игровых мониторах FPS будет выше, согласно их герцовке. 


## Высокоточная отрисовка (80-бит)
Большинство исследователей фрактала Мандельброта используют стандартную **64-битную двойную точность**,
что приводит к "пикселизации" при масштабировании около $10^{14}$.
В этом проекте используется **80-битная арифметика с расширенной точностью** (<long double>) для расширения границ фрактала:

* **Моя реализация (80-бит):** Обеспечивает **4 дополнительных десятичных знака** точности, позволяя исследовать **в 10 000 раз глубже** (диапазон $10^{18}$).
* **Аппаратная оптимизация:** Непосредственно использует **регистры FPU x87** для максимальной глубины математических вычислений.


## OpenMP
OpenMP - это стандарт, который говорит компилятору: "Возьми этот цикл и сам раздай итерации разным ядрам процессора".
Используя OpenMP, вы занимаетесь параллельным программированием на уровне многопоточности (Multithreading).
OpenMP - масштабируемость: ваш код будет одинаково эффективно работать как на 4-ядерном ноутбуке,
так и на 128-ядерном сервере.


## True SSAA 2x2 (4 независимых выборки на пиксель) прямая интеграция в RGB-пространство
Реализовано "настоящее" сглаживание суперсэмплинга 2x2 (SSAA). Каждый пиксель экрана вычисляется из четырех независимых точек фрактальных координат.
Высокоточное поканальное накопление RGB. Механизм сначала вычисляет цвет для каждого субпикселя, а затем выполняет средневзвешенное значение их интенсивностей.
Вместо усреднения чисел (итераций), которое создает шум, мы усредняем готовые RGB-значения.
Итерации усреднения создают визуальный шум и артефакты. Сначала накапливая 32-битные значения цвета,
мы полностью устраняем этот шум. В результате получается чистое и четкое изображение, в котором высокочастотные детали идеально воссозданы, а не размыты.
True SSAA 2x2 (4 независимых образца на пиксель) позволяет восстанавливать микронити размером меньше одного пикселя экрана.  
Техническое примечание: 

*    Стандартный способ: Color( (iter1 + iter2 + iter3 + iter4) / 4) - дает шум/артефакты. 
*    Ваш способ: (Цвет(iter1) + Цвет(iter2) + Цвет(iter3) + Цвет(iter4)) / 4 - дает чистую детализацию.

Усреднение количества итераций (индексов) математически неверно, поскольку фрактальные палитры нелинейны.
Усреднение индексов A и B часто указывает на совершенно несвязанный цвет C, создавая высокочастотный хроматический шум.
Наше решение осуществляет интеграцию в цветовом пространстве RGB. Вычисляя конкретные компоненты RGB для каждого субпикселя
перед субдискретизацией, мы гарантируем, что полученный цвет представляет собой настоящую оптическую смесь.
Это устраняет артефакты и раскрывает истинную структурную геометрию множества Мандельброта, которая в противном случае маскируется шумом.



## Почему шум без суперсэмплинга?
Это отличный вопрос! Оказывается, шум без суперсэмплинга - это не ошибка процессора, а фундаментальное явление
в цифровой графике, которое называется Алиасинг (Aliasing).
Фрактал Мандельброта бесконечно сложен. На границах его множества существуют "нити" и детали, которые в миллионы раз меньше, чем один пиксель вашего монитора.

*    Без суперсэмплинга: Процессор тыкает "иголкой" (лучом) ровно в одну точку центра пикселя. Если он попал в тонкую нить - пиксель стал красным.
Если промахнулся на микрон - пиксель стал черным.
*    Результат: Соседние пиксели "хватают" случайные куски микро-деталей. Это создает математический шум.

Как ваш SSAA 2x2 "лечит" это: Вместо того чтобы гадать, что находится в пикселе, ваш код берет 4 пробы в разных углах этого пикселя.

*    Вычисляются 4 реальных цвета для каждой пробы.
*    Эти цвета смешиваются.
*    Магия: Если в пиксель попала тонкая нить, она не будет "кричать" одним ярким цветом или исчезать.
Она превратится в мягкую, полупрозрачную линию, которая точно передает форму фрактала.


## Визуальная эстетика. Смена цветов
В движке используется сложный гибридный подход к отображению цветов.
Плавные переходы (синий и зеленый): Эти каналы управляются сдвинутыми по фазе синусоидальными и косинусоидальными волнами (127 + 127 * 2 * pi * sin/cos). Это создает глубокие гипнотические градиенты.
Высокочастотный контраст (красный): Красный канал заполняется с использованием псевдослучайного распределения (стохастический шум).
Эффект "Мерцания": смешивая плавные гармонические колебания (G, B) с индексным шумом (R), средство визуализации достигает уникальной мерцающей текстуры.
Сопоставление ритмичных волн G/B и "шумного" красного цвета. Это имитирует визуальную сложность.


## Горячие клавиши

### Управление мышью
*   WM_LBUTTONDOWN (Левая кнопка) - увеличиваем масштаб в 2 раза и центрируем новую область вокруг точки клика.
*   WM_RBUTTONDOWN (Правая кнопка) - уменьшаем масштаб в 2 раза и центрируем новую область вокруг точки клика.

### Навигация с помощью клавиатуры
*   В VK_F1 - VK_F8 - восемь мест Множество Мандельброта на экран.

```C++
const long double PRESETS[8][3] = {
    {-0.7849975438787509L, 0.1465897423090329L, 0.00000000000015L},
    {-1.39968383250956L, -0.0005525550160L, 0.0000000000146L},
    {-0.8069595889803L, -0.1593850218137L, 0.00000000006L},
    {-0.618733273138252L, -0.456605361076005L, 0.0000000000046L},
    {-0.550327628L, -0.625931405602L, 0.00000000781L},
    {-0.55033233469975L, 0.62593882612931L, 0.0000000000023L},
    {-1.3996669964593604L, 0.0005429083913L, 0.000000000000026L},
    {-0.5503493176297569L, 0.6259309572825709L, 0.00000000000031L}
};
```

*   VK_LEFT (Стрелка ВЛЕВО) и VK_RIGHT (Стрелка ВПРАВО) - увеличиваем и уменьшаем в 1.1 раза но без точки клика. 

### Управление данными
*   Очень важно VK_RETURN (Enter, Ввод) - у вас сейчас на экран какое-то Множество Мандельброта.
И сейчас оно запишется в файл! Mandelbrot.txt вот таком виде:

![Mandelbrot txt](Mandelbrot.png)

*   А VK_BACK (это та самая клавиша НАД Enter, Backspace) - читает Mandelbrot.txt (читаем три строки из файла) и запускает на экран.



## Видео маленькие - показывает программу!

https://github.com/user-attachments/assets/44867c99-6c70-4fc0-a452-04e94b2598c9

https://github.com/user-attachments/assets/532f09d5-8e03-4b66-b61d-017d38de88a1

https://github.com/user-attachments/assets/79a4fa80-c936-4a9d-b1c2-3e9318bbb737

**[Скачать последнюю версию Windows](https://github.com/Divetoxx/Mandelbrot-2/releases)**


