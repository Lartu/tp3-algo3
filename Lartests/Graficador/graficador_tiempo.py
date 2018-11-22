import matplotlib.pyplot as plt
import os

# Muestro loguito bonito
print("\033[36;1m\n + GRAFICARTU +\033[0m")

print("\033[33;1m\nSe van a graficar los siguientes archivos del directorio \033[31;1mmediciones\033[33;1m: \033[0m")

if not os.path.exists("graficos"):
    os.mkdir("graficos")

# Cargo el archivo y lo separo
mediciones_pd = []
mediciones_bt = []
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
    if pars[0] == "OP":
        mediciones_pd.append(float(pars[1]))

manitosMagicas = 8
for a in range(0, manitosMagicas):
    porcentajes2 = mediciones_pd
    for i in range(1, len(mediciones_pd)-1):
        mediciones_pd[i] = (porcentajes2[i-1] + porcentajes2[i+1])/2.0 + mediciones_pd[i]
        mediciones_pd[i] = mediciones_pd[i] / 2.0
        
plt.plot(mediciones_pd, color='darkmagenta')
plt.ylabel("Tiempo (ms)")
plt.xlabel("Cantidad de puntos")
nombre_archivo = "graficos/tiempos.png"
plt.savefig(nombre_archivo)
plt.clf()
print("\033[32;1mOk!\033[0m")
print("\033[32;1mImagen guardada en \033[31;1m"+nombre_archivo+"\033[32;1m.\033[0m")
    
# -------- Termino ---------
print("\n\033[32;1mListo!\033[0m\n")
