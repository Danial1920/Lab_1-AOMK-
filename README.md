# Eco Lab1 и Lab2 по ACOM

Проект на ANSI C89 для Eco Framework. Lab1 выполняет задание 44 (`polyfit`): нормальные уравнения с методом Гаусса и потоковый QR/Givens доступны через разные CID. Lab2 добавляет калькуляторные интерфейсы и демонстрирует включение и агрегирование компонентов A–E и Eco.Lab1.

## Архитектура Lab2

В интеграционном Eco-режиме все дочерние объекты создаются через настоящий `IEcoInterfaceBus1::QueryComponent`. Host-тест использует прямые конструкторы тех же компонентов с явно обозначенным тестовым allocator; подмены шины нет. Калькуляторные фасады вызывают методы дочерних интерфейсов.

```text
Eco.Lab2
├── агрегирует Eco.Lab1
│   ├── агрегирует B → публичный CalculatorX
│   └── включает E → фасад CalculatorY / CalculatorSafe
│       └── агрегирует свой B
├── включает A либо C с тем же CID → Addition в Horner
└── включает D → Multiplication в Horner
    └── включает свой B (при отсутствии B — A/C)
```

| Компонент | Интерфейсы | Реальное взаимодействие |
|---|---|---|
| A | X | Самостоятельные Addition/Subtraction |
| B | X | Самостоятельное создание или агрегирование |
| C | X/Y/Safe | Независимая реализация, заменяющая A по тому же CID |
| D | X/Y/Safe | Включает B, при отсутствии B — A/C; X вызывает дочерний объект |
| E | X/Y/Safe | Агрегирует B; Y реализует самостоятельно |
| Eco.Lab1 | Lab1, PolynomialMath, X/Y/Safe | Агрегирует B, включает E (если E недоступен — D), сохраняет polyfit |
| Eco.Lab2 | Lab2, Lab1, PolynomialMath, X/Y/Safe | Агрегирует Lab1; Horner вызывает включённые A/C и D |

При агрегировании фабрика принимает `outer` только с `IID_IEcoUnknown` и возвращает приватный неделегирующий интерфейс. Владелец освобождает его через `Release`. Публичные `QueryInterface/AddRef/Release` делегируют controlling unknown. У `Lab2 → Lab1 → B` один внешний unknown; X возвращается непосредственно из B. Включённые объекты сохраняют отдельную identity. Дочерние ссылки освобождаются до system/allocator. Резервные зависимости выбираются только при отсутствии компонента/интерфейса, а не при нехватке памяти.

Схема и контракты сверены с [InsideACOM](https://github.com/peerf-eco/Eco.Education/tree/main/Eco.Pro.Training/001.InsideACOM) и [Zzzpize/EcoLab1](https://github.com/Zzzpize/EcoLab1) (commit `57d07171c5c92b492ac3f1fd48b93ce2155dd67c`). Сохранены собственные polyfit-алгоритмы. Поведение identity проверяется отдельно: совпадение структуры каталогов не заменяет проверку ACOM-контрактов.

## Интерфейсы и идентификаторы

Официальный X: `Addition(int16_t, int16_t) → int32_t`, `Subtraction → int16_t`. Официальный Y: `Multiplication → int32_t`, `Division → int16_t`. В каждой таблице пять методов, включая QI/AddRef/Release. Безопасное деление вынесено в отдельный `IEcoCalculatorSafe::TryDivision`, не меняющий таблицу Y.

| Назначение | IID/CID |
|---|---|
| IID CalculatorX | `93221116-2248-4742-AE06-82819447843D` |
| IID CalculatorY | `BD6414C2-9096-423E-A90C-04D77AFD1CAD` |
| IID CalculatorSafe | `D3B3E2F1-8D21-4A6B-9A40-440000000054` |
| IID Lab1 / Lab2 | `D3B3E2F1-8D21-4A6B-9A40-440000000052` / `...0055` |
| CID Lab1 normal / QR | `D3B3E2F1-8D21-4A6B-9A40-440000000045` / `...0046` |
| CID A и C (одинаковый) | `4828F655-2E45-40E7-8121-EBD220DC360E` |
| CID B | `AE202E54-3CE5-4550-8996-03BD70C62565` |
| CID D | `3A8E4467-7E82-475C-B4A3-719ED8397E61` |
| CID E | `872FEF1D-E331-4B87-AD44-D1E7C232C2F0` |
| CID Lab2 | `D3B3E2F1-8D21-4A6B-9A40-440000000065` |

A и C устанавливаются по очереди: семь уникальных CID в каждом запуске, восемь отдельных артефактов с учётом обеих альтернатив. `TryDivision` обнуляет out и возвращает 1101 при делении на ноль, 1102 при `-32768 / -1`. Обычный Division возвращает 0 при этих ошибках; целочисленное деление округляется к нулю. Subtraction явно приводит результат по модулю 65536.

`IEcoLab2::Horner` вычисляет полином с коэффициентами от старшей степени. Вызовы умножения/сложения идут через D.Y и A/C.X. Из-за аргументов int16 промежуточные значения перед передачей калькулятору должны помещаться в int16; последний результат Addition может иметь диапазон int32. При переполнении возвращается 1102 и нулевой результат. Добавление Horner сопровождается новым IID Lab2 (`...0055`). Старые пользовательские double-интерфейсы калькулятора заменены; контракты polyfit сохранены. Полные объявления — `SharedFiles/*.h` и `EcoLab2.idl`.

## Подготовка macOS arm64

Установите Xcode и распакуйте официальный архив преподавателя `Eco.Framework_20260918.zip`. Исходники Framework менять не требуется.

```sh
cd "/Users/danial2006/Documents/ChatGPT/Лабораторная работа №1"
export ECO_FRAMEWORK="/Users/danial2006/Downloads/Eco.Framework"
```

Для другого расположения задайте свой путь. Компоненты не используют `math.h`, стандартные заголовки C и прямые вызовы libc. Память, копирование и заполнение выполняются через `IEcoMemoryAllocator1`. Стандартный вывод используется только консольными клиентами: официальной `Eco.StdIO.C89` arm64 в предоставленном архиве нет.

Проверочным скриптам нужны `python3`, `make`, `clang` и `nm`. Установка ripgrep (`rg`) не требуется: исходники на запрещённые зависимости проверяются стандартной библиотекой Python, внешние символы — через `nm`. Все четыре основные команды ниже дополнительно проверены с системным PATH без `rg`.

## Проверка из терминала

```sh
make ECO_FRAMEWORK="$ECO_FRAMEWORK" check
make ECO_FRAMEWORK="$ECO_FRAMEWORK" eco-test-variants
CONFIGURATION=Release make ECO_FRAMEWORK="$ECO_FRAMEWORK" component-clients
```

Host проверяет численное ядро, контракты и освобождение памяти. Интеграционный клиент получает A–E/Lab1/Lab2 через реальные фабрики и Eco Interface Bus, проверяет матрицы QueryInterface, identity, счётчики ссылок и ошибочные пути. Подмены bus, фабрик или результатов QueryInterface нет. Явно помеченный тестовый allocator используется для подсчёта выделений и детерминированных отказов. `check` также выполняет строгую C89-компиляцию, проверку отсутствия libc, sanitizer для Host и реального Eco-клиента в вариантах A/C и независимый клиент Debug. При ошибке команда завершается ненулевым кодом.

## Независимый клиент и замена A на C

`make component-clients` собирает отдельные архивы A/B/C/D/E/Lab1/Lab1QR/Lab2 и один объект клиента `Lab2Client.o`. Клиент использует неизменённые учебные X/Y и CID-заголовки из `UnitTestFiles/ExternalContracts`; `HeaderFiles` и серверные C-файлы в его компиляцию не входят. Один и тот же объект линкуется сначала с A, затем с C. SHA256 объекта проверяется после каждого запуска. B/D/E/Lab1/Lab2 не перекомпилируются при замене A на C.

Артефакты: `BuildFiles/Components/Debug` и `Release`. Запуск уже собранных клиентов:

```sh
ECO_EXPECT_CALCULATOR=A BuildFiles/Components/Debug/EcoLab2ClientA
ECO_EXPECT_CALCULATOR=C BuildFiles/Components/Debug/EcoLab2ClientC
```

Клиент проверяет все семь CID, операции, полную матрицу QI, identity, polyfit, Horner и сохранение B.X после Release исходного Lab2. Отличие A/C проверяется по наличию Y. Переменная окружения задаёт ожидаемую установленную реализацию; результаты операций вычисляются реально.

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

Сборка сама по себе не запускает UnitTest. С указанным `derivedDataPath` исполняемые файлы находятся в `BuildFiles/XcodeUnitDebug/Build/Products/Debug/Static` и `BuildFiles/XcodeUnitRelease/Build/Products/Release/Static`. Выполните оба файла последовательно для запуска тестов. Xcode Static сохраняет общий архив для совместимости проекта; отдельную поставку компонентов доказывает `component-clients`. Не запускайте одновременно две сборки с одним `derivedDataPath`: при `database is locked` остановите конкурирующую сборку или используйте новый отдельный каталог.

## Настоящий Dynamic runtime

Последовательно выполните четыре конфигурации:

```sh
for configuration in Debug Release; do
  for variant in A C; do
    ECO_FRAMEWORK="$ECO_FRAMEWORK" CONFIGURATION="$configuration" \
      CALCULATOR_VARIANT="$variant" DesignFiles/Report/build_dynamic_runtime_macos.sh || break 2
  done
done
```

Скрипт собирает в Xcode **семь** библиотек с именами CID: Lab1 normal/QR, A либо C, B, D, E, Lab2. A/C занимают одно имя файла по очереди в отдельных runtime-каталогах. Выбор экспортируемой фабрики: обычная, `POLYFIT_QR_ONLY`, `ECO_CALCULATOR_KIND=0..4` либо `ECO_LAB2_ONLY`.

`DynamicRuntimeTest.c` использует общий публичный клиент, линкуется только с системными библиотеками Framework и не регистрирует лабораторные фабрики. Все компоненты получает официальный загрузчик через `QueryComponent` и `ECO_FRAMEWORK_RT`. Проверки `DYLD_PRINT_LIBRARIES` подтверждают загрузку каждой из семи dylib; процесс с пустым runtime обязан завершиться ошибкой. Для каждой библиотеки выполняются `nm -u` и `otool -L`. Артефакты: `BuildFiles/DynamicRuntime/<Debug|Release>/<A|C>`, логи: `BuildFiles/DynamicRuntimeLogs/<Debug|Release>/<A|C>`. Framework вне проекта не изменяется.

## Фактическая проверка 28 октября 2026

| Проверка | Результат |
|---|---|
| Host | 180 условий, 0 ошибок |
| Eco A / C, включая ASan и UBSan | 1062 / 1110 условий, 0 ошибок |
| Allocator в каждом Eco-варианте | 267 выделений / 267 освобождений / 0 живых |
| Независимый клиент Debug и Release | A: 523, C: 559 условий; 0 ошибок |
| Dynamic Debug/Release × A/C | 523 / 559; все 7 dylib загружены, пустой runtime отклонён |
| Xcode Static Debug/Release | Компонент и UnitTest собраны; UnitTest: 1062 / 0 |
| C89, libc, сборочные зависимости и отрицательные мутации | Пройдены |

Счётчики — число условий, не процент покрытия и не гарантия оценки. Логи текущей проверки сохранены в `DesignFiles/Review/Verification_2026-10-28`. Предыдущий аудит `Zzzpize_comparison_2026-10-28.md` описывает состояние **до** исправлений.

## Polyfit и независимые эталоны

Lab1 поддерживает `double`, `float`, `long double`, `int`, `long`, `Add/Subtract`, `FitVector` и `IEcoPolynomial`. Коэффициенты возвращаются от старшей степени к свободному члену. Центрирование и масштабирование выполняются перед решением, затем коэффициенты переводятся обратно.

Для целочисленных входов проверяется точность преобразования во внутренний `long double`. На macOS arm64 он имеет точность `double`, поэтому некоторые 64-битные `long` (например, `2^53 + 1`) не представимы точно. Такие входы отклоняются с `ERR__POLYFIT_PRECISION` (1005), коэффициенты обнуляются; полный диапазон `long` без потери точности на этой платформе не заявляется. При высокой степени нормальные уравнения менее устойчивы, чем QR; допуски независимых сравнений относятся к наборам, перечисленным в `reference_cases.py`.

Скрипты и фактические сравнения MATLAB/SciPy находятся в `DesignFiles/Reference`. Компараторы заново собирают компонент и рассчитывают независимые эталоны, проверяя все коэффициенты девяти наборов данных для normal и QR. Для SciPy/NumPy используются заранее заданные допуски и записанный протокол обусловливания входной задачи. Для повторного запуска выберите Python с SciPy/NumPy:

```sh
make PYTHON=/opt/anaconda3/bin/python reference
make PYTHON=/opt/anaconda3/bin/python matlab-reference
```

Вместо `/opt/anaconda3/bin/python` можно указать свой Python с установленными SciPy/NumPy. `matlab-reference` запускает установленный `/Applications/MATLAB_R2026b.app/bin/matlab`; иной путь задаётся переменной `MATLAB`. Сохранённый прогон от 25 октября 2026: SciPy 1.15.3/NumPy 2.1.3 проверил 36 массивов коэффициентов, MATLAB R2026b — 18. Валидатор эталонов отдельно прошёл восемь проверок, включая неполные, дублированные, нечисловые и ошибочные результаты. Эти проверки не подменяют ACOM-тесты ЛР2.

## Платформы и структура

Целевая проверенная платформа — macOS arm64. В Linux/Windows/EcoOS обновлены списки исходников; это не означает, что их бинарная сборка выполнена. `make matrix` требует официальный `ECO_TOOLCHAIN` и явно возвращает `SKIPPED`, если он отсутствует.

- `SourceFiles` — ядро, ACOM-компоненты, фабрики и регистрация.
- `HeaderFiles`, `SharedFiles` — закрытые/публичные интерфейсы, IID/CID, IDL.
- `UnitTestFiles` — Host, Eco и отдельный Dynamic-клиент.
- `AssemblyFiles` — проекты Xcode и платформенные сборочные файлы.
- `DesignFiles/Report/EcoLab2_Report.docx` — отчёт ЛР2; корневая копия идентична.
- `DesignFiles/Prompt` — системные промпты, вне Word-отчёта.
- `COMPLIANCE.md`, `DesignFiles/Reference/test_matrix.md` — контроль требований и матрица проверок.
