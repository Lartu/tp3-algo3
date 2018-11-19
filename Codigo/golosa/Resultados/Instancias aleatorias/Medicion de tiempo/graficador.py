import matplotlib.pyplot as plt
import numpy as np

def graficar(fileName,title,xlabel,ylabel,intervaloX,maximoY,color):
    x = []
    y = []
    file = open(fileName,"r")
    for line in file:
        if line[0] == '/':
            break
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
        y1 = float(second) /float(x1*x1*x1)
        y.append(y1)
    plt.plot(x,y,color)
    plt.xticks(np.arange(min(x)-2, max(x)+4, intervaloX))
    plt.axis([min(x)-2,max(x)+4,min(y),maximoY])
    plt.ylabel(ylabel)
    plt.xlabel(xlabel)
    plt.title(title);
    #plt.legend(("óptimo instancias", "óptimo aleatorio"), loc='upper right')
    imageName = fileName + ".png"
    plt.savefig(imageName)
    #plt.clf()
    file.close()
    return;

#graficar("ResGolosa","Porcentaje de ahorro promedio para instancias aleatorias","n","Porcentaje de ahorro",50,60,'ro')
graficar("Tgolosa","Tiempo de ejecución del algoritmo","n","milisegundos",100,0.0003,'-b')
#graficar("ResGolosaSerieX","Distancia porcentual al óptimo Set X","n","Distancia al óptimo",50,100,'bo')
