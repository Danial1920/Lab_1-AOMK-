# Повторная проверка без ripgrep

Пользователь получил `NO LIBC CHECK FAILED: rg unavailable` в iTerm. Причина — обязательная зависимость проверочного скрипта от rg, отсутствующего в пользовательском PATH. Сканирование исходников переведено на стандартную библиотеку уже необходимого Python; проверка символов через nm сохранена.

Весь последовательный прогон выполнялся с PATH=/usr/bin:/bin:/usr/sbin:/sbin; перед началом проверено, что command -v rg завершается неуспешно. ECO_FRAMEWORK=/Users/danial2006/Downloads/Eco.Framework.

Все команды завершились кодом 0:

1. make check — check.log
2. make eco-test-variants — variants.log
3. CONFIGURATION=Release make component-clients — clients.log
4. make validation-build — validation.log

Добавлена отрицательная проверка исходников: неиспользуемый include stdlib.h обнаруживается даже без внешнего символа в nm. Мутации abort(), арифметики и зависимостей Horner по-прежнему обнаруживаются. Производственные C-компоненты в этом исправлении не изменялись.
