# Соответствие требованиям

## ACOM и память

- Все публичные методы принимают `me`, проверяют входы и возвращают `int16_t`.
- `QueryInterface` поддерживает `IID_IEcoPolynomialMath`, `IID_IEcoPolynomial` и `IID_IEcoUnknown` и увеличивает счётчик ссылок.
- Фабрика отклоняет агрегацию, освобождает временные ссылки на шину и allocator.
- Экземпляр компонента удерживает собственную ссылку на `IEcoSystem1` и освобождает её до allocator.
- Объект `IEcoPolynomial` владеет собственной ссылкой на allocator и освобождается ровно один раз.
- Рабочие буферы получаются только через `IEcoMemoryAllocator1`.

## Ограничения C89

Компонент собирается с `-std=c89 -fno-stack-protector`. Контрольная команда `make strict` добавляет `-pedantic -Wall -Wextra -Wcast-qual -Werror`; единственное подавленное предупреждение относится к legacy anonymous unions в заголовке Eco.Core1. В `SourceFiles`, `HeaderFiles` и `SharedFiles` нет стандартных заголовков C, `math.h`, прямых вызовов libc или динамического выделения памяти из libc.

Перегрузки объявляются из `HeaderFiles/polyfit_core_template.h`, который подключается пять раз через `_REAL`, `_OUT` и `_FN`. Это исключает расхождение сигнатур между типами. Все исходники проекта имеют заголовок с кодировкой, назначением и автором; C/H-файлы хранятся в UTF-8 with signature и CRLF.

## Реальные проверки

- `make test` — 72 проверки Host UnitTest;
- `make ECO_FRAMEWORK=/path/to/Eco.Framework eco-test` — 76 проверок обеих реализаций через Eco Interface Bus;
- Xcode Static и Dynamic Debug/Release для компонента, Static Debug/Release для UnitTest;
- macOS Dynamic runtime собирается двумя артефактами: обычным и QR с отдельной экспортируемой фабрикой;
- `DesignFiles/Report/check_no_libc.sh` — поиск запрещённых включений/вызовов и `nm -u` для объектных файлов компонента.
- `DesignFiles/Report/check_dynamic_deps.sh` — `readelf -d` на Linux или `otool -L` на macOS и проверка неопределённых символов через `nm -u`.
- `DesignFiles/Report/check_sanitizers.sh` — AddressSanitizer и UndefinedBehaviorSanitizer для Host UnitTest.
- `make matrix` — Linux `TARGET=0/1 × DEBUG=0/1` при наличии официального кросс-компилятора; без него статус явно `SKIPPED`.

Пути к бинарным библиотекам Eco Framework задаются через `ECO_FRAMEWORK`. Framework не копируется в репозиторий и должен быть распакован из официального архива преподавателя. На macOS arm64 архив не содержит `Eco.StdIO.C89`; поэтому итоговая печать Eco-клиента выполняется стандартным выводом клиентского процесса, а компонент остаётся без libc.
