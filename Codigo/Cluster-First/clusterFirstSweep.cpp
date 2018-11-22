#include <climits>
#include <chrono>
#include <random>
#include "../estructuras.h"
#include "../SimulatedAnnealing/simulatedAnnealing.h"
#define PI 3.14159265



vector<pair<int,double>> s_calcularAngulos(S_CVRP &G, int puntoComienzo){
    puntoComienzo = 0;
    //vector de angulos
    vector<pair<int,double>> angulos;
    //←Elijo un punto arbitrario donde empiezo a calcular que no sea el deposito
    if(puntoComienzo == G.getDeposito())
        puntoComienzo++;
    //Calculo el angulo para los nodos que estan despues de deposito---punto
    for(int i = 0; i < G.cantNodos(); i++)
    {
        //dot = x1*x2 + y1*y2; calculo el producto escalar
        auto producto = G.getNodos()[puntoComienzo].coordenadas.x * G.getNodos()[i].coordenadas.x
                        - G.getNodos()[puntoComienzo].coordenadas.y  * G.getNodos()[i].coordenadas.y;
        //det = x1*y2 - y1*x2; calculo la determinante
        auto determinante = G.getNodos()[puntoComienzo].coordenadas.x * G.getNodos()[i].coordenadas.y
                            + G.getNodos()[puntoComienzo].coordenadas.y  * G.getNodos()[i].coordenadas.x;
        //calculo el angulo
        auto angulo = atan2(determinante, producto) * 180 / PI;
        //agrego el angulo a la solucion
        angulos.push_back(make_pair(i,angulo));
    }
    //Represento mis angulos del 0 a 360
    for(auto &angulo : angulos)
    {
        if(angulo.second < 0)
        {
            angulo.second = 360 + angulo.second;
        }
    }
    //Ordeno de menor a mayor
    std::sort(angulos.begin(),angulos.end(),[]( const pair<int,double> a, const pair<int,double> b) {
                        return (a.second < b.second);});
    return angulos;
}

vector<vector<S_Nodo>> s_clusterizarNodos(S_CVRP &G, vector<pair<int,double>>& angulos){
    auto count = 0;
    auto i = 0;
    //Vector de clusters
    vector<vector<S_Nodo>> res;
    while(i < G.cantNodos())
    {
        //Vector de un cluster en particular
        count = 0;
        vector<S_Nodo> cluster;
        while(count < G.getCapacidad() && i < G.cantNodos())
        {
            //Chequeo que no me excedí de la capacidad máxima
            if(count + G.getNodoById(angulos[i].first).demanda > G.getCapacidad())
            {
                count = G.getCapacidad();
            }
            else
            {
                //Sumo la demanda y agrego el nodo a mi cluster
                count += G.getNodoById(angulos[i].first).demanda;
                cluster.push_back(G.getNodoById(angulos[i].first));
                ++i;
            }
        }
        //Agrego el cluster a mi vector de clusters
        res.push_back(cluster);
    }
    return res;
}


bool s_estaEnSolucion(vector<S_Nodo> nodos, S_Nodo nodo)
{
    for(auto &n : nodos)
    {
        if(n == nodo) return true;
    } return false;
}


S_Nodo s_nodoMasCercano(S_CVRP &C, S_Nodo &nodo, vector<S_Nodo> &nodos, vector<S_Nodo> &solucion){
    double distancia = numeric_limits<double>::max();
    S_Nodo res = nodo;
    for(auto &n : nodos){
        if(C.distanciaEntre(nodo.id,n.id) < distancia && !s_estaEnSolucion(solucion,n))
        {
            distancia = C.distanciaEntre(nodo.id,n.id);
            res = n;
        }
    }
    return res;
}

// TSP usando nearest neighbor
vector<S_Nodo> s_tsp(S_CVRP &C, vector<S_Nodo> &nodos)
{
    vector<S_Nodo> solucion;
    //Busco el deposito
    S_Nodo deposito = C.getNodoById(C.getDeposito());
    //Agrego el deposito a la solucion
    solucion.push_back(deposito);
    //encuentro el nodo mas cercano al deposito
    S_Nodo nodoAgregar = s_nodoMasCercano(C,deposito,nodos,solucion);
    //Agrego el nodo mas cercano al deposito

    solucion.push_back(nodoAgregar);
    for(auto &nodo : nodos)
    {
        nodoAgregar = s_nodoMasCercano(C,nodoAgregar,nodos,solucion);
        if(!(nodoAgregar == s_nodoMasCercano(C,nodoAgregar,nodos,solucion)))
            solucion.push_back(nodoAgregar);
    }
    //Chequeo por si el cluster es de un nodo
    if(!s_estaEnSolucion(solucion,nodoAgregar))
        solucion.push_back(nodoAgregar);
    solucion.push_back(deposito);

    return solucion;
}

p_solucion s_resolverCVRP(S_CVRP &C, int puntoInicial)
{
    vector<vector<S_Nodo> > caminos;
    double costoTotal = 0.0;
    //Calculo los angulos para hacer sweep
    vector<pair<int,double> > angulos =  s_calcularAngulos(C,puntoInicial);
    //Clusterizo en base a los angulos calculados
    vector<vector<S_Nodo> > clusters = s_clusterizarNodos(C, angulos);
    //Hago tsp para cada cluster y me guardo el recorrido
    for(auto &cluster : clusters)
    {
        caminos.push_back(s_tsp(C,cluster));
    }
    //Imprimo la cantidad de camiones que se usaron
//    cout << clusters.size() << endl;

    //Por cada camion imprimo el recorrido y acumulo el costo
    for(auto &camino : caminos)
    {
        for(int i = 0; i < camino.size() - 1; ++i)
        {
//            cout << camino[i].id << " ";
        }
//        cout << camino[0].id << endl;

    }
    for(int i = 0; i < caminos.size(); i++)
    {
        for(int j = 1; j < caminos[i].size(); j++){
            costoTotal += C.distanciaEntre(caminos[i][j].id, caminos[i][j-1].id);
            //cout << "distancia Entre " << rutas[i][j] << " y " << rutas[i][j-1] << " = " << G.distanciaEntre(rutas[i][j], rutas[i][j-1]) << endl;
        }
    }
    //Imprimo el costo total
//    cout << costoTotal << endl;
    
    //Convierto los clusters de nodos en ids
    //creo un vector de la carga de cada camion para armar mi solucion
    vector<vector<int>> todasIds;
    vector<int> cargas;
    for(auto &camino : caminos)
    {
        vector<int> ids;
        auto count = 0;
        for(auto &elem : camino)
        {
            //agrego los ids de los nodos visitados en un camino
            ids.push_back(elem.id);
            count += elem.demanda;
        }
        todasIds.push_back(ids);
        cargas.push_back(count);
    }


    //Armo una instancia de p_solucion
    p_solucion resultado(todasIds, cargas, costoTotal);
    return resultado;
}


int main(int argc, char** argv)
{
    if(argc != 3){
        cout << "Param 1: set" << endl << "Param 2: 0 <= puntoInicial < setSize" << endl;
        return 1;
    }
    // - cargo archivo -
    string fileName(argv[1]);

    // - parseo con TSPLib -
    TspData archivo = cargarTSP(fileName);

    // - creo el grafo -
    S_CVRP grafo = S_CVRP(archivo);


    // - Obtengo una solución heurística por sweep
    p_solucion s_solSweep = s_resolverCVRP(grafo,*argv[2]);


    // - Imprimo las rutas de la solución -
    s_solSweep.imprimirRutas();


    return 0;
}

