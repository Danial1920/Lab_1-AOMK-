# Проверка после исправления архитектуры — 28 октября 2026

Логи относятся к рабочим исходникам поверх 4c85688. Хеши исходников, заголовков, клиентов и сборочных скриптов сохранены в source-sha256.json. Это состояние до нового коммита; исторический аудит старого ABI находится рядом в Zzzpize_comparison_2026-10-28.md.

## Фактически выполнено

- `make ECO_FRAMEWORK=/Users/danial2006/Downloads/Eco.Framework check validation-build`: eco-final-check.log. Host 180/0; sanitizer Eco A 1062/0, C 1110/0, allocator 267/267/live 0; независимый Debug-клиент A 523/0, C 559/0, одинаковый SHA256 клиентского объекта.
- `CONFIGURATION=Release make component-clients`: eco-public-release.log; те же 523/559 условий без ошибок.
- Xcode Static Component и UnitTest, Debug и Release: eco-static-component-*.log и eco-static-unit-*.log. Фактический запуск исполняемых UnitTest: eco-static-run-*.log, по 1062/0.
- Xcode Dynamic Debug/Release × CALCULATOR_VARIANT=A/C: eco-final-dynamic-*.log; все четыре запуска успешны. Полные сборки, nm/otool, loader trace и отрицательные запуски — DynamicRuntimeLogs/<конфигурация>/<вариант>. По семь CID в каждом runtime.
- Повторный `make validation-build` с дополнительными адресными мутациями A.Addition и D.Multiplication: eco-final-mutations.log. Обе мутации обнаружены тестом Horner публичного клиента; мутации выполнялись только во временной копии.
- Word: 8 страниц после рендеринга, проверены визуально, исправлены отсутствующие нижние индексы в схеме. Канонический и корневой DOCX идентичны; в footer только PAGE.

## Что не утверждается

MATLAB/SciPy не перезапускались в этом исправлении; сохранённые эталоны относятся к 25 октября. Бинарные сборки других ОС не выполнялись. Счётчики условий не обозначают процент покрытия. Общий Xcode Static-архив сохранён; раздельную поставку доказывает независимая сборка архивов, а не общий UnitTest target.
