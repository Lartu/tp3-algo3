#include <climits>
#include <chrono>
#include "../estructuras.h"
#include "../SimulatedAnnealing/simulatedAnnealing.h"
#define PI 3.14159265



vector<pair<int,double>> s_calcularAngulos(S_CVRP &G, int puntoComienzo){
    //vector de angulos
    vector<pair<int,double>> angulos;
    //Elijo un punto arbitrario donde empiezo a calcular que no sea el deposito
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

double s_calcularCosto(S_CVRP &C, vector<S_Nodo> &nodos)
{
    double costo = 0.0;
    for(int i = 0; i < C.cantNodos() - 1; ++i)
    {
        //Calculo el costo del recorrido
        costo += C.getMatriz()[nodos[i].id][nodos[i + 1].id];
    }
    return costo;
}


bool s_estaEnSolucion(vector<S_Nodo> nodos, S_Nodo nodo)
{
    for(auto &n : nodos)
    {
        if(n == nodo) return true;
    } return false;
}

void prim(Matriz &m, vector<S_Nodo> &nodos);

Matriz armarMatriz(S_CVRP &C, vector<S_Nodo> &nodos)
{
    Matriz matriz;
    //Genero la matriz de adyacencia
    for(int i = 0; i < nodos.size(); ++i){
        vector<double> fila;
        for(int j = 0; j < nodos.size(); ++j){
            fila.push_back(C.distanciaEntre(nodos[i].id, nodos[j].id));
        }
        matriz.push_back(fila);
    }
}



vector<S_Nodo> s_tspAGM(S_CVRP &C, vector<S_Nodo> &nodos){
    vector<S_Nodo> solucion;
    Matriz m = armarMatriz(C,nodos);
//    prim(m, nodos);
    S_Nodo deposito = C.getNodoById(C.getDeposito());
    solucion.push_back(deposito);
    //Agrego el nodo mas cercano al deposito
    for (auto &nodo : nodos)
    {
        if (C.getMatriz()[deposito.id][nodo.id] != numeric_limits<double>::max())
        {
            solucion.push_back(nodo);
        }
    }
    //Completo el camino hasta volver al nodo
    for (int i = 0; i < nodos.size(); ++i) {
        for (int j = 0; j < nodos.size(); ++j)
        {
            if (C.getMatriz()[nodos[i].id][nodos[j].id] != numeric_limits<double>::max())
            {
                solucion.push_back(nodos[j]);
            }
        }
    }
    solucion.push_back(deposito);
    return solucion;

}

// TSP usando nearest neighbor
vector<S_Nodo> s_tsp(S_CVRP &C, vector<S_Nodo> &nodos)
{
    vector<S_Nodo> solucion;
    //Busco el deposito
    S_Nodo deposito = C.getNodoById(C.getDeposito());
    //Agrego el deposito a la solucion
    solucion.push_back(deposito);
    //Llevo registro del minimo camino
    auto min_length = numeric_limits<double>::max();
    S_Nodo nodoAgregar;
    //Agrego el nodo mas cercano al deposito
    for (auto &nodo : nodos)
    {
        auto distancia = C.getMatriz()[deposito.id][nodo.id];
        if (distancia < min_length)
        {
            min_length = distancia;
            nodoAgregar = nodo;
        }
    }
    solucion.push_back(nodoAgregar);
    //Completo el camino hasta volver al nodo
    for (int i = 0; i < nodos.size(); ++i) {
        min_length = numeric_limits<double>::max();
        for (int j = 0; j < nodos.size(); ++j)
        {
            auto distancia = C.getMatriz()[nodos[i].id][nodos[j].id];
            if (distancia < min_length && !s_estaEnSolucion(solucion, nodos[j]))
            {
                min_length = distancia;
                nodoAgregar = nodos[j];
            }
        }
        if(!s_estaEnSolucion(solucion,nodoAgregar))
        {
            solucion.push_back(nodoAgregar);
        }
    }
    //agrego el deposito al final para representar la vuelta
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
    cout << clusters.size() << endl;

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
    cout << costoTotal << endl;
    
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

vector<vector<S_Nodo> > detectarYEliminarEjesInconsistentes(S_CVRP &C)
{
    auto P = calcularPromedios(C,4);
    for(int i = 0; i < C.cantNodos(); i++)
    {
        for(int j = 0; j < C.cantNodos(); j++)
        {
                if(C.getMatriz()[j][i] / P[j][i] > 5 && C.getMatriz()[i][j] / P[i][j] > 5)
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


vector<vector<S_Nodo> > clusterizadorTp2(S_CVRP &C)
{
    prim(C.getMatriz(), C.getNodos());
    auto clusters = detectarYEliminarEjesInconsistentes(C);
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


p_solucion s_resolverCVRP2(S_CVRP &C)
{
    double costoTotal = 0;

    Matriz m = C.getMatrizCopia();

    vector<vector<S_Nodo> > caminos;
    vector<vector<S_Nodo> > clusters = clusterizadorTp2(C);

    C.setMatriz(m);

    eliminarDeposito(C, clusters);
    for(auto &cluster : clusters){
        caminos.push_back(s_tsp(C,cluster));
    }
    //Imprimo la cantidad de camiones que se usaron
    cout << clusters.size() << endl;
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
    cout << costoTotal << endl;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////JAJA/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void s_medirYPromediarCF(vector<S_CVRP> &instancias,double Ts, int n,int cs, ofstream& ofs){
    double tiempoTotal = 0;
    for(auto &instancia : instancias){
        auto start = chrono::steady_clock::now();
        s_resolverCVRP(instancia,0);
        auto end = chrono::steady_clock::now();
        auto diff = end - start;
        auto diffTiempo = chrono::duration <double, milli> (diff).count();
        tiempoTotal += diffTiempo;
    }
    auto promedio = tiempoTotal / instancias.size();
    ofs << instancias[0].getNodos().size() << "," << promedio << endl;
}

void s_medirTiemposVariandoCS(vector<S_CVRP> &instancias, ofstream &cf0, ofstream &cf1,ofstream &cf2){
    s_medirYPromediarCF(instancias,10,1000,0,cf0);
    s_medirYPromediarCF(instancias,10,1000,1,cf1);
    s_medirYPromediarCF(instancias,10,1000,2,cf2);
}

void s_medirTiemposVariandoNIt(vector<S_CVRP> &instancias, ofstream &minimoIt, ofstream &medioIt,ofstream &maximoIt){
    s_medirYPromediarCF(instancias,10,100,0,minimoIt);
    s_medirYPromediarCF(instancias,10,1000,0,medioIt);
    s_medirYPromediarCF(instancias,10,10000,0,maximoIt);
}

void s_generarVectoresDeInstancias(int nStart, int nEnd, int numeroMediciones,ofstream& ofs1,ofstream& ofs2,ofstream& ofs3, void(*fMedicion)(vector<S_CVRP>&,ofstream&,ofstream&,ofstream&)){

    //geteo todas las instancias de a un n por vez(por si son demasiadas)
    for(int i = nStart; i <= nEnd; i++){
        vector<S_CVRP> instancias = vector<S_CVRP>();
        string i_string = to_string(i);
        for(int j = 1; j <= numeroMediciones; j++){
            string j_string = to_string(j);
            //Ruta a getear. Cambienla si necesitan. No la pase por parametro porque es bien fea
            string ruta = "../Generador de instancias/instancias_generadas/" + i_string + "-elementos/LPS-n" + i_string + "-m" + j_string + ".vrp";
            ifstream stream(ruta, ios::in);
            if(stream.ios::good()){
                TspData archivo = cargarTSP(ruta);
                S_CVRP G = S_CVRP(archivo);
                instancias.push_back(G);
            }
        }
        auto asd = instancias.size();
        //Aca llamen a su funcion para hacer las mediciones (la mia es p_medirTiempos)
        if(!instancias.empty())
            fMedicion(instancias, ofs1,ofs2,ofs3);//p_medirTiempos(instancias,cs0,cs1,cs2);
    }
}

int main()
{

//    cout << "Cargando TSP:" << endl;
//    //../Codigo/A-n32-k5.vrp.txt
//    //../Codigo/X-n101-k25.vrp.txt
//    //../Codigo/X-n916-k207.vrp.txt
//    //../Codigo/X-n979-k58.vrp.txt
//    //../Codigo/X-n1001-k43.vrp.txt
//    TspData archivo = cargarTSP("../Codigo/X-n1001-k43.vrp.txt");
//
//    S_CVRP C = S_CVRP(archivo);
////    clusterizadorTp2(C);
//
//    p_solucion p = s_resolverCVRP(C,1);
////    p.esSolucion(C);
////    p.imprimirRutas();
//    p_solucion p2 = s_resolverCVRP2(C);
//////    p2.imprimirRutas();
//    cout << p2.esSolucion(C);
////    p2.imprimirRutas();

    //Abro los archivos en modo append para no pisarme
    ofstream cf0("cf0", ios::out);
    ofstream cf1("cf1", ios::out);
    ofstream cf2("cf2", ios::out);
    s_generarVectoresDeInstancias(4,100,1,cf0,cf1,cf2,s_medirTiemposVariandoCS);


    return 0;
}
