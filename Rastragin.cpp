//-----------------------------------------------------------------------------
// FirstRealGA.cpp
//-----------------------------------------------------------------------------
//*
// An instance of a VERY simple Real-coded Genetic Algorithm
//
//-----------------------------------------------------------------------------
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <eo>
#include <es.h>

// Use functions from namespace std
using namespace std;

// REPRESENTATION
//-----------------------------------------------------------------------------
// define your individuals
 typedef eoReal<double> Indi;
 #define PI 3.14 
 fstream outfile;
    


// EVAL
//-----------------------------------------------------------------------------
// a simple fitness function that computes the euclidian norm of a real vector
//    @param _indi A real-valued individual vector
double rastrigin(const Indi & _indi)
{
  double f = 20;
  for (unsigned i = 0; i < _indi.size(); i++)
    f += -10*(cos(2*PI*_indi[i])) + _indi[i]*_indi[i] ;
  outfile<<fixed<<_indi[0]<<"   "<<fixed<<_indi[1]<<"   "<<fixed<<f<<endl;

  return -f;
  
}

// GENERAL
//-----------------------------------------------------------------------------
void main_function(int argc, char **argv)
{
// PARAMETRES
  // all parameters are hard-coded!
  const unsigned int SEED = 6; // seed for random number generator
  const unsigned int VEC_SIZE = 2; // Number of object variables in genotypes
  const unsigned int POP_SIZE = 50; // Size of population
  const unsigned int T_SIZE = 3; // size for tournament selection
  const unsigned int MAX_GEN = 500; // Maximum number of generation before STOP
  const float CROSS_RATE = 0.8; // Crossover rate
  const double EPSILON = 0.1;  // range for real uniform mutation
  const float MUT_RATE = 0.02;   // mutation rate
  outfile.open("rastrigin.txt",fstream::out | fstream::trunc);
  outfile.precision(12);
    
    
// GENERAL
  //////////////////////////
  //  Random seed
  //////////////////////////
  //reproducible random seed: if you don't change SEED above,
  // you'll aways get the same result, NOT a random run
  rng.reseed(SEED);

// EVAL
  /////////////////////////////
  // Fitness function
  ////////////////////////////
  // Evaluation: from a plain C++ fn to an EvalFunc Object
  eoEvalFuncPtr<Indi> eval(  rastrigin );

// INIT
  ////////////////////////////////
  // Initilisation of population
  ////////////////////////////////

  // declare the population
  eoPop<Indi> pop;
  // fill it!
  for (unsigned int igeno=0; igeno<POP_SIZE; igeno++)
    {
      Indi v;          // void individual, to be filled
      for (unsigned ivar=0; ivar<VEC_SIZE; ivar++)
	{
	  double r = 10.24*rng.uniform() - 5.12; // new value, random in [-5.12,5.12)
	  v.push_back(r);       // append that random value to v
	}
      eval(v);                  // evaluate it
      pop.push_back(v);         // and put it in the population
    }

// OUTPUT
  // sort pop before printing it!
  pop.sort();
  // Print (sorted) intial population (raw printout)
  cout << "Initial Population" << endl;
  cout << pop;

// ENGINE
  /////////////////////////////////////
  // selection and replacement
  ////////////////////////////////////
// SELECT
  // The robust tournament selection
  eoDetTournamentSelect<Indi> select(T_SIZE);       // T_SIZE in [2,POP_SIZE]

// REPLACE
  // eoSGA uses generational replacement by default
  // so no replacement procedure has to be given

// OPERATORS
  //////////////////////////////////////
  // The variation operators
  //////////////////////////////////////
// CROSSOVER
  // offspring(i) is a linear combination of parent(i)
  eoSegmentCrossover<Indi> xover;
// MUTATION
  // offspring(i) uniformly chosen in [parent(i)-epsilon, parent(i)+epsilon]
  eoUniformMutation<Indi>  mutation(EPSILON);

// STOP
// CHECKPOINT
  //////////////////////////////////////
  // termination condition
  /////////////////////////////////////
  // stop after MAX_GEN generations
  eoGenContinue<Indi> continuator(MAX_GEN);

// GENERATION
  /////////////////////////////////////////
  // the algorithm
  ////////////////////////////////////////
  // standard Generational GA requires
  // selection, evaluation, crossover and mutation, stopping criterion


  eoSGA<Indi> gga(select, xover, CROSS_RATE, mutation, MUT_RATE,
		   eval, continuator);

  // Apply algo to pop - that's it!
  gga(pop);
outfile.close();
// OUTPUT
  // Print (sorted) intial population
  pop.sort();
  //cout<<pop[1][0]<<"\n";

   cout << "FINAL Population\n" << pop << endl;
  
}

// A main that catches the exceptions

int main(int argc, char **argv)
{
    try
    {
	main_function(argc, argv);
    }
    catch(exception& e)
    {
	cout << "Exception: " << e.what() << '\n';
    }

    return 1;
}
