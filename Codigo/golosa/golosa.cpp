#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include "../estructuras.h"
#include "../SimulatedAnnealing/simulatedAnnealing.h"
#include <chrono>
int counter = 0;
typedef vector<int> ruta;
using namespace std;
void p_mergeRutas(const S_CVRP &G, p_solucion &S,int numeroRuta1, int numeroRuta2){
	bool numeroRuta1MayorQueRuta2 = numeroRuta1 > numeroRuta2;
	//Agarro referencias a las rutas involucradas
	ruta& ruta1 = S.rutas[numeroRuta1];
	ruta& ruta2 = S.rutas[numeroRuta2];
	//Consigo el extremo de cada ruta. primero el extremo final de la primera ruta
	int primerElemento = S.rutas[numeroRuta1][ruta1.size()-2];
	//y el primer elemento que no sea el deposito de la segunda ruta
	int segundoElemento = S.rutas[numeroRuta2][1];
	//calculo el costo de la nueva carga
	double nuevaCarga = S.cargas[numeroRuta1] + S.cargas[numeroRuta2];
	//calculo el nuevo costo total
	double nuevoCostoTotal = S.costoTotal + G.distanciaEntre(primerElemento,segundoElemento) - G.distanciaEntre(primerElemento,G.getDeposito()) - G.distanciaEntre(segundoElemento,G.getDeposito());
	if(nuevoCostoTotal == numeric_limits<double>::max()){
		cout << "INF" << endl;
		cout << "ruta1: " << ruta1 << endl;
		cout << "ruta2: " << ruta2 << endl;
		cout << "costoTotal: " << S.costoTotal << endl;
		cout << "primerElemento: " << primerElemento << endl;
		cout << "segundoElemento: " << segundoElemento << endl;
		cout << "numeroRuta1: " << numeroRuta1 << endl;
		cout << "numeroRuta2: " << numeroRuta2 << endl;
		cout << "Dist Prim elem, segund elem: " << G.distanciaEntre(primerElemento,segundoElemento) << endl;
		cout << "Dist prim elem, deposito: " << G.distanciaEntre(primerElemento,G.getDeposito()) << endl;
		cout << "Dist segund elem, deposito: " << G.distanciaEntre(segundoElemento,G.getDeposito()) << endl;
	}
	//actualizo la ruta1. Primero, saco el deposito al final de la ruta1
	ruta1.erase(ruta1.end() - 1);
	//saco el deposito al principio de la ruta2
	ruta2.erase(ruta2.begin());
	//concateno las rutas
	ruta1.insert(ruta1.end(),ruta2.begin(),ruta2.end());
	//actualizo costo total y cargas
	S.costoTotal = nuevoCostoTotal;
	S.cargas[numeroRuta1] = nuevaCarga;
	//elimino ruta2 de rutas y de cargas
	S.rutas.erase(S.rutas.begin() + numeroRuta2);
	S.cargas.erase(S.cargas.begin() + numeroRuta2);

}
//Busco linealmente a que numero de ruta pertenece un elemento.
int p_rutaPertenece(int elem, vector<ruta> &rutas){
	for(int i = 0; i < rutas.size(); i++){
		for(int j = 0; j < rutas[i].size(); j++){
			if(rutas[i][j] == elem) return i;
		}
	}
	return -1;
}

bool p_invarianteMatriz(const S_CVRP &G){
	bool result = true;
	vector<pair<int,int>> paresInf = vector<pair<int,int>>();
	for(int i = 0; i < G.getMatrizConst().size(); i++){
		for(int j = 0; j < G.getMatrizConst().size(); j++){
			if(G.distanciaEntre(i,j) == numeric_limits<double>::max()){
				if(i != j){
					result = false;
				}
				paresInf.push_back(make_pair(i,j));
			}
		}
	}
	cout << "{";
	for(int i = 0; i < paresInf.size(); i++){
		cout << "("<< paresInf[i].first << "," << paresInf[i].second << "), "<< endl;	
	}
	cout << "}";
	return result;
}

p_solucion p_golosa(const S_CVRP &G){
	//Creo la solucion canonica
	p_solucion S = p_solucion(G);
	//Recorro una vez la matriz, armando un vector de triplas con i, j y la distancia i y j 
	//para todo par de nodos
	vector<vector<int>> pares;
	for(int i = 1; i < G.getMatrizConst().size(); i++){
		for(int j = 1; j < G.getMatrizConst().size(); j++){
			if(i != j){
				vector<int> tripla = vector<int>();
				tripla.push_back(i);
				tripla.push_back(j);
				tripla.push_back(G.distanciaEntre(i,j));
				pares.push_back(tripla);
			}
		}
	}
	sort(pares.begin(),pares.end(), [](vector<int> &a, vector<int> &b){ return a[2] < b[2];});
	int k = 0;
	int j = 0;
	while(k < G.getMatrizConst().size()-1 && j < pares.size()){
		bool insertado = false;
		while(!insertado && j < pares.size()){
			int rutaPrimerElemento = p_rutaPertenece(pares[j][0], S.rutas);
			int rutaSegundoElemento = p_rutaPertenece(pares[j][1], S.rutas);
			if((rutaPrimerElemento != rutaSegundoElemento) && S.cargas[rutaPrimerElemento] + S.cargas[rutaSegundoElemento] <= G.getCapacidad()){
				p_mergeRutas(G,S,rutaPrimerElemento,rutaSegundoElemento);
				insertado = true;
			}
			j++;
		}

		k++;
	}
	//cout << S.costoTotal << endl;
	//S.imprimirRutas();
	return S;
}


void p_medirYPromediarGolosa(vector<S_CVRP> &instancias,ofstream& ofs){
	counter++;
	cout << "medir: "<< counter<< endl; 
	double tiempoTotal = 0;
	vector<double> tiempos = vector<double>();
	for(auto &instancia : instancias){
		auto start = chrono::steady_clock::now();
		p_golosa(instancia);
		auto end = chrono::steady_clock::now();
		auto diff = end - start;
		auto diffTiempo = chrono::duration <double, milli> (diff).count();
		tiempos.push_back(diffTiempo);
		tiempoTotal += diffTiempo;
	}
	auto promedio = tiempoTotal / instancias.size();
	double sumaVarianza = 0;
	for(auto& valor : tiempos){
		sumaVarianza += pow((valor - promedio),2);
	}
	double varianza = sumaVarianza / instancias.size();
	ofs << instancias[0].getNodos().size() << "," << promedio << ":"<< varianza << endl;
}

void p_medirYPromediarResultadoGolosa(vector<S_CVRP> &instancias,ofstream& ofs){
	counter++;
	cout << "medir: "<< counter<< endl; 
	double costoTotal = 0;
	vector<double> costos = vector<double>();
	for(auto &instancia : instancias){
		p_solucion sol = p_golosa(instancia);
		costos.push_back(sol.costoTotal);
		costoTotal += sol.costoTotal;
	}
	auto promedio = costoTotal / instancias.size();
	double sumaVarianza = 0;
	for(auto& valor : costos){
		sumaVarianza += pow((valor - promedio),2);
	}
	double varianza = sumaVarianza / instancias.size();
	ofs << instancias[0].getNodos().size() << "," << promedio << ":"<< varianza << endl;
}

void p_medirResultadosGolosaConocido(pair<S_CVRP,double> &instancia,ofstream& ofs){
	p_solucion sol = p_golosa(instancia.first);
	double porcentajeAhorrado = (sol.costoTotal - instancia.second)*(100/ instancia.second);
	cout << "optima: " << instancia.second << endl;
	cout << "obtenida: "<< sol.costoTotal << endl; 
	ofs << instancia.first.getNodos().size() << "," << porcentajeAhorrado << ":" << endl;
}

void p_generarVectoresDeInstancias(int nStart, int nEnd, int numeroMediciones,ofstream& ofs1, void(*fMedicion)(vector<S_CVRP>&,ofstream&)){
	//geteo todas las instancias de a un n por vez(por si son demasiadas)
	for(int i = nStart; i <= nEnd; i++){
		vector<S_CVRP> instancias = vector<S_CVRP>();
		string i_string = to_string(i);
		for(int j = 1; j <= numeroMediciones; j++){
			string j_string = to_string(j);
			//Ruta a getear. Cambienla si necesitan. No la pase por parametro porque es bien fea
			string ruta = "instancias_generadas/" + i_string + "-elementos/LPS-n" + i_string + "-m" + j_string + ".vrp";
			ifstream stream(ruta, ios::in);
			if(stream.ios::good()){
				TspData archivo = cargarTSP(ruta);
				S_CVRP G = S_CVRP(archivo);
				instancias.push_back(G);	
			}
		}
		//Aca llamen a su funcion para hacer las mediciones (la mia es p_medirTiempos)
		if(!instancias.empty()) fMedicion(instancias, ofs1);
	}
}


void p_correrInstanciasConocidasDeAUnaG(int nStart, int nEnd,ofstream& ofs1, void(*fMedicion)(pair<S_CVRP,double>&,ofstream&)){

	//geteo todas las instancias de a un n por vez(por si son demasiadas)
	for(int i = nStart; i <= nEnd; i++){
		vector<pair<S_CVRP,double>> instancias = vector<pair<S_CVRP,double>>();
		string i_string = to_string(i);
		for(int j = 1; j <= 1; j++){
			string j_string = to_string(j);
			cout << i_string << endl;
			//Ruta a getear. Cambienla si necesitan. No la pase por parametro porque es bien fea
			string rutaVRP = "serieA/A-n" + i_string + ".vrp";
			string rutaSol = "serieA/A-n" + i_string + ".sol";
			ifstream stream(rutaVRP, ios::in);
			ifstream streamSol(rutaSol, ios::in);
			if(stream.ios::good()){
				TspData archivo = cargarTSP(rutaVRP);
				S_CVRP G = S_CVRP(archivo);
				cout << p_invarianteMatriz(G) << endl;
				string solString;
				getline(streamSol,solString,'\n');
				double sol = stod(solString);
				instancias.push_back(make_pair(G,sol));
			}
		}
		//Aca llamen a su funcion para hacer las mediciones (la mia es p_medirTiempos)
		if(!instancias.empty()) fMedicion(instancias[0], ofs1);//p_medirTiempos(instancias,cs0,cs1,cs2);
	}
}

int main(int argc, char** argv){

	//recibo ruta del archivo
	//string fileName(argv[1]);

	//parseo el archivo con tsplib.h y lo guardo.
	/*TspData archivo = cargarTSP(fileName);
    S_CVRP G = S_CVRP(archivo);
    p_golosa(G);*/
    /*ofstream Tgolosa("Tgolosa", ios::out);
    p_generarVectoresDeInstancias(3,1003,400,Tgolosa,p_medirYPromediarGolosa);*/
	ofstream Tgolosa("ResGolosaSerieA", ios::out);
    p_correrInstanciasConocidasDeAUnaG(20,80,Tgolosa,p_medirResultadosGolosaConocido);
	return 0;
}
