#include "Aria.h"
#include <bits/stdc++.h>
#define PI 3.14159265
using namespace std;
vector<int> fx;
vector<int> fy;
double stx,sty,enx,eny;
int curi=0;

class Logger {
private:
  ArRobot *robot;
  ArTime lastLogTime;
  ArFunctorC<Logger> robotTaskFunc;
  void logTask();
public:
  Logger(ArRobot *r);
  ~Logger();
}; 

Logger::Logger(ArRobot *r) :
  robot(r),                              // store a pointer to the ArRobot object
  robotTaskFunc(this, &Logger::logTask)  // initialize the functor to be given added as an ArRobot 
                                         // user task with this instance and which method to  call
{
  // add our task functor to the robot object as a user task, 
  // to be invoked in every robot task cycle (approx. every 100ms):
  robot->addSensorInterpTask("Logger", 50, &robotTaskFunc);    
}

Logger::~Logger()
{
  // it is important to remove our task if this object is destroyed, otherwise 
  // ArRobot will hold an invalid ArFunctor pointer in its tasks list, resulting
  // in a crash when it tries to invoke it.
  robot->remSensorInterpTask(&robotTaskFunc);
}

// This is the method invoked as the user task
void Logger::logTask()
{
  if(lastLogTime.mSecSince() >= 1000)  // 1 second has passed since start or last log 
  {
     printf("%f %f\n", robot->getX(), robot->getY());
     
     lastLogTime.setToNow(); // reset timer
  }
}


int main(int argc, char **argv)
{
     ifstream myReadFile;
     fx.clear();
     fy.clear();
     //char *file = argv[1];
     myReadFile.open("Roompath.txt");
     if (myReadFile.is_open()) {
     	while (!myReadFile.eof()) {
		      int tx,ty;
        	myReadFile >> tx >> ty;
      		fx.push_back(ty*50);
      		fy.push_back(tx*50);
        }
    }
    int n = fx.size();
    for(int i=0;i<n;i++) {
        fy[i] *= -1;
     }
    myReadFile.close();
    for(int i=0;i<fx.size();i++)
	     cout<<fx[i]<<" "<<fy[i]<<"\n";
    stx=fx[0];sty=fy[0];
    enx=fx[fx.size()-1];eny=fy[fx.size()-1];
    cout<<stx<<" "<<sty<<" "<<enx<<" "<<eny;

    Aria::init();
  ArArgumentParser parser(&argc, argv);
  parser.loadDefaultArguments();
  ArRobot robot;
  ArAnalogGyro gyro(&robot);
  ArSonarDevice sonar;
  ArRobotConnector robotConnector(&parser, &robot);
  ArLaserConnector laserConnector(&parser, &robot, &robotConnector);


  // Connect to the robot, get some initial data from it such as type and name,
  // and then load parameter files for this robot.
  if(!robotConnector.connectRobot())
  {
    ArLog::log(ArLog::Terse, "gotoActionExample: Could not connect to the robot.");
    if(parser.checkHelpAndWarnUnparsed())
    {
        // -help not given
        Aria::logOptions();
        Aria::exit(1);
    }
  }

  if (!Aria::parseArgs() || !parser.checkHelpAndWarnUnparsed())
  {
    Aria::logOptions();
    Aria::exit(1);
  }

  ArLog::log(ArLog::Normal, "gotoActionExample: Connected to robot.");

  //robot.addRangeDevice(&sonar);
  robot.runAsync(true);

  // Make a key handler, so that escape will shut down the program
  // cleanly
 /* ArKeyHandler keyHandler;
  Aria::setKeyHandler(&keyHandler);
  robot.attachKeyHandler(&keyHandler);
  printf("You may press escape to exit\n");
*/
  // Collision avoidance actions at higher priority
    ArActionLimiterForwards limiterAction("speed limiter near", 300, 600, 250);
    ArActionLimiterForwards limiterFarAction("speed limiter far", 300, 1100, 400);
    ArActionLimiterTableSensor tableLimiterAction;
    robot.addAction(&tableLimiterAction, 100);
    robot.addAction(&limiterAction, 95);
    robot.addAction(&limiterFarAction, 90);

  // Goto action at lower priority
  ArActionGoto gotoPoseAction("goto", ArPose(stx, sty, 0), 600, 400, 40, 2);
  robot.addAction(&gotoPoseAction, 50);  

  // Stop action at lower priority, so the robot stops if it has no goal
  ArActionStop stopAction("stop");
  robot.addAction(&stopAction, 40);

  ArPose pos,pos1;
  pos.setPose(stx,sty,0);
    
  robot.lock();
  robot.moveTo (pos);       
  robot.unlock();
  // turn on the motors, turn off amigobot sounds
  robot.enableMotors();
  robot.comInt(ArCommands::SOUNDTOG, 0);

  bool first = true;
  ArTime start;
  start.setToNow();
  int flag1=1;
  curi=0;
  while (Aria::getRunning()) 
  {
    robot.lock();
    // achieved the previous goal.
    double cx,cy;
    cx=robot.getX();
    cy=robot.getY();
    // Find location of ghost robot
    int index=-1;
    double tmpx,tmpy;
    for(int i=curi;i<fx.size();i++){
    	index = i;
    	tmpx=fx[i];
    	tmpy=fy[i];	
    	if(sqrt(((cx-tmpx)*(cx-tmpx))+((cy-tmpy)*(cy-tmpy)))>=3000){
    		break;
    	}
    	// else
  		 //  break;
    } 
    cout<<"*"<<index<<" "<<curi<<"\n";
    if(index>(curi+2)){
    	index = curi + 1;
    }
    curi=index;
    if(index==fx.size()-1)
	     break;                    // if all distances are <600
    // if(index == -1)               
	   //   index = curi-1;
    // else
	   index--;
    cout<<"#"<<index<<" "<<curi<<"\n";
    tmpx=fx[index];
    tmpy=fy[index];	
	
    double Frx=0.0,Fry=0.0,Ffinal,thetaf,att=10.0;
    double distg = sqrt(((cx-tmpx)*(cx-tmpx))+((cy-tmpy)*(cy-tmpy)))/100;
    double thetag = ((atan2(tmpy-cy,tmpx-cx)*180/PI)-robot.getTh());   //alpha - theta
    
    Frx+=(att*cos(thetag*PI/180)*distg);
    Fry+=(att*sin(thetag*PI/180)*distg);
    Ffinal = sqrt((Frx*Frx)+(Fry*Fry));
    thetaf = atan2(Fry,Frx)*180/PI;
    double lvel,rotvel;
    lvel = abs(Ffinal);
    rotvel = thetaf*.5 ;

    if(distg>400 && distg<600)
	     lvel = 60;                      //check
    if(distg>=0 && distg<=200)
	     lvel = 30;
    if(distg>200 && distg <=400)
	     lvel = 40;

    if(rotvel>30)
	     rotvel = 30;
    if(rotvel<-30)
	      rotvel = -30;
      cout<<lvel<<" vel  "<<rotvel<<endl;
    robot.setVel(lvel);
    robot.setRotVel(rotvel);
    robot.unlock();
    ArUtil::sleep(150);
    
  }
  robot.unlock();
  
  while (Aria::getRunning()) 
  {
    robot.lock();
    // achieved the previous goal.
    double cx,cy;
    cx=robot.getX();
    cy=robot.getY();
    // Find location of ghost robot
    int index=-1;
    double tmpx,tmpy;
    tmpx=fx[fx.size()-1];
    tmpy=fy[fx.size()-1];
    cout<<"#"<<cx<<" "<<cy<<" "<<tmpx<<" "<<tmpy<<"\n";	
    if(sqrt(((cx-tmpx)*(cx-tmpx))+((cy-tmpy)*(cy-tmpy)))<250){
      cout<<"less"<<endl;
	     break;
    }
    double Frx=0,Fry=0,Ffinal,thetaf,att=10;
    double distg = sqrt(((cx-tmpx)*(cx-tmpx))+((cy-tmpy)*(cy-tmpy)))/100;
    double thetag = ((atan2(tmpy-cy,tmpx-cx)*180/PI)-robot.getTh());
    
    Frx+=(att*cos(thetag*PI/180)*distg);
    Fry+=(att*sin(thetag*PI/180)*distg);
    Ffinal = sqrt((Frx*Frx)+(Fry*Fry));
    thetaf = atan2(Fry,Frx)*180/PI;
    double lvel,rotvel;
    lvel = abs(Ffinal);
    rotvel = thetaf*.5 ;
    if(distg>600&&distg<800)
	lvel = 60;
    if(distg>0&&distg<=200)
	lvel = 20;
    if(distg>200&&distg<=600)
	lvel = 40;
    if(rotvel>30)
	rotvel = 30;
    if(rotvel<-30)
	rotvel = -30;
    robot.setVel(lvel);
    robot.setRotVel(rotvel);
    robot.unlock();
    ArUtil::sleep(150);
    
  }  
  // Robot disconnected or time elapsed, shut down
  Aria::exit(0);
  return 0;

}

