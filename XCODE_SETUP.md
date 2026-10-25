# Xcode на macOS arm64

Полные команды сборки и запуска находятся в [README](README.md#xcode-static-debug-и-release), команды Dynamic runtime — в [соответствующем разделе](README.md#настоящий-dynamic-runtime).

Для обоих проектов задайте `ECO_FRAMEWORK` в Xcode Settings → Locations → Custom Paths, указав распакованный официальный архив. Откройте `AssemblyFiles/Mac/Xcode_v123/EcoLab1.xcodeproj` и `EcoLab1UnitTest.xcodeproj`.

| Проект | Схема | Конфигурации | Назначение |
|---|---|---|---|
| EcoLab1 | Static | Debug, Release | Одна статическая библиотека всех компонентов Lab1/Lab2/A–E |
| EcoLab1 | Dynamic | Debug, Release | Одна экспортируемая фабрика; для полного runtime нужен скрипт восьми CID |
| EcoLab1UnitTest | Static | Debug, Release | Консольный интеграционный UnitTest |

Изменённые исходники: `CEcoLab1GeneratedFactory.c`, `CalculatorComponents.c`, `EcoLab2.c`, `polyfit_bus.c`. Численное ядро сохранено. Для отдельного CID Dynamic-сборка использует селекторы `POLYFIT_QR_ONLY`, `ECO_CALCULATOR_KIND=0..4` или `ECO_LAB2_ONLY`.

Сборка UnitTest не равна запуску: для воспроизводимого запуска используйте `make ECO_FRAMEWORK="$ECO_FRAMEWORK" eco-test`. Для реальной загрузки dylib — `DesignFiles/Report/build_dynamic_runtime_macos.sh`, который сам собирает и запускает отдельный клиент.

Каждый процесс Xcode должен иметь отдельный `derivedDataPath`; скрипт runtime использует каталог для каждого CID/конфигурации и `-jobs 1`. Не запускайте одновременно IDE и терминал с одним каталогом. Если build.db занят, остановите конкурирующую сборку либо задайте новый каталог.

Системные библиотеки берутся из официального Framework без изменений. В предоставленном архиве отсутствует Eco.StdIO.C89 arm64; stdio используется только консольными клиентами.
