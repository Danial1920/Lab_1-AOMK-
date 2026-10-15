# Матрица соответствия системному промпту

| Требование | Состояние | Подтверждение или ограничение |
|---|---|---|
| ACOM-интерфейс, фабрика, два CID | Выполнено | `IEcoPolynomialMath`, `IEcoPolynomial`, `IEcoInterfaceBus1` |
| Память через `IEcoMemoryAllocator1` | Выполнено | `check_no_libc.sh`, Host allocator tests |
| C89 и запрет libc в компоненте | Выполнено | `make strict`, `make check-no-libc` |
| UTF-8 BOM/CRLF и шапки файлов | Выполнено | проверено скриптом окружения и исходниками |
| Generic-перегрузки | Частично | объявления вынесены в `polyfit_core_template.h`; тела генерируются макросом в `.c` |
| Обнуление out-параметров и проверки ошибок | Выполнено | Host 72 и Eco 76 проверки |
| `nm -u`/ABI-аудит | Выполнено для macOS | `nm -u` пуст на Dynamic dylib; `otool -L` показывает platform `libSystem` |
| Linux `readelf -d` | Не подтверждено | нужен ELF `.so` и Linux/Eco toolchain |
| Static/Dynamic Debug/Release | Выполнено на macOS | Xcode component и UnitTest build succeeded |
| Linux/EcoOS/ARM matrix | Не подтверждено | `make matrix` возвращает `SKIPPED` без `ECO_TOOLCHAIN` |
| MATLAB/SciPy | SciPy выполнен | SciPy 1.18.1 выполнен; сравнение с компонентом max_abs_error = 6.83e-15; MATLAB/Octave отсутствуют в текущем окружении |
| Итог EcoMain | Выполнено | `Проверок: 76, ошибок: 0`, ненулевой код при ошибке |
| Benchmark | Не добавлен | в отчёте приведены тестовые объёмы, допуски и scaling graph |
| Монотонность узлов | Не применимо | это требование spline, а не `polyfit` |
