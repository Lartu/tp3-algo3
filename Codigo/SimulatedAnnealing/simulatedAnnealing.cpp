#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include "simulatedAnnealing.h"
//#include "../Cluster-First/clusterFirst.cpp"
#include "../estructuras.h"
//#include "../proxMinimo/proxMinimo.cpp"
#include <random>
#include <chrono>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;
int counter = 0;
//numeroRuta1 y numeroRuta2 son los subindices de las rutas
p_solucion p_exchange(p_solucion S, int numeroRuta1, int numeroRuta2, int i, int j, const S_CVRP &G){
	ruta& ruta1 = S.rutas[numeroRuta1];
	ruta& ruta2 = S.rutas[numeroRuta2];
	int& carga1 = S.cargas[numeroRuta1];
	int& carga2 = S.cargas[numeroRuta2];
	if(carga1 + G.getDemanda(ruta2[j]) - G.getDemanda(ruta1[i]) <= G.getCapacidad()){
		if(carga2 - G.getDemanda(ruta2[j]) + G.getDemanda(ruta1[i]) <= G.getCapacidad()){
			carga1 = carga1 + G.getDemanda(ruta2[j]) - G.getDemanda(ruta1[i]);
			carga2 = carga2 - G.getDemanda(ruta2[j]) + G.getDemanda(ruta1[i]);
			//Agarro las dos rutas a modificar
			//Busco las longitudes de las aristas que voy a borrar
			double longAnteriorElemRuta1 = G.distanciaEntre(ruta1[i - 1],ruta1[i]); 
			double longPosteriorElemRuta1 = G.distanciaEntre(ruta1[i],ruta1[i + 1]);
			//Primera parte del exchange
			int elemRuta1 = ruta1[i];
			ruta1[i] = ruta2[j];
			double nuevaLongAnteriorElemRuta1 = G.distanciaEntre(ruta1[i-1],ruta1[i]);
			double nuevaLongPosteriorElemRuta1 = G.distanciaEntre(ruta1[i],ruta1[i + 1]);
			//actualizo el costo
			double costo = S.costoTotal - longAnteriorElemRuta1 - longPosteriorElemRuta1 + nuevaLongAnteriorElemRuta1 + nuevaLongPosteriorElemRuta1;
			//Segunda parte del exchange
			//Busco las longitudes de las aristas que voy a borrar
			double longAnteriorElemRuta2 = G.distanciaEntre(ruta2[j - 1],ruta2[j]); 
			double longPosteriorElemRuta2 = G.distanciaEntre(ruta2[j],ruta2[j + 1]);
			//hago el exchange
			ruta2[j] = elemRuta1;
			double nuevalongAnteriorElemRuta2 = G.distanciaEntre(ruta2[j - 1],ruta2[j]); 
			double nuevalongPosteriorElemRuta2 = G.distanciaEntre(ruta2[j],ruta2[j + 1]);
			//actualizo el costo
			costo = costo - longAnteriorElemRuta2 - longPosteriorElemRuta2 + nuevalongAnteriorElemRuta2 + nuevalongPosteriorElemRuta2;
			//armo la solucion resultante
			S.costoTotal = costo;
			p_solucion resultado(S.rutas,S.cargas,S.costoTotal);
			return resultado;
		}
	}
	return S;
}

//0 = (0,1), 1 = (1,0)
p_solucion p_shift(p_solucion S,int numeroRutaIn, int numeroRutaOut,int i,const S_CVRP &G){
	//Defino la ruta en la que voy a insertar y la ruta en la que voy a eliminar
	ruta& rutaIn = S.rutas[numeroRutaIn];
	ruta& rutaOut = S.rutas[numeroRutaOut];
	//Reviso si es posible la inserción
	if(S.cargas[numeroRutaIn] + G.getDemanda(rutaOut[i]) <= G.getCapacidad()){
		//Recorro toda la rutaIn buscando el mejor lugar de insercion
		double menorCosto = numeric_limits<double>::max();
		int anterior = 0;
		for(int k = 0; k < rutaIn.size()-2; k++){
			if(G.distanciaEntre(rutaIn[k],rutaOut[i]) + G.distanciaEntre(rutaIn[k+1],rutaOut[i]) < menorCosto){
				menorCosto = G.distanciaEntre(rutaIn[k],rutaOut[i]) + G.distanciaEntre(rutaIn[k+1],rutaOut[i]) - G.distanciaEntre(rutaIn[k],rutaIn[k+1]);
				anterior = k;
			} 
		}
		//Inserto y calculo la diferencia en costoTotal
		double totalRemovido = G.distanciaEntre(rutaIn[anterior],rutaIn[anterior+1]) + G.distanciaEntre(rutaOut[i-1],rutaOut[i]) + G.distanciaEntre(rutaOut[i+1],rutaOut[i]);
		//Me fijo si hay una arista entre el anterior del nodo eliminado y el siguiente.
		double ejeAAgregarRutaOut = (rutaOut[i-1] == 0) && (rutaOut[i+1] == 0) ? 0 : G.distanciaEntre(rutaOut[i-1],rutaOut[i+1]);
		//Sumo los pesos de todas las aristas agregadas
		double totalAgregado = G.distanciaEntre(rutaIn[anterior],rutaOut[i]) + G.distanciaEntre(rutaIn[anterior+1],rutaOut[i]) +  ejeAAgregarRutaOut;
		S.costoTotal = S.costoTotal + totalAgregado - totalRemovido;
		//actualizo la carga de la ruta en la que inserte
		S.cargas[numeroRutaIn] += G.getDemanda(rutaOut[i]);
		S.cargas[numeroRutaOut] -= G.getDemanda(rutaOut[i]);
		//Inserto y borro en las rutas
		auto itIn = rutaIn.begin();
		itIn += anterior;
		itIn++;
		rutaIn.insert(itIn,rutaOut[i]);
		auto itOut = rutaOut.begin();
		itOut += i;
		rutaOut.erase(itOut);
		//Si la rutaOut quedó vacía, la elimino de rutas y de cargas.
		if(rutaOut.size() == 2){
			auto it = S.rutas.begin();
			it += numeroRutaOut;
			S.rutas.erase(it);
			auto itCargas = S.cargas.begin();
			itCargas += numeroRutaOut;
			S.cargas.erase(itCargas);
		}
	}
	//retorno la solucion
	return S;
}

p_solucion p_dameUnVecino(const p_solucion &S,S_CVRP &G){
	//decido aleatoriamente que operacion voy a hacer (0 = (0,1), 1 = (1,0), 2 = (1,1))
	int operacion = rand() % 3;
	//Elijo dos rutas distintas
	int n = S.rutas.size();
	
	int numeroRuta1 = rand() % n;
	int numeroRuta2;
	do{
		numeroRuta2 = rand() % n;
	}while(numeroRuta1 == numeroRuta2);
	//Elijo un elemento aleatorio de cada ruta
	//No pueden ser ni el primero ni el ultimo porque es el deposito
	int i = rand() % (S.rutas[numeroRuta1].size()-2) + 1;
	int j = rand() % (S.rutas[numeroRuta2].size()-2) + 1;
	if(operacion == 0){
		return p_shift(S,numeroRuta2,numeroRuta1,i,G);
	}else if(operacion == 1){
		return p_shift(S,numeroRuta1,numeroRuta2,j,G);
	}else if(operacion == 2){
		return p_exchange(S,numeroRuta1, numeroRuta2,i,j,G);
	}
}

//tambien paso lo que necesite para llamar a savings/la heuristica de santi y parametrizar algunas cosas
//como la temperatura inicial y eso.
p_solucion p_simulatedAnnealing(S_CVRP &G,double Ts,int n, int cs){
	//Genero la solucion canonica
	p_solucion S0 = p_solucion(G);	
	//Temperatura en la iteracion K
	double Ti = Ts;
	//Temperatura final en la que el sistema se congela
	double Tf = 1;
	//numero de iteracion
	int i = 0; 
	p_solucion Sb = S0;
	p_solucion actual = Sb;
	//Mientras el sistema no se congele..
	while(Ti > Tf && i <= n){
		//Si me quedo una sola ruta, termine
		if(actual.rutas.size() < 2) return Sb;
		//Agarro un vecino aleatorio del vecindario definido por 1-interchange
		p_solucion candidato = p_dameUnVecino(actual,G);	
		//resto los costos para conseguir mi delta
		float delta = candidato.costoTotal - actual.costoTotal;
		//genero tita con una uniforme 0,1
		std::default_random_engine generator;
  		std::uniform_real_distribution<double> distribution(0.0,1.0);
  		double tita = distribution(generator);
		if(delta <= 0 || (delta > 0 && exp(-delta/Ti) >= tita)){
			//cout << "\033[1;31mACEPTO\033[0m\n" << endl;
			actual = candidato;
			//cout << "CostoTotal: " << actual.costoTotal << endl;
			if(candidato.costoTotal < Sb.costoTotal) Sb = candidato;
		}else{
			//cout << "\033[1;31mNO\033[0m\n" << endl;
		}
		//Enfrio la temperatura con algun criterio
		//CS 2
		switch(cs){
			//lineal
			case 0:
				Ti = Ts - i*(Ts - Tf)/n;
				break;
			case 1:
				Ti = Ts * pow(Tf/Ts,(float)i/(float)n);
				break;
			//sigmoidea
			case 2:
				Ti = (((Ts-Tf)/(1+exp(0.3*(i - (float)n/(float)2)))) + Tf);
				break;
			//rara
			case 3:
				Ti = 0.5*(Ts-Tf) * (1- tanh(10*(float)i/(float)n - 5)) + Tf;
				break;
		}
		//cout << i << endl;
		i++;
	}
	//cout << Sb.costoTotal << endl;
	//cout << i << endl;
	Sb.imprimirRutas();
	return Sb;
}

void p_medirYPromediarSA(vector<S_CVRP> &instancias,double Ts, int n,int cs, ofstream& ofs){
	counter++;
	//cout << "medir: "<< counter<< endl; 
	double tiempoTotal = 0;
	for(auto &instancia : instancias){
		auto start = chrono::steady_clock::now();
		p_simulatedAnnealing(instancia,Ts,n,cs);
		auto end = chrono::steady_clock::now();
		auto diff = end - start;
		auto diffTiempo = chrono::duration <double, milli> (diff).count();
		tiempoTotal += diffTiempo;
	}
	auto promedio = tiempoTotal / instancias.size();
	ofs << /*instancias[0].getNodos().size()*/n << "," << promedio << endl;
}

void p_medirResultados(vector<S_CVRP> &instancias,double Ts, int n,int cs, ofstream& ofs){
	counter++;
	//cout << "medir: "<< counter<< endl; 
	double total = 0;
	vector<double> costos = vector<double>();
	for(auto &instancia : instancias){
		p_solucion sol = p_simulatedAnnealing(instancia,Ts,n,cs);
		total += sol.costoTotal;
		costos.push_back(total);
	}
	double promedio = total / instancias.size();
	double sumaVarianza = 0;
	for(auto& valor : costos){
		sumaVarianza += pow((valor - promedio),2);
	}
	double varianza = sumaVarianza / instancias.size();
	double varianzaMuestral = sumaVarianza / instancias.size() -1;
	ofs << instancias[0].getNodos().size() << "," << promedio <<":" << varianza << "@"<< varianzaMuestral << endl;
}

void p_promedioCanonicas(vector<S_CVRP> &instancias,ofstream& ofs){
	counter++;
	//cout << "medir: "<< counter<< endl; 
	double total = 0;
	vector<double> costos = vector<double>();
	for(auto &instancia : instancias){
		p_solucion sol(instancia);
		total += sol.costoTotal;
		costos.push_back(total);
	}
	double promedio = total / instancias.size();
	double sumaVarianza = 0;
	for(auto& valor : costos){
		sumaVarianza += pow((valor - promedio),2);
	}
	double varianza = sumaVarianza / instancias.size();
	double varianzaMuestral = sumaVarianza / instancias.size() -1;
	ofs << instancias[0].getNodos().size() << "," << promedio <<"-" << varianza << "@"<< varianzaMuestral << endl;
}


void p_medirTiemposVariandoCS(vector<S_CVRP> &instancias, ofstream &cs0, ofstream &cs1,ofstream &cs2){
	p_medirYPromediarSA(instancias,10,1000,0,cs0);
	p_medirYPromediarSA(instancias,10,1000,1,cs1);
	p_medirYPromediarSA(instancias,10,1000,2,cs2);
}

void p_medirTiemposVariandoNIt(vector<S_CVRP> &instancias, ofstream &minimoIt, ofstream &medioIt,ofstream &maximoIt){
	for(int i = 0; i <= 5000; i+= 500){
		p_medirYPromediarSA(instancias,10,i,0,minimoIt);	
	}

	//p_medirYPromediarSA(instancias,10,1000,0,medioIt);
	//p_medirYPromediarSA(instancias,10,10000,0,maximoIt);
}

void p_medirResultadosVariandoCS(vector<S_CVRP> &instancias, ofstream &cs0, ofstream &cs1,ofstream &cs2){
	p_medirResultados(instancias,10,10000,0,cs0);
	p_medirResultados(instancias,10,10000,1,cs1);
	p_medirResultados(instancias,10,10000,2,cs2);
}

void p_experimentoResVariandoNIt(vector<S_CVRP> &instancias, ofstream &cienIt, ofstream &tresCienIt,ofstream &cincCienIt){
	for(int i = 1000; i <= 17000; i+=2000){
		p_medirResultados(instancias,100,i,1,cienIt);
	}
}

void p_promedioCanonicasWrapper(vector<S_CVRP> &instancias, ofstream &ofs1, ofstream &ofs2,ofstream &ofs3){
	p_promedioCanonicas(instancias,ofs1);
}

void p_medirAhorroPorcentualTs(vector<S_CVRP> &instancias,double Ts, int n,int cs, ofstream& ofs){
	counter++;
	//cout << "medir: "<< counter<< endl; 
	double total = 0;
	double totalCanonico = 0;
	vector<double> costos = vector<double>();
	for(auto &instancia : instancias){
		p_solucion solCanonica(instancia);
		totalCanonico += solCanonica.costoTotal;
		p_solucion solSA = p_simulatedAnnealing(instancia,Ts,n,cs);
		total += solSA.costoTotal;
		costos.push_back(total);
	}
	double promedioSA = total / instancias.size();
	double promedioCanonicas = totalCanonico / instancias.size();
	double sumaVarianza = 0;
	for(auto& valor : costos){
		sumaVarianza += pow((valor - promedioSA),2);
	}
	double varianza = sumaVarianza / instancias.size();
	double varianzaMuestral = sumaVarianza / instancias.size() -1;
	double porcentajeAhorro = (1 - (promedioSA / promedioCanonicas))*100;
	ofs << Ts << "," << porcentajeAhorro <<":"<<varianza << "@"<< varianzaMuestral << endl;
}

void p_medirResultadosVariandoTs(vector<S_CVRP> &instancias, ofstream &cs0, ofstream &cs1,ofstream &cs2){
	for(int i = 2; i <= 9; i++){
		p_medirAhorroPorcentualTs(instancias,i,10000,0,cs0);
		p_medirAhorroPorcentualTs(instancias,i,10000,1,cs1);
		p_medirAhorroPorcentualTs(instancias,i,10000,2,cs2);
	}
	for(int i = 10; i<= 200; i+= 10){
		p_medirAhorroPorcentualTs(instancias,i,10000,0,cs0);
		p_medirAhorroPorcentualTs(instancias,i,10000,1,cs1);
		p_medirAhorroPorcentualTs(instancias,i,10000,2,cs2);
	}
}

double p_medirPorcentajeAlOpt(pair<S_CVRP,double> &instancia,double Ts, int n,int cs, ofstream& ofs){
	//counter++;
	//cout << "medir: "<< counter<< endl; 
		p_solucion solCanonica(instancia.first);
		double costoCanonico = solCanonica.costoTotal;
		p_solucion solSA = p_simulatedAnnealing(instancia.first,Ts,n,cs);
		double porcentajeAhorrado = (solSA.costoTotal - instancia.second)*(100/ instancia.second);
		ofs << instancia.first.getNodos().size() << "," << porcentajeAhorrado<< ":" << endl;
		return porcentajeAhorrado;
		//ofs << instancia.first.getNodos().size() << "," << porcentajeAhorrado<< ":" << endl;
}

void p_medirPorcentajeAlOptWrapper(pair<S_CVRP,double> &instancia, ofstream& ofs){
	p_medirPorcentajeAlOpt(instancia,2,15000,1,ofs);
}

double p_medirConocidasNIt(pair<S_CVRP,double> &instancia,double Ts, int n,int cs, ofstream& ofs){
	counter++;
	if(counter == 1000){
		//cout << "medir: "<< counter<< endl;
		counter = 0;
	}

		p_solucion solCanonica(instancia.first);
		double costoCanonico = solCanonica.costoTotal;
		p_solucion solSA = p_simulatedAnnealing(instancia.first,Ts,n,cs);
		return (1 - (solSA.costoTotal/costoCanonico)) * 100;
		//ofs << instancias[0].first.getNodos().size() << "@" << Ts << "{}" << n << "," << (1 - (solSA.costoTotal/costoCanonico)) * 100<< ":" << endl;
}

//LO CORRO CON UN SOLO CS PORQUE LA EXPERIMENTACION PREVIA DETERMINO QUE EL 
//VALOR OPTIMO PARA EL Nit se respeta para los 3 CS en la gran gran mayoria
//de los casos. salvo en algunas excepciones, pero la diferencia no fue
//tampoco tan significativa.
void p_parametrosOptimosConocidas(vector<pair<S_CVRP,double>> &instancias, ofstream& ofs){
	double minimo = numeric_limits<double>::max();
	int maxNIt = 0;
	int maxTs = 0;
	int maxCS = -1;
	for(int k = 0; k <= 2; k++){
		for(int i = 1000; i <= 15000; i+= 2000){
			//cout << "CS:" << k << ", " << "NIt: "<<i << endl;
			for(int j = 2; j <= 9; j++){
				double total = 0;
				for(int w = 0; w < instancias.size();w++){
					double actual = p_medirPorcentajeAlOpt(instancias[w],j,i,k,ofs);
					total += actual;
				}
				if(total < minimo){
					minimo = total;
					maxNIt = i;
					maxTs = j;
					maxCS = k;
				}		
			}
			for(int j = 10; j <= 100; j+=10){
				double total = 0;	
				for(int w = 0; w < instancias.size(); w++){
					double actual = p_medirPorcentajeAlOpt(instancias[w],j,i,k,ofs);
					total += actual;
				}
				if(total < minimo){
					minimo = total;
					maxNIt = i;
					maxTs = j;
					maxCS = k;
				}	
			}
		}
	}
	ofs << "CS" <<maxCS<<" NIt: " << maxNIt << " Ts: " << maxTs << endl;
}

void p_generarVectoresDeInstancias(int nStart, int nEnd, int numeroMediciones,ofstream& ofs1,ofstream& ofs2,ofstream& ofs3, void(*fMedicion)(vector<S_CVRP>&,ofstream&,ofstream&,ofstream&)){

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
		if(!instancias.empty()) fMedicion(instancias, ofs1,ofs2,ofs3);//p_medirTiempos(instancias,cs0,cs1,cs2);
	}
}

void p_correrInstanciasConocidasDeAUna(int nStart, int nEnd,ofstream& ofs1, void(*fMedicion)(pair<S_CVRP,double>&,ofstream&)){

	//geteo todas las instancias de a un n por vez(por si son demasiadas)
	for(int i = nStart; i <= nEnd; i++){
		vector<pair<S_CVRP,double>> instancias = vector<pair<S_CVRP,double>>();
		string i_string = to_string(i);
		for(int j = 1; j <= 1; j++){
			string j_string = to_string(j);
			//cout << i_string << endl;
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
		if(!instancias.empty()) fMedicion(instancias[0], ofs1);//p_medirTiempos(instancias,cs0,cs1,cs2);
	}
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
	if(!instancias.empty()) fMedicion(instancias, ofs1);//p_medirTiempos(instancias,cs0,cs1,cs2);
}

int main(int argc, char** argv){
	if(argc < 5){
		cout << "Uso: param1: ruta archivo.vrp, param2: Ts, param3: NIt, param4: CS" << endl;
	}
	//recibo ruta del archivo
	string fileName(argv[1]);
	TspData archivo = cargarTSP(fileName);
    S_CVRP G = S_CVRP(archivo);
    int cs = stoi(argv[4]);
    if(cs < 0 || cs >= 3 ){
    	cout << "Cooling Schedule debe estar entre 0 y 2" << endl;
    	return 0;
    }
	p_simulatedAnnealing(G,stod(argv[2]),stoi(argv[3]),stoi(argv[4]));
	return 0;
}







