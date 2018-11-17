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


p_solucion s_resolverCVRP2(S_CVRP &C, double param, int d)
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////JAJA/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double s_parametroOptimoTP2(vector<S_CVRP> &instancias, ofstream& ofs, double i, int d){
    double costoTotal = 0;
    for(auto &instancia : instancias){
        costoTotal += s_resolverCVRP2(instancia, i, d).costoTotal;
    }

    auto promedio = costoTotal / instancias[0].getNodos().size();
    return promedio;
}


double s_medirYPromediarCF1(vector<S_CVRP> &instancias, ofstream& ofs, double d, int i){
    double tiempoTotal1 = 0;
    double costoTotal1 = 0;
    for(auto &instancia : instancias){
        auto start = chrono::steady_clock::now();
        costoTotal1 += s_resolverCVRP(instancia,0).costoTotal;
        auto end = chrono::steady_clock::now();
        auto diff = end - start;
        auto diffTiempo = chrono::duration <double, milli> (diff).count();
        tiempoTotal1 += diffTiempo;
    }
    auto promedio1 = tiempoTotal1 / instancias.size();
    ofs << instancias[0].getNodos().size() << "," << promedio1 << endl;
    return 0.0;
}

void s_medirYPromediarCF(vector<S_CVRP> &instancias, ofstream& ofs){
    double tiempoTotal1 = 0;
    double costoTotal1 = 0;
    for(auto &instancia : instancias){
        auto start = chrono::steady_clock::now();
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 eng(rd()); // seed the generator
        std::uniform_int_distribution<> distr(0, (int)instancia.cantNodos() - 1); // define the range
        costoTotal1 += s_resolverCVRP(instancia,distr(eng)).costoTotal;
        auto end = chrono::steady_clock::now();
        auto diff = end - start;
        auto diffTiempo = chrono::duration <double, milli> (diff).count();
        tiempoTotal1 += diffTiempo;
    }
    double tiempoTotal2 = 0;
    double costoTotal2 = 0;
    for(auto &instancia : instancias){
        auto start = chrono::steady_clock::now();
        costoTotal2 += s_resolverCVRP2(instancia, 0.0, 1).costoTotal;
        auto end = chrono::steady_clock::now();
        auto diff = end - start;
        auto diffTiempo = chrono::duration <double, milli> (diff).count();
        tiempoTotal2 += diffTiempo;
    }

    auto promedio1 = tiempoTotal1 / instancias.size();
    auto promedio2 = tiempoTotal2 / instancias.size();
    auto costoPromedio1 = costoTotal1 / instancias.size();
    auto costoPromedio2 = costoTotal2 / instancias.size();
    ofs << instancias[0].getNodos().size() << "," << promedio1 << "," << promedio2 << "," << costoPromedio1 << "," << costoPromedio2 << endl;
}

double s_medirTiemposVariandoFT(vector<S_CVRP> &instancias, ofstream &cf1,double i, int d){
      return s_parametroOptimoTP2(instancias, cf1, i, d);
}


//void s_calcularPromedioConConocidos(vector<pair<S_CVRP, double> > &instancias, ofstream &of)
//{
//    double costoTotal = 0;
//    for(auto &instancia : instancias){
//        costoTotal += s_resolverCVRP2(instancia, i).costoTotal;
//    }
//
//    auto promedio = costoTotal / instancias[0].getNodos().size();
//    return promedio;
//}

void s_generarVectoresDeInstancias(int nStart, int nEnd, int numeroMediciones,ofstream& ofs1,double param, int d, double(*fMedicion)(vector<S_CVRP>&,ofstream&,double, int)){

    double costoTotal = 0.0;
    int nIteracion = 0;
    //geteo todas las instancias de a un n por vez(por si son demasiadas)
    for(int i = nStart; i <= nEnd; i++){
        vector<S_CVRP> instancias = vector<S_CVRP>();
        string i_string = to_string(i);
        for(int j = 1; j <= numeroMediciones; j++){
            string j_string = to_string(j);
            //Ruta a getear. Cambienla si necesitan. No la pase por parametro porque es bien fea
            string ruta = "instancias_generadas/" + i_string + "-elementos/LPS-n" + i_string + "-m" + j_string + ".vrp";
            ifstream stream(ruta, ios::in);
            if(stream.ios::good())
            {
                TspData archivo = cargarTSP(ruta);
                S_CVRP G = S_CVRP(archivo);
                instancias.push_back(G);
            }
        }
        //Aca llamen a su funcion para hacer las mediciones (la mia es p_medirTiempos)
            if(!instancias.empty()) {
                costoTotal += fMedicion(instancias, ofs1, param, d);//p_medirTiempos(instancias,cs0,cs1,cs2
                nIteracion++;
            }
    }
//    ofs1 << param << "," << costoTotal / nIteracion << endl;
}



void s_correrInstanciasConocidas(int nStart, int nEnd, int numeroMediciones, int algo, ofstream& ofs1, void(*fMedicion)(vector<pair<S_CVRP,double>>&,int,double,ofstream&, bool)){

    //geteo todas las instancias de a un n por vez(por si son demasiadas)
    for(int i = nStart; i <= nEnd; i++){
        vector<pair<S_CVRP,double>> instancias = vector<pair<S_CVRP,double>>();
        string i_string = to_string(i);
        for(int j = 1; j <= numeroMediciones; j++){
            string j_string = to_string(j);
            //Ruta a getear. Cambienla si necesitan. No la pase por parametro porque es bien fea
            string rutaVRP = "serieA/A-n" + i_string + ".vrp";
            string rutaSol = "serieA/A-n" + i_string + ".sol";
            ifstream stream(rutaVRP, ios::in);
            ifstream streamSol(rutaSol, ios::in);
            if(stream.ios::good()){
                TspData archivo = cargarTSP(rutaVRP);
                S_CVRP G = S_CVRP(archivo);
                string solString;
                getline(streamSol,solString,'\n');
                double sol = stod(solString);
                instancias.push_back(make_pair(G,sol));
            }
        }
        //Aca llamen a su funcion para hacer las mediciones (la mia es p_medirTiempos)
            if(algo == 1)
            {
                if(!instancias.empty()) {
                            fMedicion(instancias, 2, 1, ofs1, true);//p_medirTiempos(instancias,cs0,cs1
                }

            }
            else
            {
                        fMedicion(instancias, 0, 0.0, ofs1, false);
            }

    }
}

double p_medirConocidasNIt(pair<S_CVRP,double> &instancia,double f_t, int d, ofstream& ofs){
    p_solucion solCanonica(instancia.first);
    double costoCanonico = solCanonica.costoTotal;
    p_solucion solSA = s_resolverCVRP2(instancia.first,f_t,d);
    return (1 - (solSA.costoTotal/costoCanonico)) * 100;
    //ofs << instancias[0].first.getNodos().size() << "@" << Ts << "{}" << n << "," << (1 - (solSA.costoTotal/costoCanonico)) * 100<< ":" << endl;
}

void p_conocidasTodo(vector<pair<S_CVRP,double>> &instancias, ofstream& ofs){
    double maximo = 0;
    int maxft = 0;
    int maxd = 0;
    for(int k = 1; k <= 4; k++){
        for(double i = 0.5; i <= 7; i+= 0.5) {
            double total = 0;
            for (int w = 0; w < instancias.size(); w++) {
                double actual = p_medirConocidasNIt(instancias[w], i, k, ofs);
                total += actual;
            }
            if (total > maximo) {
                maximo = total;
                maxft = i;
                maxd = k;
            }
        }

    }
    ofs << "d: " << maxd<<" ft: " << maxft << endl;
    cout << "d: " << maxd<<" ft: " << maxft << endl;

}



void p_correrInstanciasConocidasJuntas(int nStart, int nEnd, ofstream& ofs1, void(*fMedicion)(vector<pair<S_CVRP,double>>&,ofstream&)){
    vector<pair<S_CVRP,double>> instancias = vector<pair<S_CVRP,double>>();
    //geteo todas las instancias de a un n por vez(por si son demasiadas)
    for(int i = nStart; i <= nEnd; i++){
        string i_string = to_string(i);
        for(int j = 1; j <= 1; j++){
            string j_string = to_string(j);
            //Ruta a getear. Cambienla si necesitan. No la pase por parametro porque es bien fea
            string rutaVRP = "serieX/X-n" + i_string + ".vrp";
            string rutaSol = "serieX/X-n" + i_string + ".sol";
            ifstream stream(rutaVRP, ios::in);
            ifstream streamSol(rutaSol, ios::in);
            if(stream.ios::good()){
                TspData archivo = cargarTSP(rutaVRP);
                S_CVRP G = S_CVRP(archivo);
                string solString;
                getline(streamSol,solString,'\n');
                double sol = stod(solString);
                instancias.push_back(make_pair(G,sol));
            }
        }
        //Aca llamen a su funcion para hacer las mediciones (la mia es p_medirTiempos)
    }
    if(!instancias.empty()) fMedicion(instancias, ofs1);
}

void p_medirPorcentajeEntreCanYOpt(vector<pair<S_CVRP,double>> &instancias, int i,double j, ofstream& ofs, bool algo){
    if(algo) {
        for (auto &instancia : instancias) {
            p_solucion solCanonica(instancia.first);
            double costoCanonico = solCanonica.costoTotal;
            p_solucion solSA = s_resolverCVRP2(instancia.first,j,i);
            double porcentajeAhorrado =
                    100 - (solSA.costoTotal - instancia.second) * 100 / (costoCanonico - instancia.second);
            cout << "canonica: " << costoCanonico << endl;
            cout << "costo: " << solSA.costoTotal << endl;
            cout << "optima: " << instancia.second << endl;
            ofs << instancia.first.getNodos().size() << "," << porcentajeAhorrado << "," << j << "," << i << endl;
        }
    } else{
        for (auto &instancia : instancias) {
            p_solucion solCanonica(instancia.first);
            double costoCanonico = solCanonica.costoTotal;
            p_solucion solSA = s_resolverCVRP(instancia.first, i);
            double porcentajeAhorrado =
                    100 - (solSA.costoTotal - instancia.second) * 100 / (costoCanonico - instancia.second);
            cout << "canonica: " << costoCanonico << endl;
            cout << "costo: " << solSA.costoTotal << endl;
            cout << "optima: " << instancia.second << endl;
            ofs << instancia.first.getNodos().size() << "," << porcentajeAhorrado << "," << i << endl;
        }
    }
}

int main()
{

    cout << "Cargando TSP:" << endl;
    //../Codigo/A-n32-k5.vrp.txt
    //../Codigo/X-n101-k25.vrp.txt
    //../Codigo/X-n916-k207.vrp.txt
    //../Codigo/X-n979-k58.vrp.txt
    //../Codigo/X-n1001-k43.vrp.txt
//    TspData archivo = cargarTSP("../Codigo/A-n32-k5.vrp.txt");

//    S_CVRP C = S_CVRP(archivo);
//    clusterizadorTp2(C);

//    p_solucion p = s_resolverCVRP(C,1);
//    cout << p.esSolucion(C) << endl;
////    p.imprimirRutas();
//    p_solucion p2 = s_resolverCVRP2(C);
//////    p2.imprimirRutas();
//    p_solucion pc = p_solucion(C);
//    cout << "Solucion canonica: " << pc.costoTotal << endl;
////    p2.imprimirRutas();

    //Abro los archivos en modo append para no pisarme
    ofstream cf0("ejesPerfo.csv", ios::app);
//    ofstream cf1d1("cf2d1.csv", ios::app);
//    ofstream cf1d2("cf2d2.csv", ios::app);
//    ofstream cf1d3("cf2d3.csv", ios::app);
//    ofstream cf1d4("cf2d4.csv", ios::app);
//    ofstream cf2("pocomparativa", ios::out);
//    ofstream cf1casoOptimo("casoOptimoCSV1.csv", ios::app);
//    ofstream cf2casoOptimo("casoOptimoCSV.csv", ios::app);
//    ofstream cf2OPTIMO("cf2optimoX.csv", ios::app);
//    s_correrInstanciasConocidas(1,100,1,)
//    s_generarVectoresDeInstancias(3,100,400,cf0,0,0,s_medirYPromediarCF1);
//    p_correrInstanciasConocidasJuntas(101,655,cf2OPTIMO,p_conocidasTodo);
    s_generarVectoresDeInstancias(3,1003,400,cf0,1,1,s_medirYPromediarCF1);
//
//    cout << "calculando p optimo caso 1" << endl;
//    cf1casoOptimo << "n,porcentaje,ejeInicial" << endl;
//    s_correrInstanciasConocidas(32,100,1,2,cf1casoOptimo,p_medirPorcentajeEntreCanYOpt);
//
//    cout << "calculando p optimo caso 2" << endl;
//    cf2casoOptimo << "n,porcentaje,f_t,d" << endl;
//    s_correrInstanciasConocidas(32,100,1,1,cf2casoOptimo,p_medirPorcentajeEntreCanYOpt);
//
//
//    cf1d1 << "i,costoTotal" << endl;
//    cout << "Calculando tp2 con d = 1" << endl;
//    for(double i = 0.0; i <= 7; i = i + 0.5)
//    {
//        s_generarVectoresDeInstancias(3,100,400,cf1d1,i,1,s_medirTiemposVariandoFT);
//    }
//
//    cf1d2 << "i,costoTotal" << endl;
//    cout << "Calculando tp2 con d = 2" << endl;
//    for(double i = 0.0; i <= 7; i = i + 0.5)
//    {
//        s_generarVectoresDeInstancias(3,100,400,cf1d2,i,2,s_medirTiemposVariandoFT);
//    }
//
//    cf1d3 << "i,costoTotal" << endl;
//    cout << "Calculando tp2 con d = 3" << endl;
//    for(double i = 0.0; i <= 7; i = i + 0.5)
//    {
//        s_generarVectoresDeInstancias(3,100,400,cf1d3,i,3,s_medirTiemposVariandoFT);
//    }
//
//    cf1d4 << "i,costoTotal" << endl;
//    cout << "Calculando tp2 con d = 4" << endl;
//    for(double i = 0.0; i <= 7; i = i + 0.5)
//    {
//        s_generarVectoresDeInstancias(3,100,400,cf1d4,i,4,s_medirTiemposVariandoFT);
//    }








    return 0;
}
