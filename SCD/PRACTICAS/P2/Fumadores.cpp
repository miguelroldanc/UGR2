//g++ -std=c++11 -pthread -o fumadores_exe Fumadores.cpp HoareMonitor.cpp Semaphore.cpp
#include <iostream>
#include <iomanip>
#include <random>
#include "HoareMonitor.h"

using namespace std ;
using namespace HM ;

const int NUM_FUMADORES = 3,
	  DURACION = 10;

//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}
//----------------------------------------------------------------------

int producir(){
   chrono::milliseconds duracion_producir( aleatorio<20,200>());
   cout << "Estanquero" << " va a coger el ingrediente (" << duracion_producir.count() << " milisegundos)" << endl;
   this_thread::sleep_for( duracion_producir );
   int i = aleatorio<0,NUM_FUMADORES-1>();
   return i;
}
//----------------------------------------------------------------------

void Fumar(int i){
	chrono::milliseconds duracion_fumar( aleatorio<20,200>() );
        cout << "El fumador "<< i << " sale fuera del estanco a fumar " << duracion_fumar.count() << " milisegundos." << endl;
        this_thread::sleep_for(duracion_fumar);
        cout << "Fumador " << i << " termina de fumar y vuelve a entrar \n \n";
}
// *****************************************************************************
// clase para monitor Estanco, semántica SU

class Estanco : public HoareMonitor
{
   private:
   CondVar ingr_disp[NUM_FUMADORES],
	   mostr_vacio;
   int ingr;

   public:
   Estanco();					//constructor
   void ObtenerIngrediente(int i);		//pertenece a la hebra fumador
   void ponerIngrediente(int ingrediente);	//pertenece a la hebra estanquero
   void esperarRecogidaIngrediente();		//pertenece a la hebra estanquero
} ;
//---------------------------------------------------------------------------

Estanco::Estanco(){
   mostr_vacio = newCondVar();
   for(int i = 0;i < NUM_FUMADORES;i++){
	ingr_disp[i] = newCondVar();
   }
   ingr = -1;
}
//---------------------------------------------------------------------------

void Estanco::ObtenerIngrediente(int i){
   if(ingr != i)
	ingr_disp[i].wait();

   cout << "Fumador " << i << " retira su ingrediente\n";
   mostr_vacio.signal();
}
//---------------------------------------------------------------------------

void Estanco::ponerIngrediente(int ingrediente){
   ingr = ingrediente;
   ingr_disp[ingr].signal();
}
//---------------------------------------------------------------------------

void Estanco::esperarRecogidaIngrediente(){
   mostr_vacio.wait();
   ingr = -1;
}
//---------------------------------------------------------------------------

void Fumador(MRef<Estanco> monitor, int num_fumador){
   while(true){
        monitor->ObtenerIngrediente(num_fumador);
        Fumar(num_fumador);
   }
}
//---------------------------------------------------------------------------

void Estanquero(MRef<Estanco> monitor){
   int ingre;
   while(true){
	ingre = producir();
	monitor->ponerIngrediente(ingre);
	monitor->esperarRecogidaIngrediente();
   }
}
//*****************************************************************************

int main(){
   cout << "SOLUCION AL PROBLEMA DE LOS FUMADORES: \n \n";

   MRef<Estanco> monitor = Create<Estanco>();
   thread hebra_estanquero(Estanquero, monitor),
          hebras_fumadores[NUM_FUMADORES];
   for(int i=0;i<NUM_FUMADORES;i++){
	hebras_fumadores[i] = thread(Fumador, monitor, i);
   }

   hebra_estanquero.detach();//join();
   for(unsigned int i=0;i<NUM_FUMADORES;i++){
	hebras_fumadores[i].detach();//join();
   }

   chrono::seconds espera(DURACION);
   this_thread::sleep_for(espera);
   cout << "Solucion correcta.\n";

   return 0;

}
