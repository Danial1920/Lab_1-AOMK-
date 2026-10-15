# EcoPolynomialMath

Лабораторная работа №1 по АОМК, задание 44: вычисление коэффициентов полинома методом наименьших квадратов (`polyfit`). Компонент написан на ANSI C89 для Eco Framework и публикует интерфейс `IEcoPolynomialMath`.

## Реализация

Проект содержит две независимые реализации, которые регистрируются через `IEcoInterfaceBus1` под разными CID:

- `CID_EcoPolynomialMath` — нормальные уравнения и метод Гаусса с выбором главного элемента;
- `CID_EcoPolynomialMathQR` — потоковый QR/Givens без накопления исходной выборки.

Обе реализации поддерживают `double`, `float`, `long double`, `int` и `long`, операции `Add`/`Subtract`, представление входов через `EcoVectorView` и объект результата `IEcoPolynomial`. Коэффициенты возвращаются от старшей степени к свободному члену. Перед решением данные центрируются и масштабируются, после чего коэффициенты переводятся обратно в исходную систему координат.

Исходники компонента не используют libc и `math.h`. Память выделяется через `IEcoMemoryAllocator1`; копирование и заполнение выполняются через `Copy` и `Fill`.

## Проверка

```sh
make test
make ECO_FRAMEWORK=/path/to/Eco.Framework eco-test
```

`make test` собирает Host UnitTest в C89 и выполняет 72 проверки с выводом `Проверок: N, ошибок: K`. `eco-test` собирает консольный Eco-клиент, получает обе реализации через шину, выполняет 76 проверок и возвращает ненулевой код при любой ошибке.

Полный локальный контроль выполняется командой `make check`: Host UnitTest, строгая контрольная C89-компиляция и проверка отсутствия libc. Независимые эталоны запускаются через `make reference`; MATLAB-скрипт находится в `DesignFiles/Reference/polyfit_reference.m`, а SciPy-проверка выполняется в отдельном окружении. При установленном MATLAB вывод сохраняется в `DesignFiles/Reference/matlab_output.txt`, а сравнение выполняется `python3 DesignFiles/Reference/compare_matlab.py`.

Фактическое сравнение вывода компонента со SciPy 1.18.1 выполнено скриптом `DesignFiles/Reference/compare_scipy.py`: максимальная абсолютная ошибка для квадратного и кубического тестов составила `6.83e-15`. MATLAB R2026b также выполнен; статус и вывод сохранены в `DesignFiles/Reference/matlab_status.txt` и `DesignFiles/Reference/matlab_output.txt`.

Платформенная матрица Linux `TARGET=0/1 × DEBUG=0/1` запускается командой `make matrix` после установки официального `ECO_TOOLCHAIN`. Без кросс-компилятора скрипт завершает работу с явным статусом `SKIPPED`; локальная macOS-проверка не выдаётся за Linux-бинарный результат.

Для Xcode откройте оба проекта из `AssemblyFiles/Mac/Xcode_v123/`, задайте `ECO_FRAMEWORK` на распакованный `Eco.Framework_20260918.zip`, соберите компонент в схемах `Static` и `Dynamic` для `Debug` и `Release`, а UnitTest — в `Static/Debug` и `Static/Release`. Команды приведены в `XCODE_SETUP.md`. После динамической сборки запустите `DesignFiles/Report/check_dynamic_deps.sh`.

Для macOS arm64 отдельные Dynamic-артефакты двух реализаций создаются скриптом `DesignFiles/Report/build_dynamic_runtime_macos.sh`. Обычный файл экспортирует фабрику `CID_EcoPolynomialMath`, QR-файл собирается с `POLYFIT_QR_ONLY` и экспортирует фабрику `CID_EcoPolynomialMathQR`. Перед запуском динамического Eco-клиента оба файла нужно разместить в каталоге runtime, который использует `ECO_FRAMEWORK_RT`.

## Структура

- `SourceFiles` — численное ядро, фабрика и регистрация на шине;
- `HeaderFiles` и `SharedFiles` — закрытые и публичные интерфейсы, IID/CID и коды ошибок;
- `UnitTestFiles` — Host UnitTest и консольный Eco-клиент;
- `AssemblyFiles` — проекты Xcode;
- `DesignFiles/Report` — отчёт и проверочные материалы;
- `DesignFiles/Prompt` — системный промпт и протокол его улучшения.

Перегрузки объявляются единым повторно подключаемым шаблоном `HeaderFiles/polyfit_core_template.h`. Проверка запретов libc выполняется скриптами `DesignFiles/Report/check_no_libc.sh` и `DesignFiles/Report/check_dynamic_deps.sh`. Исходники проекта сохранены в UTF-8 with signature и CRLF; shell-скрипты используют LF.
