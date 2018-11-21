import matplotlib.pyplot as plt
import os

# Muestro loguito bonito
print("\033[36;1m\n + GRAFICARTU +\033[0m")

print("\033[33;1m\nSe van a graficar los siguientes archivos del directorio \033[31;1mmediciones\033[33;1m: \033[0m")

if not os.path.exists("graficos"):
    os.mkdir("graficos")

# Cargo el archivo y lo separo
porcentajes = []
porcentaje2 = []
archivo = "medias.txt"
print("\033[33;1m\nGraficando "+ archivo +"\033[0m")
print("[\033[35;1m*\033[0m] Cargando archivo...", end=" ")
with open(archivo, "r") as f:
    archivo = f.read()
print("\033[32;1mOk!\033[0m")
lineas = archivo.split("\n")
for l in lineas:
    if l == "":
        continue
    pars = l.split(" ")
    if pars[0] == "PO":
        porcentajes.append(100 - float(pars[1]))

manitosMagicas = 4
for a in range(0, manitosMagicas):
    porcentajes2 = porcentajes
    for i in range(1, len(porcentajes)-1):
        porcentajes[i] = (porcentajes2[i-1] + porcentajes2[i+1])/2.0 + porcentajes[i]
        porcentajes[i] = porcentajes[i] / 2.0
cuenta = 0

for a in porcentajes:
    cuenta = cuenta + a
cuenta = cuenta / len(porcentajes)
plt.axhline(y=cuenta, color='r', linestyle='-')
plt.plot(porcentajes, color='green')
plt.ylabel("Ahorro con respecto al caso canónico (%)")
plt.xlabel("Cantidad de puntos")
plt.ylim((0, 100))
nombre_archivo = "graficos/porcentajes.png"
plt.savefig(nombre_archivo)
plt.clf()
print("\033[32;1mOk!\033[0m")
print("\033[32;1mImagen guardada en \033[31;1m"+nombre_archivo+"\033[32;1m.\033[0m")
    
# -------- Termino ---------
print("\n\033[32;1mListo!\033[0m\n")
