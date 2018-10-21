//-----------------------------------------------------------------------------
// SecondRealEA.cpp
//-----------------------------------------------------------------------------
//*
// Same code than FirstBitEA as far as Evolutionary Computation is concerned
// but now you learn to enter the parameters in a more flexible way
// (also slightly different than in SecondBitEA.cpp)
// and to twidle the output to your preferences (as in SecondBitEA.cpp)
//
//-----------------------------------------------------------------------------
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// standard includes
#include <fstream>
#include <iostream>   // cout
#include <stdexcept>  // runtime_error
#include <bits/stdc++.h>

// the general include for eo
#include <eo>
#include <es.h>
#define PI 3.14 
#define alpha 1000.0
// #define min_width 0
// #define min_height 0       // x
// #define max_width 539      // y , col
// #define max_height 471     // x , row
// #define dest_x  125     //300
// #define dest_y   150      //350  
// #define source_x 125   // row
// #define source_y 25 // col
#define n 5
// REPRESENTATION
//-----------------------------------------------------------------------------
// define your individuals
 double min_width = 0.0;
double max_width = 539.0;
double min_height= 0.0;
double max_height = 471.0;

 double dest_x = 425.0;//100.0     ;//300
double dest_y  = 425.0;//350.0    ;  //350  
double source_x= 225.0   ;// row
double source_y =250.0 ;//250.0 ;// col
typedef eoReal<eoMinimizingFitness> Indi;

// Use functions from namespace std
using namespace std;

// EVALFUNC
//-----------------------------------------------------------------------------
// a simple fitness function that computes the euclidian norm of a real vector
// Now in a separate file, and declared as binary_value(const vector<bool> &)
int map_coord[600][600] ;
struct node {
  double x , y;
};



double Distance(double x1, double y1, double x2, double y2)
{ 
  double diffx = (x1 - x2);
 double diffy = (y1 - y2);
  if(diffx < 0.0 ) {
    diffx = -1*diffx ;
  }
  if(diffy < 0.0 ) {
    diffy = -1*diffy ;
  }
  double diffx_sqr = pow(diffx,2);
  double diffy_sqr = pow(diffy,2);
  double distance = sqrt (diffx_sqr + diffy_sqr);

  return distance;
}

int obstacle(double x1, double y1, double x2, double y2) {
  int obs = 0;
   double DT = Distance(x1,y1,x2,y2);
   double D = 1.0; // distance to point C
   
      for(D = 2.0; D <= DT ; D+= 2.0){       
        double x;
        double y;
        double T = D / DT;
       
        // finding point C coordinate
        x = (1 - T) * x1 + T * x2;
        y = (1 - T) * y1 + T * y2;
       // cout<<"x "<<x<<" y "<<y<<endl;
         int i = round(x) ;
        int j = round(y) ;
        //cout<<i<<" "<<j<<endl;
        if(map_coord[i][j] == 1){
          obs++;
        
        }
              
      }
       
  return obs;
}
double fitness(const vector<double>& _indi)
{

  //spline conversion

  struct node points[n];
  double fitness_value = 0.0;
  int j = 0;

  for (unsigned i = 0; i < 9; i+=2){
    //sum += _chrom[i];
    
    points[j].x = _indi[i] ;
    points[j].y = _indi[i+1];
    //cout<<points[j].x<<" "<<points[j].y<<endl;
    j+=1;    
  }
  //cout<<j<<endl;

  fitness_value += Distance(source_x,source_y,points[0].x,points[0].y);
  fitness_value += Distance(points[n-1].x,points[n-1].y,dest_x,dest_y);
   int a = obstacle(points[4].x,points[4].y,dest_x,dest_y ) ;  
    fitness_value += alpha*obstacle(points[0].x,points[0].y,source_x,source_y )  ; 
  fitness_value += alpha* a ; 
  //cout<<points[n-1].x<<" "<<points[n-1].y<<" to "<<dest_x<<" "<<dest_y<<" obs = "<<a<<endl;

  for (unsigned i = 0; i < 5; i++) {
      // bound check
    double x = points[i].x ;
    double y = points[i].y ;

      if( x >= max_height || y >= max_width || x < min_height || y < min_width){
        //cout<<"x = "<<x<<" y "<<y<<endl;
        fitness_value =  10000000.0;
        break;
      }

      if(i > 0) {
        fitness_value += Distance(points[i-1].x,points[i-1].y,points[i].x,points[i].y);
        //cout<<"ft ="<<Distance(points[i].x,points[i].y,points[i-1].x,points[i-1].y)<<endl;
      
      fitness_value += alpha*obstacle(points[i-1].x,points[i-1].y,points[i].x,points[i].y )  ;  
    }
  }

  return  fitness_value;
}
void main_function(int argc, char **argv)
{
// PARAMETRES
//-----------------------------------------------------------------------------
// instead of having all values of useful parameters as constants, read them:
// either on the command line (--option=value or -o=value)
//     or in a parameter file (same syntax, order independent,
//                             # = usual comment character
//     or in the environment (TODO)
int c,d;
  freopen("Mymatrix.txt","r",stdin); // redirect input
  freopen("out.txt","w",stdout); // redirect input
  for(int i = 0 ; i < max_height; i++) {
    for(int j = 0 ; j < max_width; j++) {
        cin>>c;
        
        if(c > 0 ){
          map_coord[i][j] = 0 ;      // empty space

        }
        else 
        map_coord[i][j] = 1;
       
    }
    
  }
  // First define a parser from the command-line arguments
  eoParser parser(argc, argv);

  // For each parameter, you can in on single line
  // define the parameter, read it through the parser, and assign it

  unsigned seed = parser.createParam(unsigned(time(0)), "seed", "Random number seed", 'S').value(); // will be in default section General

  // description of genotype
  unsigned vecSize = parser.createParam(unsigned(5*2), "vecSize", "Genotype size",'V', "Representation" ).value();

   // parameters for evolution engine
    unsigned popSize = parser.createParam(unsigned(40), "popSize", "Population size",'P', "Evolution engine" ).value();

    unsigned tSize = parser.createParam(unsigned(2), "tSize", "Tournament size",'T', "Evolution Engine" ).value();

   // init and stop
    string loadName = parser.createParam(string(""), "Load","A save file to restart from",'L', "Persistence" ).value();

    unsigned maxGen = parser.createParam(unsigned(300), "maxGen", "Maximum number of generations",'G', "Stopping criterion" ).value();

    unsigned minGen = parser.createParam(unsigned(100), "minGen", "Minimum number of generations",'g', "Stopping criterion" ).value();

    unsigned steadyGen = parser.createParam(unsigned(10), "steadyGen", "Number of generations with no improvement",'s', "Stopping criterion" ).value();

   // operators probabilities at the algorithm level
    double pCross = parser.createParam(double(0.8), "pCross", "Probability of Crossover", 'C', "Genetic Operators" ).value();

    double pMut = parser.createParam(double(0.1), "pMut", "Probability of Mutation", 'M', "Genetic Operators" ).value();

   // relative rates for crossovers
    double hypercubeRate = parser.createParam(double(1), "hypercubeRate", "Relative rate for hypercube crossover", '\0', "Genetic Operators" ).value();

    double segmentRate = parser.createParam(double(1), "segmentRate", "Relative rate for segment crossover", '\0', "Genetic Operators" ).value();

    // internal parameters for the mutations
    double EPSILON = parser.createParam(double(0.5), "EPSILON", "Width for uniform mutation", '\0', "Genetic Operators" ).value();

    double SIGMA = parser.createParam(double(0.3), "SIGMA", "Sigma for normal mutation", '\0', "Genetic Operators" ).value();

   // relative rates for mutations
    double uniformMutRate = parser.createParam(double(1), "uniformMutRate", "Relative rate for uniform mutation", '\0', "Genetic Operators" ).value();

    double detMutRate = parser.createParam(double(1), "detMutRate", "Relative rate for det-uniform mutation", '\0', "Genetic Operators" ).value();

    double normalMutRate = parser.createParam(double(1), "normalMutRate", "Relative rate for normal mutation", '\0', "Genetic Operators" ).value();

    // the name of the "status" file where all actual parameter values will be saved
    string str_status = parser.ProgramName() + ".status"; // default value
    string statusName = parser.createParam(str_status, "status","Status file",'S', "Persistence" ).value();

   // do the following AFTER ALL PARAMETERS HAVE BEEN PROCESSED
   // i.e. in case you need parameters somewhere else, postpone these
    if (parser.userNeedsHelp())
      {
      	parser.printHelp(cout);
      	exit(1);
      }
    if (statusName != "")
      {
        	ofstream os(statusName.c_str());
        	os << parser;		// and you can use that file as parameter file
      }

// EVAL
  /////////////////////////////
  // Fitness function
  ////////////////////////////
  // Evaluation: from a plain C++ fn to an EvalFunc Object
     
  // you need to give the full description of the function
  eoEvalFuncPtr<Indi, double, const vector<double>& > plainEval(  fitness);
  // ... to an object that counts the nb of actual evaluations
  eoEvalFuncCounter<Indi> eval(plainEval);

// INIT
  ////////////////////////////////
  // Initilisation of population
  ////////////////////////////////
  // Either load or initialize
  // create an empty pop
  eoPop<Indi> pop;
  // create a state for reading
  eoState inState;		// a state for loading - WITHOUT the parser
  // register the rng and the pop in the state, so they can be loaded,
  // and the present run will be the exact conitnuation of the saved run
  // eventually with different parameters
  inState.registerObject(rng);
  inState.registerObject(pop);

  if (loadName != "")
    {
      inState.load(loadName); //  load the pop and the rng
      // the fitness is read in the file:
      // do only evaluate the pop if the fitness has changed
    }
  else
    {
      rng.reseed(seed);
      // a Indi random initializer
      // based on boolean_generator class (see utils/rnd_generator.h)
      // eoUniformGenerator<double> uGen(1.0, 470.0);
      // eoInitFixedLength<Indi> random(vecSize, uGen);

      // // Init pop from the randomizer: need to use the append function
      // pop.append(popSize, random);
      // // and evaluate pop (STL syntax)
      // apply<Indi>(eval, pop);
      int pcnt = 0;
 //      for (unsigned int igeno=0; igeno<popSize; igeno++)
      while(pcnt < popSize) 
  {
      Indi v;          // void individual, to be filled
      int cnt = 0;
      //for (unsigned ivar=0; ivar<vecSize/2; ivar++)
     
      while(cnt < 5){

        
         int x = rand()%(470)  +1  ;
        int y = rand()%(538) +1; 
      if(map_coord[x][y] == 0) {

              v.push_back(x);
           v.push_back(y);
          cnt++;
        }

    }
    //cout<<endl;
     // order(v);
     if(fitness(v) < 2000) {
        eval(v);                  // evaluate it
        pcnt++;
        pop.push_back(v);         // and put it in the population
      }
       
      
    }

    } // end of initializatio of the population

// OUTPUT
  // sort pop before printing it!
  pop.sort();
  // Print (sorted) intial population (raw printout)
  cout << "Initial Population\n" ;
  cout << pop<<"\n";

// ENGINE
  /////////////////////////////////////
  // selection and replacement
  ////////////////////////////////////
// SELECT
  // The robust tournament selection
  eoDetTournamentSelect<Indi> selectOne(tSize);
  // is now encapsulated in a eoSelectPerc (entage)
  eoSelectPerc<Indi> select(selectOne);// by default rate==1

// REPLACE
  // And we now have the full slection/replacement - though with
  // no replacement (== generational replacement) at the moment :-)
  eoGenerationalReplacement<Indi> replace;

// OPERATORS
  //////////////////////////////////////
  // The variation operators
  //////////////////////////////////////
// CROSSOVER
  // uniform chooce on segment made by the parents
  eoSegmentCrossover<Indi> xoverS;
  // uniform choice in hypercube built by the parents
  eoHypercubeCrossover<Indi> xoverA;
  // Combine them with relative weights
   eoPropCombinedQuadOp<Indi> xover(xoverS, segmentRate);
  // xover.add(xoverA, hypercubeRate, true);

// MUTATION
  // offspring(i) uniformly chosen in [parent(i)-epsilon, parent(i)+epsilon]
  eoUniformMutation<Indi>  mutationU(EPSILON);
  // k (=1) coordinates of parents are uniformly modified
  eoDetUniformMutation<Indi>  mutationD(EPSILON);
  // all coordinates of parents are normally modified (stDev SIGMA)
  eoNormalMutation<Indi>  mutationN(SIGMA);
  // Combine them with relative weights
  eoPropCombinedMonOp<Indi> mutation(mutationU, uniformMutRate);
  mutation.add(mutationD, detMutRate);
  mutation.add(mutationN, normalMutRate, true);

  // The operators are  encapsulated into an eoTRansform object
  eoSGATransform<Indi> transform(xover, pCross, mutation, pMut);

// STOP
  //////////////////////////////////////
  // termination condition see FirstBitEA.cpp
  /////////////////////////////////////
  eoGenContinue<Indi> genCont(maxGen);
  eoSteadyFitContinue<Indi> steadyCont(minGen, steadyGen);
  eoFitContinue<Indi> fitCont(0);
  eoCombinedContinue<Indi> continuator(genCont);
  continuator.add(steadyCont);
  continuator.add(fitCont);


// CHECKPOINT
  // but now you want to make many different things every generation
  // (e.g. statistics, plots, ...).
  // the class eoCheckPoint is dedicated to just that:

  // Declare a checkpoint (from a continuator: an eoCheckPoint
  // IS AN eoContinue and will be called in the loop of all algorithms)
  eoCheckPoint<Indi> checkpoint(continuator);

    // Create a counter parameter
    eoValueParam<unsigned> generationCounter(0, "Gen.");

    // Create an incrementor (sub-class of eoUpdater). Note that the
    // parameter's value is passed by reference,
    // so every time the incrementer is updated (every generation),
    // the data in generationCounter will change.
    eoIncrementor<unsigned> increment(generationCounter.value());

    // Add it to the checkpoint,
    // so the counter is updated (here, incremented) every generation
    checkpoint.add(increment);

    // now some statistics on the population:
    // Best fitness in population
    eoBestFitnessStat<Indi> bestStat;
    // Second moment stats: average and stdev
    eoSecondMomentStats<Indi> SecondStat;
  //eoBestFitnessStat<EoType>  best("Best_Fitness");
       // eoAverageStat<EoType> avg("Avg_Fitness");
    // Add them to the checkpoint to get them called at the appropriate time
    checkpoint.add(bestStat);
    checkpoint.add(SecondStat);

    // The Stdout monitor will print parameters to the screen ...
    eoStdoutMonitor monitor;

    // when called by the checkpoint (i.e. at every generation)
    checkpoint.add(monitor);

    // the monitor will output a series of parameters: add them
    monitor.add(generationCounter);
    monitor.add(eval);		// because now eval is an eoEvalFuncCounter!
    monitor.add(bestStat);
    monitor.add(SecondStat);

    // A file monitor: will print parameters to ... a File, yes, you got it!
    eoFileMonitor fileMonitor("stats.xg", " ");

    // the checkpoint mechanism can handle multiple monitors
    checkpoint.add(fileMonitor);

    // the fileMonitor can monitor parameters, too, but you must tell it!
    fileMonitor.add(generationCounter);
    fileMonitor.add(bestStat);
    fileMonitor.add(SecondStat);

    // Last type of item the eoCheckpoint can handle: state savers:
    eoState outState;
    // Register the algorithm into the state (so it has something to save!!)
    outState.registerObject(parser);
    outState.registerObject(pop);
    outState.registerObject(rng);

    // and feed the state to state savers
    // save state every 100th  generation
    eoCountedStateSaver stateSaver1(20, outState, "generation");
    // save state every 1 seconds
    eoTimedStateSaver   stateSaver2(1, outState, "time");

    // Don't forget to add the two savers to the checkpoint
    checkpoint.add(stateSaver1);
    checkpoint.add(stateSaver2);
    // and that's it for the (control and) output

// GENERATION
  /////////////////////////////////////////
  // the algorithm
  ////////////////////////////////////////

  // Easy EA requires
  // stopping criterion, eval, selection, transformation, replacement
  eoEasyEA<Indi> gga(checkpoint, eval, select, transform, replace);

  // Apply algo to pop - that's it!
  gga(pop);

// OUTPUT
  // Print (sorted) intial population
  pop.sort();
cout<<"is ? = "<<map_coord[1][385]<<endl;
    int a = Distance( 191.964, 2.35333  , dest_x,dest_y ) ;

cout<<" d = "<<a<<endl;
 a = Distance( 63.3512 ,294.842 ,191.964 ,2.35333 ) ;

cout<<" d = "<<a<<endl;
  cout << "FINAL Population\n" << pop << endl;
// GENERAL
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