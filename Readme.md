Лабораторная работа по операционным системам (2022)

Задача: написать загрузчик и ядро операционной системы. 

Операционная системма должна уметь вычислять значения арифметических операция (консольный режим)

Условия:
- gcc
- fasm
- qemu

==============================================

CalcOS by Ilya Petrov, SPBSTU 2022

email: klekks@yandex.ru

Comands for calcos:
	- info
	- shutdown
	- expretion: 
		"2+2*2", "9/2" and something like this.

emulator command:
	qemu -fda bootsect.bin -fdb kernel.bin

Before started:
	- make kernel
	- make bootsect
