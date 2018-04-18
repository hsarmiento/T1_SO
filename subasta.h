typedef struct subasta *Subasta;

Subasta nuevaSubasta(int unidades);
int ofrecer(Subasta s, double precio);
double adjudicar(Subasta s, int *punidades);
/*int *ordenarPool(int *pool)*/
double *iniciarPool(int n);
int posicionMenor(double *array, int n);
double sumarSubastas(double *data, int size);
void mostrarPool(double *pool,int size);

