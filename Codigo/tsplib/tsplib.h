#ifndef TSPLIB_H
#define TSPLIB_H
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "cpptrim.h"
using namespace std;

class TspData{
private:
    int capacity;
    bool finalizado = false;
    vector<pair<pair<int, int>, pair<int, bool>>> puntos;
public:
    //Métodos que solo se usan al crear el objeto
    void inicializar(int dim){
        if(!finalizado){
            for(int a = 0; a < dim; ++a){
                puntos.push_back(make_pair(make_pair(-1, -1), make_pair(-1, false)));
            }
        }
    }
    void ponerCapacidad(int cap){
        if(!finalizado) capacity = cap;
    }
    void agregarPunto(int id, int x, int y){
        if(!finalizado){
            puntos[id].first.first = x;
            puntos[id].first.second = y;
        }
    }
    void agregarDemanda(int id, int demanda){
        if(!finalizado) puntos[id].second.first = demanda;
    }
    void agregarDeposito(int id){
        if(!finalizado) puntos[id].second.second = true;
    }
    void finalizar(){
        finalizado = true;
    }
    //Métodos realmente públicos ------------
    //Devuelve la cantidad de puntos en el archivo
    int dimension() const{
        return puntos.size();
    }
    //Devuelve la capacidad de los camiones en el archivo
    int capacidad(){
        return capacity;
    }
    //Devuelve las coordenadas del punto id
    pair<int, int> coords(int id) const{
        if(id > 0 && id <= dimension())
            return puntos[id-1].first;
        else{
            cout << "TSPLIB: Error, los índices deben estar entre 1 y " << dimension() << endl;
            exit(1);
        }
    }
    //Devuelve la demanda del punto id
    int demanda(int id) const{
        if(id > 0 && id <= dimension())
            return puntos[id-1].second.first;
        else{
            cout << "TSPLIB: Error, los índices deben estar entre 1 y " << dimension() << endl;
            exit(1);
        }
    }
    //Devuelve si el punto id es depósito o no
    bool esDeposito(int id) const{
        if(id > 0 && id <= dimension())
            return puntos[id-1].second.second;
        else{
            cout << "TSPLIB: Error, los índices deben estar entre 1 y " << dimension() << endl;
            exit(1);
        }
    }
};

//Carga un archivo y devuelve sus contenidos en un vector de strings
vector<string> tsp_loadFile(string filename){
    //Try to open file
    ifstream file(filename);
    vector<string> fileContents;
    string currentLine;
    
    //Check if the file exists
    if(!(bool) file){
        cout << "Error cargando el archivo "
             << filename << "." << endl;
        exit(1);
    }
    
    //Load the file
    if (file.is_open()){
        //Get each line of the file
        while(getline(file, currentLine)) {
            fileContents.push_back(currentLine);
        }
    }
    
    return fileContents;            
}

//Separa un string dado un caracter delimitador
vector<string> split(char delimiter, string original) {
    original = original + delimiter;
    vector<string> parameters;
    string current_token = "";
    for(int i = 0; i < original.length(); ++i){
        if(original[i] == delimiter && current_token != ""){
            parameters.push_back(current_token);
            current_token = "";
        }
        else{
            current_token += original[i];
        }
    }
    return parameters;
}

//Agarra las lineas de un archivo, extrae los puntos y sus demandas
//y devuelve una lista(tupla(tupla(x, y), tupla(demanda, esDeposito))).
//Además toma por referencia 'capacity' y devuelve este valor por ahí.
#include <algorithm>
TspData tsp_parseFile(vector<string> file){
    int seccion = 0;
    TspData res;
    for(string linea : file){
        trim(linea);
        replace( linea.begin(), linea.end(), '\t', ' ');
        if(linea.substr(0, 9) == "DIMENSION"){
            int dim = stoi(split(' ', linea)[2]);
            res.inicializar(dim);
            continue;
        }
        else if(linea.substr(0, 8) == "CAPACITY"){
            int cap = stoi(split(' ', linea)[2]);
            res.ponerCapacidad(cap);
            continue;
        }
        else if(linea == "NODE_COORD_SECTION"){
            seccion = 1;
            continue;
        }
        else if(linea == "DEMAND_SECTION"){
            seccion = 2;
            continue;
        }
        else if(linea == "DEPOT_SECTION"){
            seccion = 3;
            continue;
        }
        else if(linea == "EOF"){
            break;
        }
        vector<string> tokens;
        switch(seccion){
            case 1:
                tokens = split(' ', linea);
                res.agregarPunto(stoi(tokens[0])-1, stoi(tokens[1]), stoi(tokens[2]));
                break;
            case 2:
                tokens = split(' ', linea);
                res.agregarDemanda(stoi(tokens[0])-1, stoi(tokens[1]));
                break;
            case 3:
                if(linea != "-1") res.agregarDeposito(stoi(linea)-1);
                break;
        }
    }
    res.finalizar();
    return res;
}

//Combina las dos funciones anteriores en un wrapper bonito
TspData cargarTSP(string ruta){
    return tsp_parseFile(tsp_loadFile(ruta));
}
#endif
