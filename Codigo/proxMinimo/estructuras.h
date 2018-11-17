#ifndef TP3_ALGO3_ESTRUCTURAS_H
#define TP3_ALGO3_ESTRUCTURAS_H
#include "../tsplib/tsplib.h"
#include <limits>
#include <cmath>
#include <algorithm>
#include <vector>

using distancia = double;
using Matriz = vector<vector<double>>;
using namespace std;

struct S_Coordenada{
    double x;
    double y;
}__attribute__((packed));

struct S_Nodo{
    int id;
    int demanda;
    S_Coordenada coordenadas;
    bool operator==(const S_Nodo& n) const
    {
        return (coordenadas.x == n.coordenadas.x && coordenadas.y == n.coordenadas.y);
    }
}__attribute__((packed));

class S_CVRP{

public:

    S_CVRP(TspData tsp){

        //Encuentro mi deposito
        for(int i = 1; i < tsp.dimension(); i++){
            if(tsp.esDeposito(i))
                deposito = i - 1;
        }

        //Asigno la capacidad de los camiones
        capacidad = tsp.capacidad();


        //Guardo las coordenadas centrando en deposito

        for(int i = 1; i <= tsp.dimension(); i++){

            S_Coordenada c;
            c.x = tsp.coords(i).first;
            c.y = tsp.coords(i).second;
            c.x = c.x - tsp.coords(deposito + 1).first;
            c.y = c.y - tsp.coords(deposito + 1).second;

            S_Nodo n;
            n.id = i - 1;
            n.coordenadas.x = c.x;
            n.coordenadas.y = c.y;
            n.demanda = tsp.demanda(i);

            nodos.push_back(n);
        }

        //Genero la matriz de adyacencia
        for(int i = 0; i < tsp.dimension(); ++i){
            vector<double> fila;
            for(int j = 0; j < tsp.dimension(); ++j){
                fila.push_back(distancia(nodos[i].coordenadas, nodos[j].coordenadas));
            }
            matriz.push_back(fila);
        }

    };



    const Matriz& getMatriz() const{
        return matriz;
    }

     const size_t cantNodos() const{
        return nodos.size();
    }

    int getDeposito() const{
        return deposito;
    }


     const vector<S_Nodo>& getNodos() const{
         return nodos;
     }

     int getCapacidad() const{
         return capacidad;
     }

     const S_Nodo& getNodoById(int id){
         return getNodos()[id];
     }

     const int getDemanda(int id) const{
        return getNodos()[id].demanda;
     }

     const double distanciaEntre(int id1, int id2) const{
        return getMatriz()[id1][id2];
     }

     vector<int> getNodosIds() const{
        vector<int> ids = vector<int>();
        for(int i = 0; i < nodos.size(); i++){
            ids.push_back(nodos[i].id);
        }
        return ids;
     }

private:

    double distancia(S_Coordenada desde, S_Coordenada hasta){
        if(desde.x == hasta.x && desde.y == hasta.y) return numeric_limits<double>::max();
        return sqrt(pow((double)(desde.x - hasta.x), 2.0) + pow((double)(desde.y - hasta.y), 2.0));
    }

    int deposito;
    vector<S_Nodo> nodos;
    int capacidad;
    Matriz matriz;


};


typedef vector<int> ruta;

class p_solucion{
public:
    p_solucion(vector<ruta> _rutas,vector<int> _cargas,double _costoTotal) : rutas(_rutas),cargas(_cargas),costoTotal(_costoTotal) {};
    double costoTotal;
    vector<ruta> rutas;
    vector<int> cargas;
    
    // - Imprimo las rutas para poder verlas -
    void imprimirRutas()
    {
        cout << "\033[1;33mRutas de esta solución\033[m: " << endl;
        for(ruta rutaActual : rutas)
        {
            cout << "\033[1;35m *\033[m ";
            for(int i = 0; i < rutaActual.size(); ++i){
                if(i != 0) cout << " \033[1;34m->\033[m ";
                cout << rutaActual[i];
            }
            cout << endl;
        }
        
        cout << "\033[1;32mCosto total\033[m: " << costoTotal << endl;
    }

    bool esSolucion(const S_CVRP &G) const{
        //Checkeo que todos los clientes sean visitados una vez.
        vector<int> clientes = vector<int>();
        for(auto &ruta : rutas){
            for(auto &elem : ruta){
                if(elem != 0){
                    clientes.push_back(elem);
                }
            }
        }
        clientes.push_back(0);
        auto ids = G.getNodosIds();
        sort(ids.begin(),ids.end());
        sort(clientes.begin(),clientes.end());
        if(ids!=clientes){
            cout << "No es solucion porque ids != clientes" << endl;
            return false;
        }
        //Checkeo que ningun camion se pase de capacidad
        auto capacidad = G.getCapacidad();
        for(int i = 0; i < cargas.size(); i++){
            if(cargas[i] > capacidad){
                cout << "No es solucion porque un camión se pasa de capacidad (camino " << i << ")" << endl;
                return false;
            }
        }
        //checkeo que el vector de cargas sea consistente con las demandas de las rutas
        for(int i = 0; i < rutas.size(); i++){
            int demandaTotal = 0;
            for(auto &elem : rutas[i]){
                if(elem != 0) demandaTotal += G.getDemanda(elem);
            }
            if(demandaTotal != cargas[i]){
                cout << "No es solucion porque el vector de cargas no es consistente con las demandas de las rutas" << endl;
                return false;
            }
        } 

        //Checkeo que el costo total no sea infinito ni negativo
        if(costoTotal == numeric_limits<double>::max() || costoTotal < 0) return false;
        return true;
    }
};

//Función que calcula lo que costaría el camino sin optimizar nada
double l_costeSinOptimizar(const S_CVRP & grafo)
{
    // - Obtengo las distancias -
    Matriz distancias = grafo.getMatriz();
    
    // - Inicializo el coste total -
    double costeTotal = 0;
    
    // - Obtengo el deposito -
    int depot = grafo.getDeposito();
    
    // - Calculo todas las triplas Dep -> nodo -> Dep -
    for(int i = 0; i < distancias.size(); ++i){
        if(depot == i) continue;
        costeTotal += distancias[depot][i] * 2;
    }
    
    // - Devuelvo el coste sin optimizar -
    return costeTotal;
}

#endif
