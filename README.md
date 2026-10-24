# EcoPolynomialMath (Лаб.работа №1, задание 44)

Лабораторная работа №1 по АОМК, задание 44: вычисление коэффициентов полинома методом наименьших квадратов (`polyfit`). Компонент написан на ANSI C89 для Eco Framework и публикует интерфейс `IEcoPolynomialMath`.

## Лабораторная работа №2

В основной ветке `main` поверх Lab1 добавлен ACOM-граф калькуляторных компонентов: `IEcoCalculatorX` (сложение и вычитание), `IEcoCalculatorY` (умножение, деление и `TryDivision`), корневые интерфейсы `IEcoLab1` и `IEcoLab2`, отдельные CID A–E, включение A/D, агрегирование B/E и внешнее агрегирование `Eco.Lab1` компонентом `Eco.Lab2`. Поведение polyfit и его UnitTest сохранены.

## Реализация

Проект содержит две независимые реализации, которые регистрируются через `IEcoInterfaceBus1` под разными CID:

- `CID_EcoPolynomialMath` — нормальные уравнения и метод Гаусса с выбором главного элемента;
- `CID_EcoPolynomialMathQR` — потоковый QR/Givens без накопления исходной выборки.

Обе реализации поддерживают `double`, `float`, `long double`, `int` и `long`, операции `Add`/`Subtract`, представление входов через `EcoVectorView` и объект результата `IEcoPolynomial`. Коэффициенты возвращаются от старшей степени к свободному члену. Перед решением данные центрируются и масштабируются, после чего коэффициенты переводятся обратно в исходную систему координат.

Исходники компонента не используют libc и `math.h`. Память выделяется через `IEcoMemoryAllocator1`; копирование и заполнение выполняются через `Copy` и `Fill`.

## Проверка

```sh
cd "/Users/danial2006/Documents/ChatGPT/Лабораторная работа №1"

make test
make ECO_FRAMEWORK=/path/to/Eco.Framework eco-test
make check
```

`make test` собирает Host UnitTest в C89 и выполняет 72 проверки с выводом `Проверок: N, ошибок: K`. `eco-test` собирает консольный Eco-клиент, получает Lab1, Lab2 и A–E через шину, выполняет 108 проверок и возвращает ненулевой код при любой ошибке.

Eco UnitTest использует реальные factory и `IEcoInterfaceBus1`; тестовый allocator применяется только в Host UnitTest для проверки контракта памяти.

Полный локальный контроль выполняется командой `make check`: Host UnitTest, строгая контрольная C89-компиляция и проверка отсутствия libc. Независимые эталоны запускаются через `make reference`; MATLAB-скрипт находится в `DesignFiles/Reference/polyfit_reference.m`, а SciPy-проверка выполняется в отдельном окружении. При установленном MATLAB вывод сохраняется в `DesignFiles/Reference/matlab_output.txt`, а сравнение выполняется `python3 DesignFiles/Reference/compare_matlab.py`.

Фактическое сравнение вывода компонента со SciPy 1.18.1 выполнено скриптом `DesignFiles/Reference/compare_scipy.py`: максимальная абсолютная ошибка для квадратного и кубического тестов составила `6.83e-15`. MATLAB R2026b также выполнен; статус и вывод сохранены в `DesignFiles/Reference/matlab_status.txt` и `DesignFiles/Reference/matlab_output.txt`.

Платформенная матрица Linux `TARGET=0/1 × DEBUG=0/1` запускается командой `make matrix` после установки официального `ECO_TOOLCHAIN`. Без кросс-компилятора скрипт завершает работу с явным статусом `SKIPPED`; локальная macOS-проверка не выдаётся за Linux-бинарный результат.

Для Xcode откройте оба проекта из `AssemblyFiles/Mac/Xcode_v123/`, задайте `ECO_FRAMEWORK` на распакованный `Eco.Framework_20260918.zip`, соберите компонент в схемах `Static` и `Dynamic` для `Debug` и `Release`, а UnitTest — в `Static/Debug` и `Static/Release`. Команды приведены в `XCODE_SETUP.md`. После динамической сборки запустите `DesignFiles/Report/check_dynamic_deps.sh`.

## Полная сборка через терминал

Сначала укажите путь к распакованному Eco Framework:

```sh
export ECO_FRAMEWORK="/Users/danial2006/Downloads/Eco.Framework"
cd "/Users/danial2006/Documents/ChatGPT/Лабораторная работа №1"
```

Локальные тесты, строгая C89-компиляция, проверка отсутствия libc и sanitizer:

```sh
make check
```

Интеграционный Eco UnitTest через настоящий `IEcoSystem1` и `IEcoInterfaceBus1`:

```sh
make ECO_FRAMEWORK="$ECO_FRAMEWORK" eco-test
```

Компонент Static Debug:

```sh
xcodebuild \
  -project AssemblyFiles/Mac/Xcode_v123/EcoLab1.xcodeproj \
  -scheme Static -configuration Debug \
  -derivedDataPath BuildFiles/XcodeComponentStaticDebug \
  -jobs 1 ECO_FRAMEWORK="$ECO_FRAMEWORK" \
  CODE_SIGNING_ALLOWED=NO build
```

Компонент Static Release:

```sh
xcodebuild \
  -project AssemblyFiles/Mac/Xcode_v123/EcoLab1.xcodeproj \
  -scheme Static -configuration Release \
  -derivedDataPath BuildFiles/XcodeComponentStaticRelease \
  -jobs 1 ECO_FRAMEWORK="$ECO_FRAMEWORK" \
  CODE_SIGNING_ALLOWED=NO build
```

Компонент Dynamic Release:

```sh
xcodebuild \
  -project AssemblyFiles/Mac/Xcode_v123/EcoLab1.xcodeproj \
  -scheme Dynamic -configuration Release \
  -derivedDataPath BuildFiles/XcodeComponentDynamicRelease \
  -jobs 1 ECO_FRAMEWORK="$ECO_FRAMEWORK" \
  CODE_SIGNING_ALLOWED=NO build
```

UnitTest Static Debug:

```sh
xcodebuild \
  -project AssemblyFiles/Mac/Xcode_v123/EcoLab1UnitTest.xcodeproj \
  -scheme Static -configuration Debug \
  -derivedDataPath BuildFiles/XcodeUnitDebug \
  -jobs 1 ECO_FRAMEWORK="$ECO_FRAMEWORK" \
  CODE_SIGNING_ALLOWED=NO build
```

UnitTest Static Release:

```sh
xcodebuild \
  -project AssemblyFiles/Mac/Xcode_v123/EcoLab1UnitTest.xcodeproj \
  -scheme Static -configuration Release \
  -derivedDataPath BuildFiles/XcodeUnitRelease \
  -jobs 1 ECO_FRAMEWORK="$ECO_FRAMEWORK" \
  CODE_SIGNING_ALLOWED=NO build
```

Не запускайте одновременно сборку одного проекта в Xcode и терминале. Если появляется `database is locked`, остановите текущую сборку и удалите только её производный каталог, например `rm -rf BuildFiles/XcodeUnitDebug`, затем повторите команду.

Для macOS arm64 отдельные Dynamic-артефакты двух реализаций polyfit и внешнего Lab2 factory создаются скриптом `DesignFiles/Report/build_dynamic_runtime_macos.sh`:

```sh
ECO_FRAMEWORK="$ECO_FRAMEWORK" DesignFiles/Report/build_dynamic_runtime_macos.sh
```

Обычный файл экспортирует фабрику `CID_EcoPolynomialMath`, QR-файл собирается с `POLYFIT_QR_ONLY` и экспортирует фабрику `CID_EcoPolynomialMathQR`, а Lab2-файл собирается с `ECO_LAB2_ONLY` и экспортирует `GetEcoLab2Factory`. Перед запуском динамического Eco-клиента нужный файл нужно разместить в каталоге runtime, который использует `ECO_FRAMEWORK_RT`.

## Структура

- `SourceFiles` — численное ядро, фабрика и регистрация на шине;
- `HeaderFiles` и `SharedFiles` — закрытые и публичные интерфейсы, IID/CID и коды ошибок;
- `UnitTestFiles` — Host UnitTest и консольный Eco-клиент;
- `AssemblyFiles` — проекты Xcode;
- `DesignFiles/Report` — отчёт и проверочные материалы;
- `DesignFiles/Prompt` — системный промпт и протокол его улучшения.

Перегрузки объявляются единым повторно подключаемым шаблоном `HeaderFiles/polyfit_core_template.h`. Проверка запретов libc выполняется скриптами `DesignFiles/Report/check_no_libc.sh` и `DesignFiles/Report/check_dynamic_deps.sh`. Исходники проекта сохранены в UTF-8 with signature и CRLF; shell-скрипты используют LF.
