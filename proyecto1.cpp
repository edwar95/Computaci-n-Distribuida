#include <string>
#include <fstream>
#include <iostream>
#include<stdlib.h>
#include<string.h>
#include<vector>
#include<sstream>
#include<map>
#include <algorithm> 
# include <omp.h>
#include<math.h>


using namespace std;


float T(string elementoA,string elementoB);
string jaccardTanimoto(vector<string> elementoA, vector<string> elementoB);


vector< vector<string> > cargarArchivo(){
	string linea;
	ifstream ficheroIn("ZINC_chemicals1.txt");
	vector< vector<string> > chemicals;	
	
	while(std::getline(ficheroIn, linea)){
	    std::stringstream   linestream(linea);
    	std::vector<string> valor;
    	valor.resize(4);
		linestream >> valor[0] >> valor[1]>>valor[2]>>valor[3];
		vector<string> elemento;
    	elemento.push_back(valor[1]);
    	elemento.push_back(valor[3]);
		chemicals.push_back(elemento);
	}
	
	return chemicals;
}


void  fill (vector< vector<string> > chemicals, vector<string> &listaFinal,int y, int x){
	int chemSize = chemicals.size();	
    int min = 0;
    int max;
    float raiz;
	
	if (y != 0){
        raiz = sqrt(y / float(x));
        min = int(round(chemSize - (raiz * chemSize)));
	}
	
	max = chemSize - 1;
    
    
	if (y + 1 != x){
        raiz = sqrt((x - y - 1) / float(x));
        max = int(round(chemSize - (raiz * chemSize)));
	}
	
	
	for (int i=0;i<chemicals.size();i++){
        for (int j=i+1;j<chemicals.size();j++){
            listaFinal.push_back(jaccardTanimoto(chemicals[i],chemicals[j]));						
		}		
	}
}


string jaccardTanimoto(vector<string> elementoA, vector<string> elementoB){
	float a=T(elementoA[1],elementoB[1]);
	return elementoA[0] + "\t" + elementoB[0] + "\t" + to_string(a); 
}


float T(string elementoA,string elementoB){
	map<char,int>elemento1;
	map<char,int>elemento2;
	map<char,int>elemento3;
	int valorA=0;
	int valorB=0;
	int valorC=0;
	float t=0;
	for(char& c : elementoA){
		if (c=='@'){
			elemento1[c]=1;
		}
		else if (elemento1.count(c)>0){
			int valor=elemento1[c];
			elemento1[c]=1+valor;
		}	
		else{
			elemento1[c]=1;	
		}	
	}
	
	for(char& c : elementoB){
		if (c=='@'){
			elemento2[c]=1;
		}
		else if (elemento2.count(c)>0){
			int valor=elemento2[c];
			elemento2[c]=1+valor;
		}	
		else{
			elemento2[c]=1;	
		}	
	}
	
	for(char & c : elementoA){
		if(elemento2.count(c)>0){
			int a=elemento1[c];
			int b=elemento2[c];
			int d=min(a,b);
			elemento3[c]=d;
		}
	}
	
	for(auto const &ent1 : elemento1) {
		valorA+=ent1.second;
	}	
	for(auto const &ent1 : elemento2) {
  		valorB+=ent1.second;
	}
	for(auto const &ent1 : elemento3) {
  		valorC+=ent1.second;
	}
	
	t=valorC/float(valorA+valorB-valorC);
	
	t = floor(t * 100 + 0.5) / 100;

    return t;
}

void imprimirArchivo(vector< vector <string> > listaFinal){
	ofstream out("salida.txt",ofstream::out);
	for(int i=0;i<listaFinal.size();i++){
		for(int j=0;j<listaFinal[i].size();j++){
			out<<listaFinal[i][j]<<"\n";
		}
	} 
}

int main(){
	int x=omp_get_max_threads();
	vector< vector<string> > listas; 
	listas.resize(x);
	
	#pragma omp parallel num_threads(x)
	{
		int i=omp_get_thread_num();
		fill(cargarArchivo(),listas[i],i,x);
	}
	imprimirArchivo(listas);
	

}

