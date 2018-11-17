#include "estructuras.h"
#include <chrono>

//Función que marca como no visitable un nodo en la matriz de distancias
void l_tacharNodo(Matriz & distancias, int nodo)
{
    // - Largo del lado de la matriz (que se espera cuadrada) -
    int ladoMatriz = distancias.size();
    
    // - Marco no visitable (-1) al nodo en la matriz -
    for(int i = 0; i < ladoMatriz; ++i)
    {
        distancias[i][nodo] = -1;
    }
}

//Función que devuelve el próximo camino más corto disponible en la
//matriz de distancias y el nodo al que dicho camino lleva
int l_proximoNodo(const Matriz & distancias, int nodoActual)
{
    // - Largo del lado de la matriz (que se espera cuadrada) -
    int ladoMatriz = distancias.size();
    
    // - Guardo el nodo al que puedo ir y su distancia -
    int proxNodo = -1; //Nodo al que voy a ir
    int minDist  = -1; //Distancia hasta el nodo
    
    // - Veo hasta donde puedo ir en el menor camino -
    for(int i = 0; i < ladoMatriz; ++i)
    {
        // - No voy a ir del nodo actual al nodo actual -
        if(nodoActual == i) continue;
        
        // - Guardo la distancia para tenerla más cómoda -
        double distancia = distancias[nodoActual][i];
        
        // - Busco el próximo nodo al que puedo ir -
        if(minDist == -1 || (distancia < minDist && distancia != -1))
        {
            proxNodo = i;
            minDist  = distancia;
        }
    }
    
    // - Devuelvo el nodo al que voy a ir -
    return proxNodo;
}

//TODO poner lindo y explicar esto
double l_calcularDistanciaRuta(const S_CVRP & grafo, const ruta & rutaActual){
    double costoTotal = 0;
    for(int i = 0; i < rutaActual.size() - 1; ++i){
        costoTotal += grafo.distanciaEntre(rutaActual[i], rutaActual[i+1]);
    }
    return costoTotal;
}

//TODO poner lindo y explicar esto
double l_calcularCosteRuta(const S_CVRP & grafo, const ruta & rutaActual)
{
    // - Coste total de la ruta actual -
    int costeRuta = 0;
    
    // - Sumo cada nodo -
    for (int nodo : rutaActual)
    {
        costeRuta += grafo.getDemanda(nodo);
    }
    return costeRuta;
}

//TODO poner lindo todo esto y explicarlo
void l_combinarCaminos(vector<ruta> & rutas, int r1, int r2)
{
    ruta nuevaRuta;
    nuevaRuta.reserve(rutas[r1].size() + rutas[r2].size());
    nuevaRuta.insert(nuevaRuta.end(), rutas[r1].begin(), rutas[r1].end()-1);
    nuevaRuta.insert(nuevaRuta.end(), rutas[r2].begin()+1, rutas[r2].end());
    rutas.push_back(nuevaRuta);
    rutas[r1].clear();
    rutas[r2].clear();
}

// Función que resuelve un grafo pasado como S_CVRP usando la heurística
// de Próximo Mínimo
p_solucion l_heuristicaProxMinimo(const S_CVRP & grafo)
{
    // - Inicializo estructuras que voy a pasar en la solución -
    vector<ruta> rutas;
    vector<int> cargas;
    
    // - Cantidad de nodos que quedan sin visitar -
    int nodosSinVisitar = grafo.cantNodos() - 1; // -1 por deposito
    
    // - Obtengo las distancias -
    Matriz distancias = grafo.getMatriz();
    
    // - Defino el punto desde donde empiezo -
    int puntoActual = grafo.getDeposito();
    
    // - Creo un camino vacío para ir agregando nodos -
    ruta rutaActual;
    
    // - Pusheo el depósito para empezar -
    rutaActual.push_back(puntoActual);
    
    // - Mientras haya nodos sin visitar, genero rutas sin tener en
    // cuenta las capacidades de los camiones ni nada -
    while(nodosSinVisitar > 0)
    {
        // - Tomo el nodo al que puedo moverme con menor distancia -
        puntoActual = l_proximoNodo(distancias, puntoActual);
        
        // - Lo agrego a mi camino actual -
        rutaActual.push_back(puntoActual);
        
        // - Si no es un depósito, marco este nodo como no visitable -
        if(puntoActual != grafo.getDeposito())
        {
            // - Un nodo menos por visitar -
            --nodosSinVisitar;
            
            // - Lo tacho como no visitable -
            l_tacharNodo(distancias, puntoActual);
        }
        
        // - Si el nodo es un depósito, pusheo el camino a rutas -
        else
        {
            // - Guardo la ruta creada -
            rutas.push_back(rutaActual);
            
            // - Vacío mi contenedor de rutas -
            rutaActual.clear();
            
            // - Pusheo el depósito para empezar -
            rutaActual.push_back(puntoActual);
        }
    }
    
    // - Si quedó una ruta todavía sin pushear, le agrego un depot
    // y la pusheo -
    if(rutaActual.size() > 0)
    {
        rutaActual.push_back(grafo.getDeposito());
        rutas.push_back(rutaActual);
    }
    
    // - Reviso cada ruta revisando que las capacidades se cumplan -
    for (int j = 0; j < rutas.size(); ++j)
    {
        // - Guardo la capacidad de mi camión actual -
        int capacidadActual = grafo.getCapacidad();
        
        // - Reviso cada punto de la ruta -
        for(int i = 0; i < rutas[j].size(); ++i)
        {
            // - Si estoy en un depósito, o estoy saliendo o llegué -
            if(rutas[j][i] == grafo.getDeposito()) continue;
            
            // - Obtengo la demanda de este punto -
            int demandaNodo = grafo.getDemanda(rutas[j][i]);
            
            // - Si todavía tengo espacio en el camión -
            if(demandaNodo <= capacidadActual)
            {
                // - Saco espacio al camión -
                capacidadActual -= demandaNodo;
            }
            
            // - Si me quedé sin espacio, mando otro camión -
            else
            {
                // - Creo dos nuevas rutas -
                ruta nuevaRuta1;
                ruta nuevaRuta2;
                
                // - Agrego un depot al principio de lo que no recorrí -
                nuevaRuta2.push_back(grafo.getDeposito());
                
                // - Pueblo ambas rutas con partes de la ruta actual -
                for(int k = 0; k < rutas[j].size(); ++k)
                {
                    // - Guardo lo que pude recorrer hasta ahora en 
                    // la ruta1 -
                    if(k < i) nuevaRuta1.push_back(rutas[j][k]);
                    
                    // - Guardo lo que todavía no recorrí en la otra -
                    else{
                        nuevaRuta2.push_back(rutas[j][k]);
                    }
                }
                
                // - Agrego un depot al final de lo que ya recorrí -
                nuevaRuta1.push_back(grafo.getDeposito());
                
                // - Pusheo la ruta 2 a rutas -
                rutas.push_back(nuevaRuta2);
                
                // - Reemplazo la ruta actual por ruta 1 -
                rutas[j] = nuevaRuta1;
                
                // - Sigo revisando la siguiente ruta -
                break;
            }
        }
    }
    
    //TODO emprolijar este mamarracho
    bool reduje = false;
    do{
        // - Obtengo los costes en demanda de cada ruta -
        cargas.clear();
        for (ruta rutaActual : rutas)
        {
            cargas.push_back(l_calcularCosteRuta(grafo, rutaActual));
        }
        
        //TODO poner linda esta parte
        // - Veo si es posible combinar rutas -  (n^2)
        vector<pair<double, pair<int, int>>> posiblesCombinaciones; //Peso, camino 1 - camino 2
        for(int i = 0; i < cargas.size(); ++i)
        {
            for(int j = 0; j < cargas.size(); ++j)
            {
                if(i == j) continue;
                //Ver si puedo sumarle al más grande
                if(cargas[i] + cargas[i] < grafo.getCapacidad())
                {
                    ruta ruta1(rutas[i].begin(), rutas[i].end()-1);
                    ruta ruta2(rutas[j].begin()+1, rutas[j].end());
                    //Veo si los caminos sirven (o sea, que las distancias sean menos) y si los costes me dan
                    if(l_calcularDistanciaRuta(grafo, rutas[i]) + l_calcularDistanciaRuta(grafo, rutas[j]) > l_calcularDistanciaRuta(grafo, ruta1) + l_calcularDistanciaRuta(grafo, ruta2) + grafo.distanciaEntre(ruta1[ruta1.size()-1], ruta2[0]) && l_calcularCosteRuta(grafo, ruta1) + l_calcularCosteRuta(grafo, ruta2) <= grafo.getCapacidad())
                    {
                        double ahorro = l_calcularDistanciaRuta(grafo, rutas[i]) + l_calcularDistanciaRuta(grafo, rutas[j]) - l_calcularDistanciaRuta(grafo, ruta1) - l_calcularDistanciaRuta(grafo, ruta2) - grafo.distanciaEntre(ruta1[ruta1.size()-1], ruta2[0]);
                        posiblesCombinaciones.push_back(make_pair(ahorro, make_pair(i, j)));
                    }
                    //Se prueban las dos combinaciones posibles porque i y j van en el mismo rango entonces prueban x/y y y/x.
                }
            }
        }
        
        reduje = true;
        double ahorroTotal = 0;
        if(!posiblesCombinaciones.empty()){
            //TODO poner linda esta parte
            //Combino todo lo que pueda, del mejor al peor
            vector<bool> combinacionesHechas(grafo.cantNodos(), false);
            sort(posiblesCombinaciones.rbegin(), posiblesCombinaciones.rend());
            for(auto comb : posiblesCombinaciones){
                int primerCamino = comb.second.first;
                int segundoCamino = comb.second.second;
                //Si no combine estos caminos ya
                if(!combinacionesHechas[primerCamino] && !combinacionesHechas[segundoCamino])
                {
                    combinacionesHechas[primerCamino] = true;
                    combinacionesHechas[segundoCamino] = true;
                    l_combinarCaminos(rutas, primerCamino, segundoCamino);
                    reduje = true;
                    ahorroTotal += comb.first;
                }
            }
            
            //TODO poner esto lindo
            //Borro rutas que dejé vacías
            auto it = rutas.begin();
            while(it != rutas.end()){
                if(it->empty()){
                    rutas.erase(it);
                    it = rutas.begin();
                }else{
                    ++it;
                }
            }
            
            // - Recalculo la demanda total -
            cargas.clear();
            for (ruta rutaActual : rutas)
            {
                cargas.push_back(l_calcularCosteRuta(grafo, rutaActual));
            }
        }
    }while(!reduje);
    
    // - Calculo la distancia total recorrida -
    double costoTotal = 0;
    for (ruta rutaActual : rutas)
    { 
        costoTotal += l_calcularDistanciaRuta(grafo, rutaActual);
    }
    
    // - Genero la estructura solución y la devuelvo -
    p_solucion solucion(rutas, cargas, costoTotal);
    return solucion;
}

int main(int argc, char** argv)
{
    // - cargo archivo -
	string fileName(argv[1]);
    
    // - parseo con TSPLib -
	TspData archivo = cargarTSP(fileName);
    
    // - creo el grafo -
    S_CVRP grafo = S_CVRP(archivo);
    
    // - Inicializo el timer -
    auto tiempo_inicio = chrono::steady_clock::now();
    auto tiempo_fin = chrono::steady_clock::now();
    auto tiempo_diferencia = (tiempo_fin - tiempo_inicio);
    
    // - Obtengo lo que me costaría sin optimizar -
    tiempo_inicio = chrono::steady_clock::now();
    double costeSinOptimizar = l_costeSinOptimizar(grafo);
    tiempo_fin = chrono::steady_clock::now();
    tiempo_diferencia = (tiempo_fin - tiempo_inicio);
    cout << "@CosteCanonico: ";
    cout << costeSinOptimizar;
    cout << " (duración: " << chrono::duration <double, milli> (tiempo_diferencia).count() << " ms)" << endl;
    
    // - Obtengo una solución heurística por proxMinimo -
    tiempo_inicio = chrono::steady_clock::now();
    p_solucion l_sol = l_heuristicaProxMinimo(grafo);
    tiempo_fin = chrono::steady_clock::now();
    tiempo_diferencia = (tiempo_fin - tiempo_inicio);
    cout << "@CosteOptimizado: ";
    cout << l_sol.costoTotal;
    cout << " (duración: " << chrono::duration <double, milli> (tiempo_diferencia).count() << " ms)" << endl;
    
    // - Imprimo mejora porcentual -
    double porcentaje = l_sol.costoTotal * 100 / costeSinOptimizar;
    cout << "@PorcentajeCosteFinal: ";
    cout << porcentaje << "%" << endl;
}
