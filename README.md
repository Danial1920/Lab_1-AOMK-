# Eco Lab1 и Lab2 по ACOM

Проект на ANSI C89 для Eco Framework. Lab1 выполняет задание 44 (`polyfit`): нормальные уравнения с методом Гаусса и потоковый QR/Givens доступны через разные CID. Lab2 добавляет калькуляторные интерфейсы и демонстрирует включение и агрегирование компонентов A–E и Eco.Lab1.

## Архитектура Lab2

Все дочерние объекты создаются через настоящий `IEcoInterfaceBus1::QueryComponent`. Калькуляторные фасады вызывают методы дочерних интерфейсов.

```text
Eco.Lab2
├── агрегирует Eco.Lab1 через внутренний неделегирующий IEcoUnknown
│   └── включает C
│       ├── включает A: Addition
│       └── включает D: Subtraction, Multiplication, Division
│           └── включает B: Addition, Subtraction
└── включает E: фасад CalculatorY
    └── агрегирует B: CalculatorX
```

| Компонент | Интерфейсы | Реальное взаимодействие |
|---|---|---|
| A | X | Непосредственно выполняет сложение/вычитание |
| B | X | Поддерживает самостоятельное создание и агрегацию |
| C | X/Y | Включает A и D; сложение вызывает A, вычитание и Y вызывает D |
| D | X/Y | Включает B и делегирует ему X; самостоятельно выполняет Y |
| E | X/Y | Агрегирует B через внутренний unknown; Y реализует самостоятельно |
| Eco.Lab1 | Lab1, PolynomialMath, X/Y | Сохраняет polyfit, включает C, поддерживает внешнюю агрегацию |
| Eco.Lab2 | Lab2, Lab1, PolynomialMath, X/Y | Агрегирует Lab1, включает E; внешний Y вызывает E |

При агрегировании фабрика принимает `outer` только с `IID_IEcoUnknown` и возвращает приватный неделегирующий интерфейс. Владелец освобождает его через `Release`. Публичные `QueryInterface/AddRef/Release` делегируют controlling unknown; запросы между всеми поддерживаемыми интерфейсами сохраняют единую identity. Для включения сохраняется отдельная identity дочернего объекта.

Учебные D/E из [InsideACOM](https://github.com/peerf-eco/Eco.Education/tree/main/Eco.Pro.Training/001.InsideACOM) использованы как архитектурный ориентир. Здесь согласованный API использует `double` и новые IID, поэтому он не подменяет бинарно совместимые целочисленные интерфейсы уроков. `TryDivision` возвращает статус и обнуляет результат при делении на ноль. Учебный `Division` возвращает числовой результат; для диагностики ошибки используется `TryDivision`.

## Интерфейсы и идентификаторы

`IEcoCalculatorX` предоставляет `Addition/Subtraction`, `IEcoCalculatorY` — `Multiplication/Division/TryDivision`. Контракты находятся в `SharedFiles/*.h` и `SharedFiles/EcoLab2.idl`.

| Назначение | IID/CID |
|---|---|
| IID CalculatorX / CalculatorY | `D3B3E2F1-8D21-4A6B-9A40-440000000050` / `...0051` |
| IID Lab1 / Lab2 | `D3B3E2F1-8D21-4A6B-9A40-440000000052` / `...0053` |
| CID Lab1 normal / QR | `D3B3E2F1-8D21-4A6B-9A40-440000000045` / `...0046` |
| CID A / B / C / D / E / Lab2 | `D3B3E2F1-8D21-4A6B-9A40-440000000060` / `...0061` / `...0062` / `...0063` / `...0064` / `...0065` |

## Подготовка macOS arm64

Установите Xcode и распакуйте официальный архив преподавателя `Eco.Framework_20260918.zip`. Исходники Framework менять не требуется.

```sh
cd "/Users/danial2006/Documents/ChatGPT/Лабораторная работа №1"
export ECO_FRAMEWORK="/Users/danial2006/Downloads/Eco.Framework"
```

Для другого расположения задайте свой путь. Компоненты не используют `math.h`, стандартные заголовки C и прямые вызовы libc. Память, копирование и заполнение выполняются через `IEcoMemoryAllocator1`. Стандартный вывод используется только консольными клиентами: официальной `Eco.StdIO.C89` arm64 в предоставленном архиве нет.

## Проверка из терминала

```sh
make ECO_FRAMEWORK="$ECO_FRAMEWORK" check
make ECO_FRAMEWORK="$ECO_FRAMEWORK" eco-test
```

Host проверяет численное ядро, контракты и освобождение памяти. Интеграционный клиент получает A–E/Lab1/Lab2 через реальные фабрики и Eco Interface Bus, проверяет матрицы QueryInterface, identity, счётчики ссылок и ошибочные пути. Подмены bus, фабрик или результатов QueryInterface нет. Явно помеченный тестовый allocator используется для подсчёта выделений и детерминированных отказов. `check` также выполняет строгую C89-компиляцию, проверку отсутствия libc и sanitizer для Host и реального Eco-клиента. При ошибке команда завершается ненулевым кодом.

## Xcode Static Debug и Release

Оба проекта находятся в `AssemblyFiles/Mac/Xcode_v123`. В Xcode Settings → Locations → Custom Paths задайте `ECO_FRAMEWORK`. Компонент собирается схемами Static/Dynamic, клиент — Static. ЛР2 собирается теми же проектами с дополнительными исходниками, имя проекта сохранено для совместимости ЛР1.

Последовательно выполните:

```sh
for configuration in Debug Release; do
  xcodebuild -project AssemblyFiles/Mac/Xcode_v123/EcoLab1.xcodeproj \
    -scheme Static -configuration "$configuration" -jobs 1 \
    -derivedDataPath "BuildFiles/XcodeComponentStatic$configuration" \
    ECO_FRAMEWORK="$ECO_FRAMEWORK" CODE_SIGNING_ALLOWED=NO build

  xcodebuild -project AssemblyFiles/Mac/Xcode_v123/EcoLab1UnitTest.xcodeproj \
    -scheme Static -configuration "$configuration" -jobs 1 \
    -derivedDataPath "BuildFiles/XcodeUnit$configuration" \
    ECO_FRAMEWORK="$ECO_FRAMEWORK" CODE_SIGNING_ALLOWED=NO build
done
```

Сборка сама по себе не запускает UnitTest. С указанным `derivedDataPath` исполняемые файлы находятся в `BuildFiles/XcodeUnitDebug/Build/Products/Debug/Static` и `BuildFiles/XcodeUnitRelease/Build/Products/Release/Static`. Для запуска интеграционных проверок удобнее команда `make ... eco-test` выше. Не запускайте одновременно две сборки с одним `derivedDataPath`: при `database is locked` остановите конкурирующую сборку или используйте новый отдельный каталог.

## Настоящий Dynamic runtime

```sh
ECO_FRAMEWORK="$ECO_FRAMEWORK" CONFIGURATION=Release \
  DesignFiles/Report/build_dynamic_runtime_macos.sh
ECO_FRAMEWORK="$ECO_FRAMEWORK" CONFIGURATION=Debug \
  DesignFiles/Report/build_dynamic_runtime_macos.sh
```

Скрипт собирает в Xcode **восемь** библиотек с именем из реального 32-значного CID: две реализации Lab1, A–E и Lab2. Файлы размещаются в `BuildFiles/DynamicRuntime/Debug` или `Release`. Выбор экспортируемой фабрики: обычная, `POLYFIT_QR_ONLY`, `ECO_CALCULATOR_KIND=0..4` либо `ECO_LAB2_ONLY`.

Отдельный `DynamicRuntimeTest.c` линкуется только с официальными системными библиотеками. Он получает все восемь компонентов через `QueryComponent`; официальный загрузчик использует `ECO_FRAMEWORK_RT`, без ручной регистрации лабораторных фабрик. `DYLD_PRINT_LIBRARIES` подтверждает фактическую загрузку каждой dylib. Проверяются 44 условия; процесс с пустым runtime обязан завершиться ошибкой. Для каждой библиотеки выполняются `nm -u` и `otool -L`. Логи находятся в `BuildFiles/DynamicRuntimeLogs/<конфигурация>`; Framework за пределами проекта не изменяется.

## Polyfit и независимые эталоны

Lab1 поддерживает `double`, `float`, `long double`, `int`, `long`, `Add/Subtract`, `FitVector` и `IEcoPolynomial`. Коэффициенты возвращаются от старшей степени к свободному члену. Центрирование и масштабирование выполняются перед решением, затем коэффициенты переводятся обратно.

Скрипты и фактические сравнения MATLAB/SciPy находятся в `DesignFiles/Reference`. Компараторы заново собирают компонент и рассчитывают независимые эталоны, проверяя все коэффициенты девяти наборов данных для normal и QR. Для SciPy/NumPy используются заранее заданные допуски и записанный протокол обусловливания входной задачи. Для повторного запуска выберите Python с SciPy/NumPy:

```sh
make PYTHON=/opt/anaconda3/bin/python reference
make PYTHON=/opt/anaconda3/bin/python matlab-reference
```

Вместо `/opt/anaconda3/bin/python` можно указать свой Python с установленными SciPy/NumPy. `matlab-reference` запускает установленный `/Applications/MATLAB_R2026b.app/bin/matlab`; иной путь задаётся переменной `MATLAB`. Свежий прогон SciPy 1.15.3/NumPy 2.1.3 проверил 36 массивов коэффициентов, MATLAB R2026b — 18. Валидатор эталонов отдельно прошёл восемь проверок, включая неполные, дублированные, нечисловые и ошибочные результаты. Эти проверки не подменяют ACOM-тесты ЛР2.

## Платформы и структура

Целевая проверенная платформа — macOS arm64. В Linux/Windows/EcoOS обновлены списки исходников; это не означает, что их бинарная сборка выполнена. `make matrix` требует официальный `ECO_TOOLCHAIN` и явно возвращает `SKIPPED`, если он отсутствует.

- `SourceFiles` — ядро, ACOM-компоненты, фабрики и регистрация.
- `HeaderFiles`, `SharedFiles` — закрытые/публичные интерфейсы, IID/CID, IDL.
- `UnitTestFiles` — Host, Eco и отдельный Dynamic-клиент.
- `AssemblyFiles` — проекты Xcode и платформенные сборочные файлы.
- `DesignFiles/Report/EcoLab2_Report.docx` — отчёт ЛР2; корневая копия идентична.
- `DesignFiles/Prompt` — системные промпты, вне Word-отчёта.
- `COMPLIANCE.md`, `DesignFiles/Reference/test_matrix.md` — контроль требований и матрица проверок.
