# Сборка в Xcode

1. Установите Xcode.
2. Распакуйте официальный архив `Eco.Framework_20260918.zip` в удобный каталог.
3. Откройте `AssemblyFiles/Mac/Xcode_v123/EcoLab1.xcodeproj` и `EcoLab1UnitTest.xcodeproj`.
4. В Xcode Settings → Locations → Custom Paths добавьте `ECO_FRAMEWORK` со значением каталога распакованного Eco Framework.
5. Для компонента соберите схемы `Static` и `Dynamic` в конфигурациях `Debug` и `Release`. Для UnitTest доступны `Static/Debug` и `Static/Release`.

Для лабораторной работы №2 используйте ветку `lab2`. В ней те же Xcode-проекты собирают дополнительные исходники `CalculatorComponents.c` и `EcoLab2.c`; полный Eco UnitTest регистрирует Lab1, Lab2 и компоненты A–E и выполняет 103 проверки.

Проверка из терминала:

```sh
xcodebuild -project AssemblyFiles/Mac/Xcode_v123/EcoLab1.xcodeproj \
  -scheme Static -configuration Debug \
  -derivedDataPath BuildFiles/XcodeComponentDebug \
  ECO_FRAMEWORK=/path/to/Eco.Framework CODE_SIGNING_ALLOWED=NO build

xcodebuild -project AssemblyFiles/Mac/Xcode_v123/EcoLab1.xcodeproj \
  -scheme Static -configuration Release \
  -derivedDataPath BuildFiles/XcodeComponentRelease \
  ECO_FRAMEWORK=/path/to/Eco.Framework CODE_SIGNING_ALLOWED=NO build

xcodebuild -project AssemblyFiles/Mac/Xcode_v123/EcoLab1.xcodeproj \
  -scheme Dynamic -configuration Release \
  -derivedDataPath BuildFiles/XcodeComponentDynamicRelease \
  ECO_FRAMEWORK=/path/to/Eco.Framework CODE_SIGNING_ALLOWED=NO build

xcodebuild -project AssemblyFiles/Mac/Xcode_v123/EcoLab1UnitTest.xcodeproj \
  -scheme Static -configuration Debug \
  -derivedDataPath BuildFiles/XcodeUnitDebug \
  ECO_FRAMEWORK=/path/to/Eco.Framework CODE_SIGNING_ALLOWED=NO build

xcodebuild -project AssemblyFiles/Mac/Xcode_v123/EcoLab1UnitTest.xcodeproj \
  -scheme Static -configuration Release \
  -derivedDataPath BuildFiles/XcodeUnitRelease \
  ECO_FRAMEWORK=/path/to/Eco.Framework CODE_SIGNING_ALLOWED=NO build
```

Компонент не содержит локального `StdIOShim` и не использует libc. В поставленном на macOS arm64 архиве отсутствует библиотека `Eco.StdIO.C89`, поэтому Eco-клиент печатает итоговую строку через стандартный вывод клиентского процесса. Это ограничение относится только к консольному клиенту; численный компонент проверяется отдельно через `nm -u` и `check_no_libc.sh`.

После Dynamic-сборки выполните:

```sh
DesignFiles/Report/check_dynamic_deps.sh \
  "BuildFiles/Mac/arm64 x86_64/DynamicRelease/4D572607A26A48C1BBC94AB2CB6CFF86.dylib"
```

Для проверки обеих Dynamic CID на macOS используйте `DesignFiles/Report/build_dynamic_runtime_macos.sh`.
Скрипт создаёт обычный артефакт и отдельный QR-артефакт в каталоге `DynamicReleaseQR`.
