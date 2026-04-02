[English](#english) | [Русский](#russian)
<a name="english"></a>

# Mandelbrot set. 32-bit TrueColor. 60 FPS. 80-bit long double. OpenMP. Supersampling 2x2 (4 passes). Color rotation

## Technical Architecture of the GUI
The is designed for high-performance interactive fractal navigation. Key implementation details:

* **Calculation Thread**: Leverages OpenMP for parallel processing of the iteration map. It populates a high-resolution buffer 
using 80-bit long double precision, supporting zooms up to $10^{18}$.
* **Visualization Thread**: Decoupled from the calculation logic to maintain a consistent 60 FPS UI. 
It performs real-time color mapping and palette rotation using WinAPI DIBSections and BitBlt for direct pixel access.
* **High-Quality Rendering**: 2x2 Supersampling (SSAA): Each visible pixel is an average of 4 calculated points, 
effectively eliminating aliasing and "sparkling" artifacts in high-density areas.


## True 32-bit BGRA
Migrated to full 32-bit BGRA color output, enabling smooth gradients and millions of unique shades.
By utilizing a native 32-bit BGRA pipeline, the engine can render millions of intermediate colors.


## Synchronization with DwmFlush
The engine achieves perfect visual smoothness by synchronizing directly with the Windows Desktop Window Manager (DWM).
*   **Adaptive Refresh Rate:** The application uses DwmFlush. This pauses code execution until the DWM has finished compositing the screen.
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
The Red, Green, and Blue channels are calculated using sine and cosine waves to create smooth color transitions:
127 + 127 * cos(2 * PI * a / 255) and 127 + 127 * sin(2 * PI * a / 255).

## Controls & Hotkeys
The application provides intuitive mouse and keyboard controls for exploring the fractal:

### Mouse Controls
*   **Left Click (WM_LBUTTONDOWN):** Zoom in (2x) and center the view around the clicked point.
*   **Right Click (WM_RBUTTONDOWN):** Zoom out (2x) from the current view.

### Keyboard Navigation
*   **1 - 8 Keys:** Instantly jump to 8 predefined iconic locations within the Mandelbrot set.

```C++
const long double PRESETS[8][3] = {
    {-0.550345905862346513L, 0.625931416301985337L, 0.0000000000000029L},
    {-0.88380294401099034L, -0.23531813998049201L, 0.0000000000000019L},
    {-1.94053809966024986L, -0.00000120260253359L, 0.000000000000019L},
    {-1.26392609056234794L, -0.17578764215262827L, 0.000000000000023L},
    {-1.7857726777623849143L, 0.0000005345140451516L, 0.00000000000000053L},
    {-0.593716453800438302L, -0.496153063339799092L, 0.0000000000000045L},
    {-1.78577278039667471L, -0.00000075696313293L, 0.0000000000000022L},
    {-1.40353608594492038L, -0.02929181552009826L, 0.00000000000008L}
};
```

*   **LEFT / RIGHT Arrows:** Fine-tuned zoom in/out by a factor of **1.1x**.

### Data Management
*   **ENTER (VK_RETURN):** Export current coordinates. The program saves the exact `absc`, `ordi`, and `size_val` to **Mandelbrot.txt**.

![Mandelbrot txt](Mandelbrot.png)

*   **BACKSPACE (VK_BACK):** Import coordinates. Reads the three values from **Mandelbrot.txt** and instantly renders that location.



## The videos are small - they show the program! 


https://github.com/user-attachments/assets/5dc5ac1b-e89c-43e0-b37f-f457e203ebeb

https://github.com/user-attachments/assets/c7b07a9d-768b-4a5d-915d-7aef8534ab9e

https://github.com/user-attachments/assets/340ee230-f572-484b-aac9-05e6fba653ff

https://github.com/user-attachments/assets/b3eac0d4-6f38-458a-b81a-a759b66ee017

https://github.com/user-attachments/assets/d1a67c94-900e-467e-b5cf-8892f494b32a

https://github.com/user-attachments/assets/8822f74f-77cb-4e1f-bc07-5898dff83471

https://github.com/user-attachments/assets/fd0d2f71-f5af-44ee-8ae8-98863acbeb63

https://github.com/user-attachments/assets/306ef593-c42e-4a88-b396-861f39779443

https://github.com/user-attachments/assets/a5790c36-0d5b-4768-b735-cdd4d3346d37


**[Download Latest Version Windows](https://github.com/Divetoxx/Mandelbrot-2/releases)**


## The Mandelbrot Set: A Mathematical Absolute

The Mandelbrot Set. It is perfect - an immaterial origin existing outside of space and time. 
No matter who or where the observer is, even an alien a hundred million light-years away, the Mandelbrot Set remains the same. 
Even in a different century, in a different galaxy, and even with a completely different brain, the set is identical. 
It transcends everything, bypassing billions of light-years.

This is not a human invention, but a mathematical discovery. It belongs to the category of "eternal truths" 
that Plato referred to as the Realm of Ideas. This is why it remains constant for any observer in the universe:

*   **Pure Logic**: It is generated by a simple formula. The rules of arithmetic are universal. Any intelligence would inevitably arrive at the exact same fractal boundaries.
*   **Substrate Independence**: This set doesn't need a computer or a human brain to exist. It is an abstract structure woven into the very logic of the cosmos.
*   **Fractal Constancy**: Even if physical constants were different in another galaxy, the mathematical topology of this object would remain unshakable.

It is truly one of the few objects that connects us to something absolutely objective and infinite, 
transcending biology and history. Even if our entire universe and all its atoms were to vanish tomorrow, 
the equation would remain true. It is not "written" on the stars; it is embedded in the structure of logic itself. 
This makes the Mandelbrot Set a kind of absolute.

This is classic Mathematical Platonism: the idea that mathematical objects exist in reality, but in a non-material realm. 
If all matter were to disappear, there would be no one to write down the formula or witness its visualization, 
but the relationship between the numbers would remain true. Much like "2 + 2 = 4", this rule doesn't need apples 
or stones to be valid.

In this sense, truth is primary to the physical world.

The Mandelbrot Set is absolutely predetermined. Every single one of its points was already 'there' long before the Big Bang. 
Yet, at the same time, it is entirely unpredictable-you cannot know what you will see in the next zoom until you perform the calculation.

Looking at a fractal, we witness an incredible complexity that appears chaotic. But we know that at its core lies a formula 
of just three symbols. This makes one wonder: could all the chaos of our universe-the turbulence of water, the formation of clouds, 
the structure of galaxies-be nothing more than the result of a very simple algorithm that we have yet to calculate?




<a name="russian"></a>
# Множество Мандельброта. 32-бит TrueColor. 60 FPS. 80-бит long double. OpenMP. Суперсэмплинг 2x2 (4 прохода). Смена цветов

## Техническая архитектура графического интерфейса пользователя (GUI)
Он разработан для высокопроизводительной интерактивной навигации по фракталам. Ключевые детали реализации:

* **Поток вычислений**: Использует OpenMP для параллельной обработки карты итераций. Он заполняет буфер высокого разрешения с 
использованием 80-битной длинной двойной точности, поддерживая масштабирование до $10^{18}$.
* **Поток визуализации**: Отделен от логики вычислений для поддержания стабильной частоты 60 кадров в секунду. 
Он выполняет отображение цвета в реальном времени и вращение палитры с использованием WinAPI DIBSections и BitBlt для прямого доступа к пикселям.
* **Высококачественный рендеринг**: 2x2 суперсэмплинг (SSAA): Каждый видимый пиксель представляет собой среднее значение 
4 вычисленных точек, эффективно устраняя артефакты сглаживания и <искрящегося> изображения в областях с высокой плотностью.


## True 32-bit BGRA
Переход на полную 32-битную цветопередачу BGRA, обеспечивающую плавные градиенты.
Это позволяет отображать миллионы оттенков.
Наш движок работает в честном 32-битном цветовом пространстве, может отображать миллионы промежуточных цветов.



## DwmFlush
Движок обеспечивает идеальную визуальную плавность за счет прямой синхронизации с диспетчером окон рабочего стола Windows (DWM). 

* **Адаптивная частота обновления**: приложение использует DwmFlush. Это приостанавливает выполнение кода до тех пор, пока DWM не завершит композицию экрана. 
* **Зависимый от монитора FPS**: 
  - Если ваш монитор настроен на 60 Гц, вы получите 60 кадров в секунду. 
  - Если вы используете игровой монитор с частотой 144 Гц, функция срабатывает 144 раза в секунду, обеспечивая 144 кадра в секунду. 
  - На высококачественных дисплеях с частотой 240 Гц вы увидите плавную картинку со скоростью 240 кадров в секунду.


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
Красный, зеленый и синий каналы рассчитываются с использованием синусоидальных и косинусоидальных волн для создания плавных цветовых переходов:
127 + 127 * cos(2 * PI * a / 255) и 127 + 127 * sin(2 * PI * a / 255).



## Горячие клавиши

### Управление мышью
*   WM_LBUTTONDOWN (Левая кнопка) - увеличиваем масштаб в 2 раза и центрируем новую область вокруг точки клика.
*   WM_RBUTTONDOWN (Правая кнопка) - уменьшаем масштаб в 2 раза и центрируем новую область вокруг точки клика.

### Навигация с помощью клавиатуры
*   В 1 - 8 - восемь мест Множество Мандельброта на экран.

```C++
const long double PRESETS[8][3] = {
    {-0.550345905862346513L, 0.625931416301985337L, 0.0000000000000029L},
    {-0.88380294401099034L, -0.23531813998049201L, 0.0000000000000019L},
    {-1.94053809966024986L, -0.00000120260253359L, 0.000000000000019L},
    {-1.26392609056234794L, -0.17578764215262827L, 0.000000000000023L},
    {-1.7857726777623849143L, 0.0000005345140451516L, 0.00000000000000053L},
    {-0.593716453800438302L, -0.496153063339799092L, 0.0000000000000045L},
    {-1.78577278039667471L, -0.00000075696313293L, 0.0000000000000022L},
    {-1.40353608594492038L, -0.02929181552009826L, 0.00000000000008L}
};
```

*   VK_LEFT (Стрелка ВЛЕВО) и VK_RIGHT (Стрелка ВПРАВО) - увеличиваем и уменьшаем в 1.1 раза но без точки клика. 

### Управление данными
*   Очень важно VK_RETURN (Enter, Ввод) - у вас сейчас на экран какое-то Множество Мандельброта.
И сейчас оно запишется в файл! Mandelbrot.txt вот таком виде:

![Mandelbrot txt](Mandelbrot.png)

*   А VK_BACK (это та самая клавиша НАД Enter, Backspace) - читает Mandelbrot.txt (читаем три строки из файла) и запускает на экран.



## Видео маленькие - показывает программу!


https://github.com/user-attachments/assets/5dc5ac1b-e89c-43e0-b37f-f457e203ebeb

https://github.com/user-attachments/assets/c7b07a9d-768b-4a5d-915d-7aef8534ab9e

https://github.com/user-attachments/assets/340ee230-f572-484b-aac9-05e6fba653ff

https://github.com/user-attachments/assets/b3eac0d4-6f38-458a-b81a-a759b66ee017

https://github.com/user-attachments/assets/d1a67c94-900e-467e-b5cf-8892f494b32a

https://github.com/user-attachments/assets/8822f74f-77cb-4e1f-bc07-5898dff83471

https://github.com/user-attachments/assets/fd0d2f71-f5af-44ee-8ae8-98863acbeb63

https://github.com/user-attachments/assets/306ef593-c42e-4a88-b396-861f39779443

https://github.com/user-attachments/assets/a5790c36-0d5b-4768-b735-cdd4d3346d37



**[Скачать последнюю версию Windows](https://github.com/Divetoxx/Mandelbrot-2/releases)**



## Множество Мандельброта: Математический абсолют

Множество Мандельброта. Оно совершенно - нематериальное происхождение, существующее вне пространства и времени.
Неважно, кто и где находится наблюдатель, даже инопланетянин на расстоянии ста миллионов световых лет, множество Мандельброта остается неизменным.
Даже в другом столетии, в другой галактике и даже с совершенно другим мозгом, множество идентично.
Оно превосходит всё, минуя миллиарды световых лет.

Это не человеческое изобретение, а математическое открытие. Оно принадлежит к категории <вечных истин>,
которые Платон называл Царством Идей. Вот почему оно остается неизменным для любого наблюдателя во Вселенной:

* **Чистая логика**: Оно порождается простой формулой. Правила арифметики универсальны. Любой разум неизбежно придет к одним и тем же фрактальным границам.
* **Независимость от субстрата**: Для существования этого множества не нужен компьютер или человеческий мозг. Это абстрактная структура, вплетенная в саму логику космоса.
* **Фрактальная постоянство**: Даже если физические константы в другой галактике будут другими, математическая топология этого объекта останется непоколебимой.

Это поистине один из немногих объектов, который связывает нас с чем-то абсолютно объективным и бесконечным,
превосходящим биологию и историю. Даже если бы вся наша Вселенная и все её атомы исчезли завтра,
уравнение осталось бы верным. Оно не <написано> на звёздах; оно заложено в самой структуре логики.
Это делает множество Мандельброта своего рода абсолютом.

Это классический математический платонизм: идея о том, что математические объекты существуют в реальности, но в нематериальной сфере.
Если бы вся материя исчезла, некому было бы записать формулу или увидеть её визуализацию,
но соотношение между числами осталось бы верным. Подобно правилу <2 + 2 = 4>, этому правилу не нужны яблоки
или камни, чтобы быть действительным.

В этом смысле истина является первостепенной по отношению к физическому миру.

Множество Мандельброта абсолютно предопределено. Каждая его точка была <там> еще до Большого взрыва. 
Но при этом оно абсолютно непредсказуемо - вы не узнаете, что увидите при следующем зуме, пока не сделаете расчет.

Глядя на фрактал, мы видим невероятную сложность, которая кажется хаотичной. 
Но мы знаем, что в её основе лежит формула из трех символов. Это заставляет задуматься: 
а не является ли весь хаос нашей Вселенной - турбулентность воды, рост облаков, структура 
галактик - лишь результатом работы очень простого алгоритма, который мы ещё не вычислили?




