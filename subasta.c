#include <nSystem.h>
#include "subasta.h"

#define TRUE 1
#define FALSE 0

struct subasta {
  nMonitor mon, mon2, mon3;
  int escribiendo;
  int insertados;
  int tam;
  double *pool;
  double menor_val;
  int adjudicado;
  int eliminados;
  int habilitado;
};


Subasta nuevaSubasta(int n){
	Subasta subastas = (Subasta)nMalloc(sizeof(*subastas));
	subastas->mon = nMakeMonitor();
	subastas->mon2 = nMakeMonitor();
	subastas->mon3 = nMakeMonitor();
	subastas->tam = n;
	subastas->pool = iniciarPool(n);
	subastas->menor_val = 0;
	subastas->adjudicado = FALSE;
	subastas->insertados = 0;
	subastas->escribiendo = FALSE;
	subastas->eliminados = 0;
	subastas->habilitado = FALSE;
	return subastas;
}

double adjudicar(Subasta s, int *prestantes){
	double suma;
	if(s->habilitado){
		nEnter(s->mon);
		while(s->escribiendo){
			nWait(s->mon);
		}
		s->escribiendo = FALSE;
		suma = sumarSubastas(s->pool,s->tam); 
		*prestantes = s->tam - s->insertados;
		nEnter(s->mon2);
		s->adjudicado = TRUE;
		nNotifyAll(s->mon2);
		nExit(s->mon2);
		nEnter(s->mon3);
		while(s->eliminados < s->insertados){
			nWait(s->mon3);
		}
		nExit(s->mon3);
		nNotifyAll(s->mon);
		s->escribiendo = TRUE;
		s->habilitado = FALSE;
		nExit(s->mon);		
		nDestroyMonitor(s->mon); 
		nDestroyMonitor(s->mon2);
		nDestroyMonitor(s->mon3);
	}else{
		suma = sumarSubastas(s->pool,s->tam); 
		*prestantes = s->tam - s->insertados;
	}
	nFree(s->pool);
	nFree(s);
	return suma;
}

int ofrecer(Subasta s, double precio){
	nEnter(s->mon);
	s->habilitado = TRUE;
	s->escribiendo = TRUE;
	int i = -1;
	int pos_menor;
	double menor;
	pos_menor = posicionMenor(s->pool,s->tam);
	menor = s->pool[pos_menor];
	if(precio > menor){
		if(s->insertados < s->tam){
			s->insertados++;
		}
		s->pool[pos_menor] = precio;
		i = pos_menor;
		pos_menor = posicionMenor(s->pool,s->tam);
		s->menor_val = s->pool[pos_menor];
	}
	nEnter(s->mon2);
	s->escribiendo = FALSE;
	nNotifyAll(s->mon);
	nExit(s->mon);
	nNotifyAll(s->mon2);
	while(i != -1 && precio == s->pool[i]){
		nWait(s->mon2);
		if(s->adjudicado && precio == s->pool[i]){
			nExit(s->mon2);
			nEnter(s->mon3);
			s->eliminados++;
			if(s->eliminados == s->insertados){
				s->adjudicado = FALSE;
				nNotifyAll(s->mon3);	
			}
			nExit(s->mon3);
			return TRUE;
		}
	}
	nExit(s->mon2);
	return FALSE;

}


double *iniciarPool(int n){
	double *p = (double*)nMalloc(sizeof(double)*n);
	int i;
	for(i = 0; i<n; i++){
		p[i] = 0;
	}
	return p;
}


int posicionMenor(double *array, int n)
{
	int i, location;
	double minimum = 9999999;
	for(i=0; i<n; i++){
		if(array[i] < minimum){
			minimum = array[i];
			location = i;
		}
	}
    return location; 
}

double sumarSubastas(double *data, int size){
	double suma = 0;
	int i;
	for(i=0; i<size;i++){
		suma = suma + data[i];
	}
	return suma;
}
