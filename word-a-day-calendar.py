#  -*- coding: UTF-8 -*-
import requests
import re
from unihiker import GUI
import time
from datetime import datetime
from bs4 import BeautifulSoup

def split_for_led_screen(text, max_chars=30):
  words = text.split()  
  sentence_list = []
  current_line = ""

  for word in words:
    if len(current_line) + len(word) + 1 <= max_chars:
      current_line += " " + word 
    else:
      sentence_list.append(current_line.strip())
      current_line = word
  if current_line:
    sentence_list.append(current_line.strip())
  return sentence_list

today = datetime.now()
gui = GUI()

gui.fill_round_rect(x=0, y=0, w=240, h=320, r=3, color="#f7f483",)

# Print Word of the Day
word_pattern = r"<h2[^>]*>(.*?)</h2>"
word_response = requests.get(f"https://api.thingspeak.com/apps/thinghttp/send_request?api_key=Q1EN7TP027PXQ0AY")
# print(word_response.text)
new_word = re.findall(word_pattern, word_response.text)
print(new_word[0])
gui.draw_text(text=new_word[0],origin="center",x=22,y=160,color="#000000",font_size=26,angle=90)

gui.draw_text(text="||",origin="center",x=60,y=205,color="#e60b0b",font_size=18,angle=90)

# Print Word of the Day Type
word_type_pattern = r'<span class="main-attr">(.*?)</span>'
word_type_response = requests.get(f"https://api.thingspeak.com/apps/thinghttp/send_request?api_key=GT11LO2O1IPLQY49")
# print(word_type_response.text)
new_word_type = re.findall(word_type_pattern, word_type_response.text)
print(new_word_type[0])
gui.draw_text(text=new_word_type[0],origin="center",x=60,y=265,color="#0045b5",font_size=15,angle=90)

# Print Word of the Day Pronunciation
word_pronunciation_pattern = r'<span class="word-syllables">(.*?)</span>'
word_pronunciation_response = requests.get(f"https://api.thingspeak.com/apps/thinghttp/send_request?api_key=2QMLYJM8Z4WTCQ0W")
print(word_pronunciation_response.text)
new_word_pronunciation = re.findall(word_pronunciation_pattern, word_pronunciation_response.text)
print(new_word_pronunciation[0])
gui.draw_text(text=new_word_pronunciation[0],origin="center",x=60,y=105,color="#73a4eb",font_size=15,angle=90)

# Print Word of the Day Definition
word_def_response = requests.get(f"https://api.thingspeak.com/apps/thinghttp/send_request?api_key=KTJTIEGX8WMVLVMJ")
# print(word_def_response.text)
word_def_parsed = BeautifulSoup(word_def_response.text, 'html.parser')
text_elements = word_def_parsed.find_all(string=True)
text = ' '.join(element.strip() for element in text_elements if element.strip())
sentence_list = []
current_sentence = ""
for char in text:
  if char in ';.':
    if current_sentence:
      sentence_list.append(current_sentence)
    current_sentence = ""
  else:
    current_sentence += char
if current_sentence:
  sentence_list.append(current_sentence)
print(sentence_list[0])

adjusted_list = split_for_led_screen(sentence_list[0])   # Function call for spliting definition
print(adjusted_list)
def_x = 100

for i in range(len(adjusted_list)):
    gui.draw_text(text=adjusted_list[i],origin="center",x=(def_x + i * 25),y=160,color="#40444a",font_size=15,angle=90)
    print(adjusted_list[i], " : ", def_x + i * 50)


# Print date at bottom
formatted_date = today.strftime('%B %d %Y')
gui.fill_round_rect(x=200, y=0, w=40, h=320, r=3, color="#4d4646",)
gui.draw_text(text=formatted_date,origin="center",x=220,y=160,color="#ffffff",font_size=24,angle=90)

while True:
    #Keep program running
    time.sleep(1)