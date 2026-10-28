"""Render the actual Lab2 ownership graph for the report using Pillow."""
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
from math import atan2, cos, sin
root=Path(__file__).resolve().parent
im=Image.new('RGB',(1800,900),'white'); d=ImageDraw.Draw(im)
font=ImageFont.truetype('/System/Library/Fonts/Supplemental/Arial.ttf',31)
small=ImageFont.truetype('/System/Library/Fonts/Supplemental/Arial.ttf',27)
nodes={'L2':(900,90,'Eco.Lab2'), 'L1':(400,280,'Eco.Lab1'), 'A':(900,280,'A / C\nAddition'), 'D':(1430,280,'D\nMultiplication'), 'B1':(180,510,'B1\nпубличный X'), 'E':(610,510,'E\nY через фасад Lab1'), 'B3':(1430,510,'B3\nX для D'), 'B2':(610,730,'B2\nX для E')}
def arrow(a,b,dashed=False):
 x,y,_=nodes[a];xx,yy,_=nodes[b];y+=60;yy-=60
 if dashed:
  for k in range(0,100,10):
   d.line((x+(xx-x)*k/100,y+(yy-y)*k/100,x+(xx-x)*(k+6)/100,y+(yy-y)*(k+6)/100),fill='#24577A',width=5)
 else:d.line((x,y,xx,yy),fill='#445566',width=4)
 t=atan2(yy-y,xx-x);sz=17
 d.polygon([(xx,yy),(xx-sz*cos(t-.5),yy-sz*sin(t-.5)),(xx-sz*cos(t+.5),yy-sz*sin(t+.5))],fill='#24577A')
for a,b,agg in [('L2','L1',True),('L2','A',False),('L2','D',False),('L1','B1',True),('L1','E',False),('E','B2',True),('D','B3',False)]:arrow(a,b,agg)
for x,y,t in nodes.values():
 d.rounded_rectangle((x-170,y-58,x+170,y+58),radius=12,fill='#F3F6F8',outline='#24577A',width=3)
 box=d.multiline_textbbox((0,0),t,font=font,spacing=5,align='center');w=box[2]-box[0];h=box[3]-box[1]
 d.multiline_text((x-w/2,y-h/2-box[1]),t,font=font,fill='#152936',spacing=5,align='center')
d.text((80,825),'Сплошная стрелка — включение. Пунктир — агрегирование.',font=small,fill='#222222')
(root/'figures').mkdir(exist_ok=True)
im.save(root/'figures/lab2_architecture.png')
