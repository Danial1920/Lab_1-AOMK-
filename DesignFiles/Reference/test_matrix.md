# Матрица проверок Lab1 и Lab2

| Набор | Проверяемое поведение | Способ выполнения |
|---|---|---|
| Host Lab1 | Полиномы, типы, нулевые указатели, ошибки, allocator | make test |
| Host ACOM contracts | Интерфейсные контракты и очистка памяти | make test |
| Eco A–E/Lab1/Lab2 | Получение всех восьми CID через реальные factory и bus | make eco-test |
| QueryInterface | Все пары поддерживаемых интерфейсов, self, unknown, неизвестный IID | Eco integration |
| Включение | Lab1→C→A/D→B, вызовы дочерних операций | Eco integration |
| Агрегирование | E→B, Lab2→Lab1; приватный unknown, контролирующий unknown | Eco integration |
| Reference lifetime | Баланс AddRef/Release, ранний Release корня, живые дочерние интерфейсы | Eco integration |
| Fault injection | Отказ allocator, неизвестный IID в factory, очистка частичного графа | Eco integration |
| Деление | Отрицательные/дробные/нулевые аргументы, TryDivision с нулём/NULL | Host + Eco |
| Численные регрессии | Обе реализации, пять типов, FitVector, IEcoPolynomial | Host + Eco |
| C89 | Компонентные исходники, строгие предупреждения | make strict |
| Отсутствие libc | Запрещённые заголовки/вызовы, nm undefined symbols | make check-no-libc |
| Sanitizer | AddressSanitizer/UBSan для Host и реального Eco-клиента | make sanitize |
| Xcode Static | Компонент и UnitTest Debug/Release | README commands |
| Xcode Dynamic | Восемь отдельных CID-файлов Debug/Release | build_dynamic_runtime_macos.sh |
| Dynamic integration | Официальный bus loader, операции, identity; 44 условия | DynamicRuntimeTest.c |
| Dynamic dependency | nm/otool для каждой dylib, DYLD trace всех восьми | Runtime script |
| Отсутствующий runtime | Отдельный процесс без dylib обязан вернуть ошибку | Runtime script |
| MATLAB/SciPy | Независимые численные эталоны Lab1 | make reference / matlab-reference с Python, содержащим SciPy/NumPy |
| Linux matrix | TARGET 0/1 × DEBUG 0/1 | SKIPPED без официального ECO_TOOLCHAIN |

Полный прогон: `make ECO_FRAMEWORK="$ECO_FRAMEWORK" check eco-test`. Количества проверок и ошибки выводятся в консоль; тест возвращает ненулевой код при ошибке или нарушении ожидаемого плана. Сборочные инструкции платформ за пределами macOS не являются свидетельством выполненного запуска.

Последний полный прогон: Host 167/0, Eco 953/0, tracked allocator 348/348/live 0; Dynamic Debug и Release по 44/0; валидатор эталонов 8/8. Числа являются счётчиками проверенных условий, а не процентом покрытия.
