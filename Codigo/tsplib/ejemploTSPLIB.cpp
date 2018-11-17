#include "tsplib/tsplib.h" 

int main(){
    TspData archivo = cargarTSP("A-n32-k5.vrp.txt");
    cout << "Cantidad de puntos: "
         << archivo.dimension() << endl;
    cout << "¿Es depósito el nodo 1?: "
         << (archivo.esDeposito(1) ? "Sí" : "No") << endl;
    cout << "X del nodo 32: "
         << archivo.coords(32).first << endl;
    cout << "Y del nodo 32: " 
         << archivo.coords(32).second << endl;
    cout << "Demanda del nodo 16: "
         << archivo.demanda(16) << endl;
    cout << "Capacidad de los camiones: "
         << archivo.capacidad() << endl;
    return 0;
}


