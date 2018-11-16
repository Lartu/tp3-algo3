import matplotlib.pyplot as plt
import numpy as np

def graficar(fileName,title,xlabel,ylabel,intervaloX,maximoY,color):
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
    plt.plot(x,y,color)
    plt.xticks(np.arange(min(x), max(x)+1, intervaloX))
    plt.axis([0,max(x),0,maximoY])
    plt.ylabel(ylabel)
    plt.xlabel(xlabel)
    plt.title(title);
    plt.legend(("Nit = 100", "Nit = 1000", "Nit = 10000"), loc='upper right')
    imageName = fileName + ".png"
    plt.savefig(imageName)
    #plt.clf()
    file.close()
    return;

graficar("MedT-100it","titulo","n","milisegundos",100,1000,'b-')
graficar("MedT-1000it","titulo","n","milisegundos",100,1000,'g-')
graficar("MedT-10000it","Comparativa temporal variando el número máximo de iteraciones","n","milisegundos",100,1000,'r-')
