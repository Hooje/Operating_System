import threading 
import hashlib
import multiprocessing
import bs4
import lxml	
import requests
import asyncio
import itertools
import time

choose_task = input()
choose_way = input().split()
num_thread = 1
num_task = int(input())
#print(num_task)
#print(choose_way[0])
if choose_way[0] == '1'  or choose_way[0] == '2':
	#print('here')
	num_thread = int(choose_way[1])

tasks = []

def question_1(thread_th):
	#print('question1')
	inpt = []

	for i in range(num_task):
		if i%num_thread == thread_th :

			inpt.append(tasks[i])
	#print('here1')
	#print(inpt)
	for s in inpt:
		#print(s)
		S = s.encode()
		for C in map(bytes, itertools.product(range(0x21, 0x7E), repeat=5)):
			if hashlib.sha256(C+S).hexdigest()[0:5] == '00000':
				print(((C+S).decode()))
				break


def question_2(thread_th):
	print('question2')
	inpt = []
	for i in range(num_task):
		if i%num_thread == thread_th :
			inpt.append(tasks[i])
	for s in inpt:
		print(s)
		response = requests.get(s)
		soup = bs4.BeautifulSoup(response.text, "lxml")
		print(soup.title.text)
question = [question_1, question_2]
for i in range(num_task):
	#print(i)
	x = input()
	tasks.append(x)
	#print(x)

start = time.time()

if choose_way[0] == '1' :
	#print(1)
	threads = []
	for i in range(num_thread):
  		threads.append(threading.Thread(target = question[int(choose_task)-1], args = (i,)))
  		threads[i].start()


if choose_way[0] == '2' :
	#print(2)
	processes = [] 
	for i in range(num_thread):
  		processes.append(multiprocessing.Process(target = question[int(choose_task)-1], args = (i,)))
  		processes[i].start()
if choose_way[0] == '3' :
	#print(3)
	loop = asyncio.get_event_loop()
	async def func(): 
		#print('here')
		#loop = loop.get_running_loop()
		coro=[loop.run_in_executor(None, question[int(choose_task)-1], 0)]
		await asyncio.gather(*coro)
		
		#return  
	loop.run_until_complete(func())

end = time.time()
print(end-start)