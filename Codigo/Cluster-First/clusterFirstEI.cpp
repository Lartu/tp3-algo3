#include <climits>
#include <chrono>
#include <random>
#include "../estructuras.h"
#include "../SimulatedAnnealing/simulatedAnnealing.h"

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

int minDistancia(vector<double> &d, vector<bool> &v)
{
    float min = numeric_limits<double>::max();
    int res = -1;
    for(int i = 0; i < d.size(); i++)
    {
        if(d[i] < min && !v[i])
        {
            min = d[i];
            res = i;
        }
    }
    return res;
}

bool estaTodoEnTrue(vector<bool> &v){
    for(int i = 0; i < v.size(); i++)
    {
        if(!v[i]) return false;
    }
    return true;
}

void prim(Matriz &m, const vector<S_Nodo> &nodos)
{
    vector<double> distancias(m.size(), numeric_limits<double>::max());
    vector<bool> visitados(m.size(), false);
    vector<int> padre(m.size(), -1);


    for (int i = 0; i < m.size(); i++)
    {
        if (m[0][i] != numeric_limits<double>::max())
        {
            distancias[i] = m[0][i];
            padre[i] = nodos[0].id;
        }
    }

    distancias[0] = 0;
    visitados[0] = true;
    padre[0] = nodos[0].id;

    while (!estaTodoEnTrue(visitados))
    {
        int indiceDeNodo = minDistancia(distancias, visitados);
        S_Nodo v = nodos[indiceDeNodo];
        visitados[indiceDeNodo] = true;

        for (int i = 0; i < m.size(); i++)
        {
            if (m[indiceDeNodo][i] != numeric_limits<double>::max())
            {
                if (distancias[i] > m[indiceDeNodo][i] && !visitados[i])
                {
                    distancias[i] = m[indiceDeNodo][i];
                    padre[i] = v.id;
                }
            }
        }
    }

    //Modifico la matriz del grafo con los resultados obtenidos
    for(int i = 0; i < m.size(); i++)
    {
        for (int j = 0; j < m.size(); j++)
        {
            if((padre[i] != nodos[j].id) && (padre[j] != nodos[i].id))
            {
                m[i][j] = numeric_limits<double>::max();
            }
        }
    }

}

int calcularPesoDeCluster(vector<S_Nodo> &n)
{
    auto count = 0;
    for(auto &elem : n)
    {
        count += elem.demanda;
    }
    return count;
}

vector<vector<S_Nodo>> bfs(S_CVRP &C)
{
    int numeroDeComp = 0;
    vector<int> compConexas(C.cantNodos(), -1);
    vector<vector<S_Nodo>> clusters;
    vector<int> cola;
    //mientras haya componentes conexas que no sean -1
    while(find(compConexas.begin(),compConexas.end(),-1) != compConexas.end())
    {
        //Aumento el numero de componente
        numeroDeComp++;
        //Llevo registro de los nodos pertenecientes a la cc
        vector<S_Nodo> cluster;
        //Elijo mi proximo vertice no visitado
        int v;
        for(int i = 0; i < compConexas.size(); i++)
        {
            if(compConexas[i] == -1) v = i;
        }
        //le asigno su componente conexa
        compConexas[v] = numeroDeComp;
        cluster.push_back(C.getNodoById(v));
        cola.clear();
        cola.push_back(v);
        while(!cola.empty())
        {
            int w = cola.at(0);
            cola.erase(cola.begin());
            for(int i = 0; i < C.cantNodos(); i++)
            {
                if(C.getMatriz()[w][i] != numeric_limits<double>::max())
                {
                    if(compConexas[i] == -1){
                        compConexas[i] = numeroDeComp;
                        cluster.push_back(C.getNodoById(i));
                        cola.push_back(i);
                    }
                }
            }
        }
        clusters.push_back(cluster);
    }
    return clusters;
}

void calcularPromedioAux(S_CVRP &C, int d, double &sumaActual, int indice, pair<int,int> noConsiderar, int &cantEjes)
{
    if(d == 0){
        return;
    }else{
        for(int i = 0; i < C.cantNodos(); i++)
        {
            if(C.getMatriz()[indice][i] != numeric_limits<double>::max() && !(indice == noConsiderar.first && i == noConsiderar.second))
            {
                sumaActual = sumaActual + C.getMatriz()[indice][i];
                cantEjes = cantEjes + 1;
                pair<int,int> noRepetir(i,indice);
                calcularPromedioAux(C, d - 1, sumaActual, i, noRepetir, cantEjes);
            }
        }
    }
    return;
}

vector<vector<double>> calcularPromedios(S_CVRP &C, int d)
{
    vector<vector<double>> promedios(C.cantNodos(), vector<double>(C.cantNodos(),0));
    for(int i = 0; i < C.cantNodos(); i++)
    {
        for(int j = 0; j < C.cantNodos(); j++)
        {
            if(C.getMatriz()[i][j] != numeric_limits<double>::max())
            {
                double suma = 0;
                int cantEjes = 0;
                pair<int,int> noConsiderar = make_pair(i,j);
                calcularPromedioAux(C, d, suma, i, noConsiderar, cantEjes);

                if(cantEjes == 0) promedios[i][j] = 0;
                else promedios[i][j] =  suma / cantEjes;
            }else
            {
                promedios[i][j] = numeric_limits<double>::max();
            }
        }
    }
    return promedios;
}

vector<vector<S_Nodo> > detectarYEliminarEjesInconsistentes(S_CVRP &C, double param, int d)
{
    auto P = calcularPromedios(C,d);
    for(int i = 0; i < C.cantNodos(); i++)
    {
        for(int j = 0; j < C.cantNodos(); j++)
        {
            if(C.getMatriz()[j][i] / P[j][i] > param && C.getMatriz()[i][j] / P[i][j] > param)
            {
                //el eje es inconsistente!
                C.getMatriz()[i][j] = numeric_limits<double>::max();
                C.getMatriz()[j][i] = numeric_limits<double>::max();
            }
        }
    }
    return bfs(C);
}

vector<vector<S_Nodo> > partirCluster(S_CVRP &C, vector<S_Nodo> &cluster)
{
    vector<vector<S_Nodo> > particiones;
    if(calcularPesoDeCluster(cluster) > C.getCapacidad()){
        auto c1 = vector<S_Nodo>(cluster.begin(), cluster.begin() + (cluster.size() / 2));
        auto c2 = vector<S_Nodo>(cluster.begin() + (cluster.size() / 2),  cluster.end());
        auto p1 = partirCluster(C,c1);
        auto p2 = partirCluster(C,c2);
        particiones.insert(std::end(particiones), p1.begin(), p1.end());
        particiones.insert(std::end(particiones), p2.begin(), p2.end());
    }
    else
    {
        particiones.push_back(cluster);
    }

    return particiones;

}


vector<vector<S_Nodo> > clusterizadorTp2(S_CVRP &C, double param, int d)
{
    prim(C.getMatriz(), C.getNodos());
    auto clusters = detectarYEliminarEjesInconsistentes(C, param, d);
    vector<vector<S_Nodo> > clustersValidos;
    for(auto &cluster : clusters)
    {
        auto pCluster = partirCluster(C,cluster);
        clustersValidos.insert(std::end(clustersValidos), pCluster.begin(), pCluster.end());
    }

    return clustersValidos;
}

void eliminarDeposito(S_CVRP &C, vector<vector<S_Nodo>> &clusters){
    for(auto &cluster : clusters)
    {
        for(auto it = cluster.begin(); it != cluster.end();)
        {
            if(it->id == C.getDeposito())
            {
                cluster.erase(it);
            }else{
                it++;
            }
        }
    }
    for(auto it = clusters.begin(); it != clusters.end();)
    {
        if(it->empty())
        {
            clusters.erase(it);
        }
        else
        {
            it++;
        }

    }
}


p_solucion s_resolverCVRP(S_CVRP &C, double param, int d)
{
    double costoTotal = 0;

    Matriz m = C.getMatrizCopia();

    vector<vector<S_Nodo> > caminos;
    vector<vector<S_Nodo> > clusters = clusterizadorTp2(C, param, d);

    C.setMatriz(m);

    eliminarDeposito(C, clusters);
    for(auto &cluster : clusters){
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


    for(int i = 0; i < caminos.size(); i++){
        for(int j = 1; j < caminos[i].size(); j++){
            costoTotal += C.distanciaEntre(caminos[i][j].id, caminos[i][j-1].id);
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

    if(argc != 4){
        cout << "Param 1: set" << endl << "Param 2: 0 <= f_t" << endl << "Param 3: 0 <= d (int)" << endl;
        return 1;
    }

    // - cargo archivo -
    string fileName(argv[1]);

    // - parseo con TSPLib -
    TspData archivo = cargarTSP(fileName);

    // - creo el grafo -
    S_CVRP grafo = S_CVRP(archivo);

    // - Obtengo una solución heurística por EI
    p_solucion s_solEI = s_resolverCVRP(grafo,*argv[2],*argv[3]);

    // - Imprimo las rutas de la solución -
    s_solEI.imprimirRutas();


    return 0;
}

