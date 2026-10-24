# Матрица проверок

## Лабораторная работа №2

| Группа | Проверки |
|---|---|
| A–E | Получение пяти компонентов через CID и Interface Bus |
| CalculatorX | Addition/Subtraction |
| CalculatorY | Multiplication/Division/TryDivision |
| QueryInterface | Lab1↔X/Y, Lab2→Lab1/X/Y, неизвестный IID |
| Aggregation | controlling unknown для B/E/Lab1 |
| Контроль | AddRef/Release, деление на ноль, ненулевой код ошибки |
| Eco integration | 103 проверки, 103 успешных |

| Группа | Проверки |
|---|---|
| Численный результат | Квадратичный, кубический, постоянный и линейный наборы |
| Устойчивость | Центрирование и масштабирование больших `x` |
| Типы | `double`, `float`, `long double`, `int`, `long` |
| API | `Add`, `Subtract`, `FitVector`, `IEcoPolynomial::Evaluate` |
| ACOM | IID, AddRef/Release, неизвестный IID, две фабрики и два CID |
| Ошибки | NULL, недостаток точек, недопустимая степень, сингулярность, NaN |
| Сборка | C89 Host, строгий C89, Eco Interface Bus, Xcode Static/Dynamic Debug/Release |
| Эталон | `reference_polyfit.py`, `polyfit_reference.m`, optional `reference_scipy.py` |
| ABI | `nm -u` для объектов и Dynamic dylib, `readelf -d` на Linux или `otool -L` на macOS |
| Контракт клиента | EcoMain печатает `Проверок: N, ошибок: K` и возвращает ненулевой код при ошибке |
