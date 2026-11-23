"""Generate the Lab3 report from the four client logs and mutation audit."""
from io import BytesIO
from pathlib import Path
import math
import re

from docx import Document
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.shared import Cm, Pt, RGBColor
from PIL import Image, ImageDraw, ImageFont

root = Path(__file__).resolve().parents[2]
build = root/'BuildFiles/Lab3'
logs = {(c,m):(build/c/(m+'.log')).read_text().splitlines() for c in ('Debug','Release') for m in ('Static','Dynamic')}
audit = (build/'audit.log').read_text()
if '4/4 server defects detected' not in audit:
    raise SystemExit('Run make lab3-audit first')

def diagram():
    img = Image.new('RGB',(1300,315),'white')
    draw = ImageDraw.Draw(img)
    font = ImageFont.truetype('/System/Library/Fonts/Supplemental/Arial.ttf',32)
    left, right = 380, 920
    for i, color in enumerate(('#20516A','#A74E28')):
        a = (left, 80+150*i)
        for j in range(2):
            b = (right-8, 65+150*j+30*i)
            draw.line([a,b],fill=color,width=5)
            angle = math.atan2(b[1]-a[1], b[0]-a[0])
            draw.polygon([b]+[(b[0]-21*math.cos(angle+t),b[1]-21*math.sin(angle+t)) for t in (-.46,.46)],fill=color)
    for xpos, texts, colors in [(40,['Eco.Lab1 · Гаусс','Eco.Lab1 · QR'],['#20516A','#A74E28']),
                                (right,['Консоль','Журнал'],['#384B58']*2)]:
        for i,(label,color) in enumerate(zip(texts,colors)):
            y = 80+150*i
            draw.rounded_rectangle((xpos,y-47,xpos+340,y+47),radius=15,fill='#F2F5F6',outline=color,width=3)
            draw.text((xpos+170,y),label,anchor='mm',fill=color,font=font)
    data = BytesIO()
    img.save(data,format='PNG'); data.seek(0)
    return data

d=Document()
d.core_properties.title='Лабораторная работа № 3. События в polyfit'
d.core_properties.author='Анвари Д.А., 24ТИДИ'
sec=d.sections[0]; sec.page_width=Cm(21); sec.page_height=Cm(29.7)
sec.left_margin=Cm(2.3); sec.right_margin=Cm(2.2); sec.top_margin=Cm(2); sec.bottom_margin=Cm(1.9)
sec.footer_distance=Cm(.8)
styles=d.styles
for name, size, color in [('Normal',11,'202B32'),('Heading 1',22,'193E50'),('Heading 2',14,'20516A')]:
    style=styles[name]; style.font.name='Arial'; style.font.size=Pt(size); style.font.color.rgb=RGBColor.from_string(color)
    style.paragraph_format.space_after=Pt(6); style.paragraph_format.line_spacing=1.13
for name in ('Heading 1','Heading 2'):
    styles[name].paragraph_format.space_before=Pt(9)
footer=sec.footer.paragraphs[0]; footer.alignment=WD_ALIGN_PARAGRAPH.CENTER
field=OxmlElement('w:fldSimple'); field.set(qn('w:instr'),'PAGE'); footer._p.append(field)

def body(t): return d.add_paragraph(t)
def caption(t):
    p=body(t); p.runs[0].italic=True; p.runs[0].font.size=Pt(9)
def table(headers, rows, widths):
    tab=d.add_table(rows=0,cols=len(headers)); tab.autofit=False
    for col,w in zip(tab.columns,widths): col.width=Cm(w)
    for idx,row in enumerate([headers]+rows):
        cells=tab.add_row().cells
        for cell,w,t in zip(cells,widths,row):
            cell.width=Cm(w); p=cell.paragraphs[0]
            p.paragraph_format.space_before=Pt(3); p.paragraph_format.space_after=Pt(3)
            r=p.add_run(t); r.bold=idx==0; r.font.size=Pt(10)
            shade=OxmlElement('w:shd'); shade.set(qn('w:fill'),'DEE9ED' if idx==0 else ('F2F5F6' if idx%2 else 'FFFFFF'))
            cell._tc.get_or_add_tcPr().append(shade)
    return tab
def code(lines):
    p=d.add_paragraph()
    p.paragraph_format.left_indent=Cm(.35); p.paragraph_format.line_spacing=1.06
    r=p.add_run('\n'.join(lines)); r.font.name='Courier New'; r.font.size=Pt(9)
def section(t): d.add_heading(t,2)
def title(t): d.add_heading(t,1)
def page(): d.add_page_break()

title('Лабораторная работа № 3')
p=body('События в компоненте полиномиальной аппроксимации'); p.runs[0].bold=True; p.runs[0].font.size=Pt(15)
body('Выполнил Анвари Д.А., группа 24ТИДИ\nНижний Новгород, 2026')
section('Задача и схема подключения')
body('В задании № 44 компонент Eco.Lab1 подбирает коэффициенты полинома методом наименьших квадратов. В ЛР3 клиент наблюдает за ходом расчёта. Для схемы «многие ко многим» он получает через IEcoInterfaceBus1 два объекта по CID из ЛР1. В клиенте созданы консольный приёмник и журнал. Каждый подписан на оба объекта.')
d.add_picture(diagram(),width=Cm(16.2))
caption('Рисунок 1. Направление стрелок совпадает с направлением доставки событий. Каждой стрелке соответствует отдельная пара «точка, cookie».')
section('Контракт событий')
table(['Метод IEcoLab1Events','Данные обратного вызова'],[
    ['OnFitStarted','Источник, алгоритм, число точек и степень.'],
    ['OnPivotSelected','Столбец, выбранная строка и значение ведущего элемента перед перестановкой.'],
    ['OnGivensRotation','Номер входной точки, столбец, косинус и синус выполненного вращения.'],
    ['OnFitFinished','Статус и коэффициенты в порядке убывания степени. При ошибке длина равна нулю.']], [5.4,11.1])
section('Время жизни подписки')
body('Клиент запрашивает IEcoConnectionPointContainer, находит точку по IID события и вызывает Advise. Точка удерживает ссылку на приёмник. Unadvise отключает ровно одну связь. Вызовы синхронные в одном потоке. Журнал копирует значения до возврата из обработчика.')

page();title('Наблюдение за вычислением')
body('Входные точки имеют абсциссы −2, −1, 0, 1, 2 и ординаты 11, 5, 3, 5, 11. Все пять точек лежат на параболе y = 2x² + 3. Поэтому известный ответ для степени 2 — [2, 0, 3].')
section('Что передают алгоритмы')
ref=logs['Release','Dynamic']
code([line.strip() for line in ref if 'pivot col=' in line][:3]+
     [line.strip() for line in ref if 'rotate sample=' in line][:3])
caption('Фрагмент Release/Dynamic. Индексы строк и столбцов начинаются с нуля.')
body('Ядро масштабирует абсциссы. Здесь z = x/2, поэтому строки матрицы имеют вид [z², z, 1]. В нормальных уравнениях метод Гаусса выбирает три ведущих элемента. В первом столбце максимальный модуль равен 2,5 и находится в строке 2. Поле row показывает выбор до обмена строк. Клиент сверяет каждый выбор с эталоном.')
body('QR добавляет строки матрицы по мере чтения точек. Каждое вращение обновляет треугольную матрицу и правую часть. При двух нулевых элементах алгоритм пропускает шаг. На этих данных выполнено 12 вращений. UnitTest сверяет номера точек, столбцов и оба параметра каждого вращения.')
section('Отключение одной из четырёх связей')
code([s for s in ref if s.startswith('round=')])
body('Каждому приёмнику сначала пришло 5 событий от Гаусса и 14 от QR. В оба числа входят начало и конец. Перед вторым расчётом клиент вызвал Unadvise для связи QR с консолью. Все 14 событий QR снова дошли до журнала, а доставка событий Гаусса сохранилась у обоих приёмников.')
section('Сравнение результатов')
code([line.strip() for line in ref if 'finish status=' in line][:2])
body('Вторая строка относится к QR. Коэффициент при x отличается от нуля примерно на 2,1·10⁻¹⁶ из-за округления. Оба результата укладываются в абсолютный допуск 10⁻¹⁰. Разное число уведомлений отражает разные вычислительные шаги.')

page(); title('Проверка и выводы')
rows=[]
for c in ('Debug','Release'):
    rows.append([c]+[re.fullmatch(r'LAB3 PASS: (\d+) checks',logs[c,m][-1]).group(1) for m in ('Static','Dynamic')])
table(['Сборка macOS arm64','Static','Dynamic'],rows,[7.5,4.5,4.5])
caption('Число выполненных проверок. В Static дополнительно проверяется регистрация семи фабрик. В Dynamic их загружает Eco Framework по CID.')
section('Ошибочные случаи')
body('В опыте с пятью одинаковыми абсциссами обе реализации вернули ошибку вырожденной системы. Приёмник получил OnFitFinished с тем же статусом и пустым результатом. Проверены неверный IID, приёмник без интерфейса событий, повторный Unadvise и отключение из обработчика.')
body('Проверка перечислителей охватывает снимок подписок, независимое положение клона и конец последовательности. В агрегате Lab2 контейнер возвращает тот же IEcoUnknown, что и внешний объект. При уничтожении источника освобождается последняя подписка.')
code([s for s in ref if s.startswith('Allocation audit:')])
body('При семи точечных отказах allocator проверены создание подписки, снимка и клона. После отказов объект снова выполнил polyfit. ASan/UBSan завершили прогон без диагностик.')
section('Проверка самих тестов')
body('В четырёх отдельных копиях проекта поочерёдно убрали событие Гаусса, изменили знак синуса, отключили доставку второму подписчику и оставили подписку активной после успешного Unadvise. На каждом дефекте UnitTest остановился с ненулевым кодом.')
section('Повторение из корня проекта')
code(['export ECO_FRAMEWORK="$HOME/Downloads/Eco.Framework"','make lab3-verify','make check eco-test-variants validation-build'])
section('Вывод')
body('Опыт подтверждает схему «два источника — два приёмника» и независимость четырёх подписок. Проверка коэффициентов показывает точность результата, а проверка промежуточных событий различает работу Гаусса и QR даже при совпадающем ответе.')
d.save(root/'EcoLab3_Report.docx')
