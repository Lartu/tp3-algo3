import matplotlib.pyplot as plt
import numpy as np

def sumar(fileName):
    x = []
    y = []
    file = open(fileName,"r")
    for line in file:
        firstPart = True
        first =""
        second =""
        for char in line:
            if char == ',':
                firstPart = False
                continue
            if char == ':':
                break
            if firstPart:
                first = first + char
            else: 
                second = second + char
        x1 = int(first)
        x.append(x1)
        y1 = float(second)
        y.append(y1)
    file.close()
    print(sum(y))
    return sum(y)

sumar("XconParamRandom NIt = 15000")
#graficar("ahorros con optimo random","Porcentajes de ahorro para cada instancia conocida","n","Porcentaje ahorrado",2,100,'ro')
#graficar("ResCs2 porcentaje ahorro","Porcentaje de ahorro para cada CS con parámetros óptimos","n","Porcentaje ahorrado",100,100,'-r')
