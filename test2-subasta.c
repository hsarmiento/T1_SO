#include "nSystem.h"
#include "subasta.h"

// Para poder usar rand
#include <stdlib.h>

nSem mutex;

// print_msg:
//   1   => con mensajes y delay
//   0   => sin mensajes, con delay
//   -1  => sin mensajes, sin delay

int aleatorio(Subasta s, int print_msg, char *nom, int oferta) {
  if (print_msg>=0) {
    nSetTaskName(nom);
    nWaitSem(mutex); 
    int delay= (rand()%2000)*3;
    nSignalSem(mutex);
    nSleep(delay);
  }
  if (print_msg>0)
    nPrintf("%s ofrece %d\n", nom, oferta);
  int ret= ofrecer(s, oferta);
  nPrintf("retorno aleat es %d\n",ret);
  if (print_msg>0) {
    if (ret)
      nPrintf("%s se adjudico un item a %d\n", nom, oferta);
    else
      nPrintf("%s fallo con su oferta de %d\n", nom, oferta);
  }
  return ret;
}

int oferente(Subasta s, int print_msg, char *nom, int oferta) {
  nSetTaskName(nom);
  if (print_msg>0)
    nPrintf("%s ofrece %d\n", nom, oferta);
  int ret= ofrecer(s, oferta);
  if (print_msg>0) {
    if (ret)
      nPrintf("%s se adjudico un item a %d\n", nom, oferta);
    else
      nPrintf("%s fallo con su oferta de %d\n", nom, oferta);
  }
  return ret;
}



int test1(int print_msg) {
  Subasta s= nuevaSubasta(4);
  nTask pimienta= nEmitTask(oferente, s, print_msg, "pimienta", 10);
  if (print_msg>=0)
    nSleep(1000);
  nTask merken= nEmitTask(oferente, s, print_msg, "merken", 1);
  if (print_msg>=0)
    nSleep(1000);
  nTask comino= nEmitTask(oferente, s, print_msg, "comino", 4);
  if (print_msg>=0)
    nSleep(1000);
  nTask eneldo= nEmitTask(oferente, s, print_msg, "eneldo", 5);
  if (print_msg>=0)
    nSleep(1000);
  
  nTask oregano= nEmitTask(oferente, s, print_msg, "oregano", 8);
  if (print_msg>=0)
    nSleep(1000);
  if (nWaitTask(merken))
    nFatalError("nMain", "merken debio perder con 1\n");
  int u;
  int recaud= adjudicar(s, &u);
  if (recaud!=27)
    nFatalError("test1", "La recaudacion debio ser 7 y no %d\n", recaud);
  if (u!=0)
    nFatalError("test1", "Quedaron %d unidades sin vender\n", u);
  if (!nWaitTask(pimienta))
    nFatalError("nMain", "pimienta debio ganar con 10\n");
  if (!nWaitTask(eneldo))
    nFatalError("nMain", "eneldo debio ganar con 5\n");
  if (!nWaitTask(oregano))
    nFatalError("nMain", "oregano debio ganar con 8\n");
  if (!nWaitTask(comino))
    nFatalError("nMain", "comino debio ganar con 4\n");
  
  if (print_msg>0)
    nPrintf("El monto recaudado es %d y quedaron %d unidades sin vender\n",
            recaud, u);
  return 0;
}

//random delay and size pool equal to 1
int test2(int print_msg) {
  Subasta s= nuevaSubasta(1);
  nTask claudits= nEmitTask(aleatorio, s, print_msg, "claudits", 300);
  nTask marta= nEmitTask(aleatorio, s, print_msg, "marta", 900);
  nTask perrini= nEmitTask(aleatorio, s, print_msg, "perrini", 1200);
  nTask alanezz= nEmitTask(aleatorio, s, print_msg, "alanezz", 600);
  if (nWaitTask(claudits))
    nFatalError("test1", "claudits debio perder con 300\n");
  if (nWaitTask(marta))
    nFatalError("test1", "marta debio perder con 900\n");
  if (nWaitTask(alanezz))
    nFatalError("test1", "alanezz debio perder con 300\n");
  
  int u;
  int recaud= adjudicar(s, &u);
  if (recaud!=1200)
    nFatalError("test1", "La recaudacion debio ser 1200 y no %d\n", recaud);
  if (!nWaitTask(perrini))
    nFatalError("nMain", "perrini debio ganar con 1200\n");
  if (print_msg>0)
    nPrintf("El monto recaudado es %d y quedaron %d unidades sin vender\n",
            recaud, u);
  return 0;
}

// just one offering and several empty slots
int test3(int print_msg) {
  Subasta s= nuevaSubasta(10);
  nTask tomas= nEmitTask(oferente, s, print_msg, "tomas", 10, -1);
  if (print_msg>=0)
    nSleep(3000);
  int u;
  int recaud= adjudicar(s, &u);
  if (recaud!=10)
    nFatalError("test3", "La recaudacion debio ser 10 y no %d\n", recaud);
  if (u!=9)
    nFatalError("test3", "Quedaron %d unidades sin vender\n", u);
  if (print_msg>0)
    nPrintf("El monto recaudado es %d y quedaron %d unidades sin vender\n",
            recaud, u);
  if (!nWaitTask(tomas))
    nFatalError("nMain", "tomas debio ganar con 10\n");
  return 0;
}

#define N 500
#define M 100000

int nMain() {
  mutex= nMakeSem(1);
  nPrintf("una sola subasta con tiempos deterministicos\n");
  test2(1);
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");


  nPrintf("una sola subasta con tiempos aleatorios y delay\n");
  test1(1);
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");


  nPrintf("una sola subasta con 1 oferente y varios slots vacios\n");
  test3(1);
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");

  nSetTimeSlice(1);
  nPrintf("%d subastas en paralelo\n", N);
  nTask *tasks1= nMalloc(M*sizeof(nTask));
  nTask *tasks2= nMalloc(M*sizeof(nTask));
  nTask *tasks3= nMalloc(M*sizeof(nTask));
  int k;
  for (k=1; k<N; k++) {
    tasks2[k]= nEmitTask(test2, 0);
    tasks1[k]= nEmitTask(test1, 0);
    tasks3[k]= nEmitTask(test3, 0);
  }
  tasks3[0]= nEmitTask(test3, 1);
  tasks1[0]= nEmitTask(test1, 1);
  tasks2[0]= nEmitTask(test2, 1);
  
  for (k=0; k<N; k++) {
    nWaitTask(tasks3[k]);
    nWaitTask(tasks1[k]);
    nWaitTask(tasks2[k]);    
  }
  nPrintf("test aprobado\n");
  nPrintf("-------------\n");
  nPrintf("%d subastas en paralelo\n", M*2);
  nPrintf("(tomo alrededor de un minuto en mi computador)\n");
  for (k=1; k<M; k++) {
    tasks1[k]= nEmitTask(test1, -1);
    tasks2[k]= nEmitTask(test2, -1);
  }
  tasks1[0]= nEmitTask(test1, 1);
  tasks2[0]= nEmitTask(test2, 1);
  nWaitTask(tasks1[0]);
  nWaitTask(tasks2[0]);
  nPrintf("Enterrando tareas.  Cada '.' son 30 tareas enterradas.\n");
  for (k=1; k<M; k++) {
    nWaitTask(tasks1[k]);
    nWaitTask(tasks2[k]);
    if (k%10==0) nPrintf(".");
  }
  nPrintf("\ntest aprobado\n");
  nPrintf(  "-------------\n");
  nPrintf("Felicitaciones: paso todos los tests\n");
  return 0;
}
