//g++ -std=c++11 -pthread -o barberia_exe Barberia.cpp HoareMonitor.cpp Semaphore.cpp
#include <iostream>
#include <iomanip>
#include <random>
#include "HoareMonitor.h"

using namespace std ;
using namespace HM ;
const int CLIENTES = 5,
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

//---------------------------------------------------------------------------

void CortarPeloACliente(){
	chrono::milliseconds cortes( aleatorio<800,1000>() );
        cout << "El barbero comienza a cortarle el pelo: " << cortes.count() << " milisegundos." << endl;
        this_thread::sleep_for(cortes);
        cout << "El barbero ya ha terminado de cortarle el pelo al cliente" << endl << endl;
}

//---------------------------------------------------------------------------

void EsperarFueraBarberia(unsigned int i){
	chrono::milliseconds espera( aleatorio<20,200>() );
        cout << "El cliente "<< i << " sale fuera de la barberia " << espera.count() << " milisegundos." << endl;
        this_thread::sleep_for(espera);
        cout << "Cliente " << i << " termina de esperar y vuelve a entrar \n \n";
}

// *****************************************************************************
// clase para monitor Barberia, semántica SU

class Barberia : public HoareMonitor
{
   private:
   CondVar sala_espera,
           silla,
           barbero;            

   public:
   Barberia() ;			        // constructor
   void cortarPelo(unsigned int i);	//pertenece a la hebra cliente
   void siguienteCliente();		//pertenece a la hebra barbero
   void finCliente();			//pertenece a la hebra barbero
} ;
//---------------------------------------------------------------------------

Barberia::Barberia(){
   sala_espera=newCondVar();
   silla=newCondVar();
   barbero=newCondVar();
}

//---------------------------------------------------------------------------
void Barbero(MRef<Barberia> monitor){
   while(true){
        monitor->siguienteCliente();
        CortarPeloACliente();
        monitor->finCliente();
   }
}
//---------------------------------------------------------------------------

void Cliente(MRef<Barberia> monitor, unsigned int num_cliente){
   while(true){
        monitor->cortarPelo(num_cliente);
        EsperarFueraBarberia(num_cliente);
   }
}

//---------------------------------------------------------------------------

void Barberia::finCliente(){
   silla.signal();
}

//---------------------------------------------------------------------------

void Barberia::siguienteCliente(){
   if(sala_espera.empty())
     barbero.wait();

   sala_espera.signal();
}

//---------------------------------------------------------------------------

void Barberia::cortarPelo(unsigned int i){
   barbero.signal();
   sala_espera.wait();
   silla.wait();
}

//*****************************************************************************

int main(){
   cout << "SOLUCION AL PROBLEMA DE LA BARBERIA: \n \n";

   MRef<Barberia> monitor = Create<Barberia>();
   thread hebra_barbero(Barbero, monitor),
          hebras_clientes[CLIENTES];

   for(unsigned int i=0;i<CLIENTES;i++){
	hebras_clientes[i] = thread(Cliente, monitor, i);
   }

   hebra_barbero.detach();//join();
   for(unsigned int i=0;i<CLIENTES;i++){
	hebras_clientes[i].detach();//join();
   }
   chrono::seconds espera(DURACION);
   this_thread::sleep_for(espera);
   cout << "Solucion correcta.\n";

   return 0;

}
