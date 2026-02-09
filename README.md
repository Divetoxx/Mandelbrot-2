# Mandelbrot set fragment

## Palette Generation Logic
The palette generation is done here. 0 is Green, 1 is Blue, and 2 is Red.

```pascal
pal[a][0]:=round(127+127cos(2pia/255));
pal[a][1]:=round(127+127sin(2pia/255));
pal[a][2]:=Random(256)
```


Everything is powered by **OpenMP** parallel loops for maximum performance.
OpenMP - Scalability: Your code will run equally efficiently on a 4-core laptop and a 128-core server.

## Look at the results! 

![Mandelbrot Set](Mandelbrot1.bmp)

**[Download Latest Version (Windows & Linux)](https://github.com/Divetoxx/Mandelbrot-2/releases)**




## Логика генерации палитры
Генерация палитры сделана вот. 0 - это Green, 1 - это Blue и 2 - это Red.

```pascal
pal[a][0]:=round(127+127*cos(2*pi*a/255));
pal[a][1]:=round(127+127*sin(2*pi*a/255));
pal[a][2]:=Random(256)
```



И тоже параллельный цикл OpenMP. OpenMP - масштабируемость: ваш код будет одинаково эффективно работать как на 4-ядерном ноутбуке,
так и на 128-ядерном сервере.

## Посмотрите на результаты!

![Mandelbrot Set](Mandelbrot1.bmp)


**[Скачать последнюю версию (Windows и Linux)](https://github.com/Divetoxx/Mandelbrot-2/releases)**






