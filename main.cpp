#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>  // for std::sort, std::next_permutation
#include <boost/filesystem.hpp>
#include "globalDefs.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>
  #include <SDL.h>
  #include <sys/stat.h>
  #undef main
#else
  #include <GL/gl.h>
  #include <GL/glut.h>
  #include <SDL/SDL.h>
#ifdef WIN32
  #include <direct.h>
#else
  #include <sys/stat.h>
#endif
#endif

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include "arc-ball/ArcBall.hpp"

#include "mainmouse.cpp"
#include "maintouch.cpp"


std::vector<int> sequenceOrder ;
std::vector<glm::mat4> targetMatrices ;
std::vector <std::tuple<int,glm::mat4>> trialTargets ;
string path ;

using namespace std;

struct compareFirstTupleMember {
    bool operator()(const tuple<int, glm::mat4>& a, const tuple<int, glm::mat4>& b) const {
        return get<0>(a) < get<0>(b);
    }
};



int createDirectory(){
    char *pathChar = new char[path.length() + 1];
    strcpy(pathChar, path.c_str());
    if(boost::filesystem::exists(path)){
        cerr << "DIRECTORY ALREADY EXIST, please check the tester ID\n";
        return -1 ;
    }
    boost::filesystem::create_directory(boost::filesystem::path(path));
    //Creating main directory for current subject
    //mkdir(pathChar,S_IRWXU);

    return 0;
}

int createConditionDirectory(string dirToCreate){
    if(boost::filesystem::exists(dirToCreate)){
        cerr << "SUB-DIRECTORY ALREADY EXIST, please check the tester ID\n";
        return -1 ;
    }
    boost::filesystem::create_directory(boost::filesystem::path(dirToCreate));
}

void createSubjectFileInfo(char* subID, std::vector<int> order){
    ofstream outfile (path+"/info.txt");
    outfile << "Subject ID: " << subID << endl ;
    outfile << "Order: " << order[0] << " - " << order[1] << " - " << order[2] << std::endl ;
}

vector<int> getPermutation(char* argv[]){
    int subjectID = atoi(argv[1]);//TODO
    std::vector<int> cond = {1,2,3};

    std::sort(cond.begin(), cond.end()); // "cond" est déjà trié, mais au cas où

    //do {
    //    std::cout << cond[0] << " " << cond[1] << " " << cond[2] << '\n';
    //}   while (std::next_permutation(cond.begin(), cond.end()));

    for(int i=0;i<subjectID;i++){
        //std::cout << cond[0] << " " << cond[1] << " " << cond[2] << '\n';
        std::next_permutation(cond.begin(), cond.end());
    }

    std::cout << cond[0] << " " << cond[1] << " " << cond[2] << '\n';

    return cond ;
}

void getPermutation(char* argv[], int condition){
    /*int subjectID = atoi(argv[1]);

    sort(trialTargets.begin(), trialTargets.end(), compareFirstTupleMember());

    for(int i=0;i<subjectID*condition;i++){
        //std::cout << cond[0] << " " << cond[1] << " " << cond[2] << '\n';
        std::next_permutation(begin(trialTargets), end(trialTargets), compareFirstTupleMember());
    }
    
    for(int i=0;i<NBOFTRIALS;i++){
        std::cout << get<0>(trialTargets[i]) << " ; " ;
    }
    cout << endl ;*/
    tuple<int,glm::mat4> tmp ;
    tmp = trialTargets[0];
    trialTargets[0] = trialTargets[NBOFTRIALS-1];
    trialTargets[NBOFTRIALS-1] = tmp ;

    tmp = trialTargets[3];
    trialTargets[3] = trialTargets[NBOFTRIALS-3];
    trialTargets[NBOFTRIALS-3] = tmp ;

    tmp = trialTargets[5];
    trialTargets[5] = trialTargets[NBOFTRIALS-5];
    trialTargets[NBOFTRIALS-5] = tmp ;

    for(int i=0;i<NBOFTRIALS;i++){
        std::cout << get<0>(trialTargets[i]) << " ; " ;
    }
    cout << endl ;

}


void loadTargets(){
    trialTargets.push_back(tuple<int,glm::mat4>(1,glm::mat4(0.993176, -0.075523, 0.088865, 0.000000, 0.082323, 0.993740, -0.075523, 0.000000, -0.082605, 0.082323, 0.993176, 0.000000, -0.999984, -0.736924, 0.511211, 1.000000)));
    trialTargets.push_back(tuple<int,glm::mat4>(2,glm::mat4(0.416407, 0.869553, 0.265486, 0.000000, -0.492963, -0.029419, 0.869553, 0.000000, 0.763933, -0.492963, 0.416407, 0.000000, -0.905911, 0.357729, 0.358593, 1.)));
    trialTargets.push_back(tuple<int,glm::mat4>(3,glm::mat4(0.996476, 0.062779, -0.055632, 0.000000, -0.059261, 0.996266, 0.062779, 0.000000, 0.059366, -0.059261, 0.996476, 0.000000, 0.661931, -0.930856, -0.893077, 1.000000)));
    trialTargets.push_back(tuple<int,glm::mat4>(4,glm::mat4(0.866834, 0.463736, -0.183158, 0.000000, -0.339754, 0.818240, 0.463736, 0.000000, 0.364919, -0.339754, 0.866834, 0.000000, -0.233169, -0.866316, -0.165028, 1.000000)));
    trialTargets.push_back(tuple<int,glm::mat4>(5,glm::mat4(1.0f)));
    trialTargets.push_back(tuple<int,glm::mat4>(6,glm::mat4(1.0f)));
    trialTargets.push_back(tuple<int,glm::mat4>(7,glm::mat4(1.0f)));
    trialTargets.push_back(tuple<int,glm::mat4>(8,glm::mat4(1.0f)));
    trialTargets.push_back(tuple<int,glm::mat4>(9,glm::mat4(1.0f)));
    trialTargets.push_back(tuple<int,glm::mat4>(10,glm::mat4(1.0f)));
    trialTargets.push_back(tuple<int,glm::mat4>(11,glm::mat4(1.0f)));
    trialTargets.push_back(tuple<int,glm::mat4>(12,glm::mat4(1.0f)));
    trialTargets.push_back(tuple<int,glm::mat4>(13,glm::mat4(1.0f)));
    trialTargets.push_back(tuple<int,glm::mat4>(14,glm::mat4(1.0f)));
    trialTargets.push_back(tuple<int,glm::mat4>(15,glm::mat4(1.0f)));
}

void launchCondition(int ind, int argc, char * argv[]){
	switch(ind){
		case 1:
            createConditionDirectory(path+MOUSE);
            getPermutation(argv,2);
			mainmouse::launchMouseExp(argc, argv, trialTargets, path+MOUSE);
			break;
		case 2:
            createConditionDirectory(path+TOUCH);
            getPermutation(argv,2);
			maintouch::launchTouchExp(argc, argv, trialTargets, path+TOUCH);
			break ;
		case 3:
            cout << "TANGIBLE CONDITION" << endl ;
			createConditionDirectory(path+TANGIBLE);
            //
			break ;
		default:
			cerr << "Error, not a valid condition number" << std::endl ;
			break ;
	}
}

int getNextTrialToDo(string path){
    int nbOfFiles = 0 ;
    vector<int> trialsDone ;
    for(boost::filesystem::directory_iterator it(path); it != boost::filesystem::directory_iterator(); ++it){
        if(boost::filesystem::is_regular_file(it->status())){
            cout << it->path().stem() << endl ;
            trialsDone.push_back(atoi(it->path().stem().c_str()));
            nbOfFiles++ ;
        }
    }
    for(std::vector<string>::size_type i = 0 ; i!=trialsDone.size(); i++){
        cout << "File = " << trialsDone[i] << endl ;
    }
    if(NBOFTRIALS == nbOfFiles){
        return -1 ;
    }
    else{
        return nbOfFiles ;
    }
}


int main(int argc, char *argv[])
{

    short a ;

	if(argc < 2){
        cerr << "Please enter subject ID!!\n" ;
        return -1 ;
    }
    path = "./results/"+string(argv[1]) ;
    sequenceOrder = getPermutation(argv);
    
    if(createDirectory()!=-1){
    	createSubjectFileInfo(argv[1], sequenceOrder);
        loadTargets();
        launchCondition(sequenceOrder[0], argc, argv);
        cout << "********************************************************************************************************************************" << endl 
             << "This part of the experiment is over" << endl << "Please fill in the questionnaire and press enter" << endl 
             << "********************************************************************************************************************************" << endl ;
        /*cin >> a ;*/

        launchCondition(sequenceOrder[1], argc, argv);
        cout << "********************************************************************************************************************************" << endl
             << "This part of the experiment is over" << endl << "Please fill in the questionnaire and press enter" << endl 
             << "********************************************************************************************************************************" << endl ;
        //cin >> a ;

        launchCondition(sequenceOrder[2], argc, argv);
        cout << "********************************************************************************************************************************" << endl
             << "The experiment is over, thanks for your participation" << endl 
             << "********************************************************************************************************************************" << endl ;
        
    }
    
    else{           // Cas de bug pendant l'expérience/
        //First recover the sequence thanks to the subject ID.
        sequenceOrder = getPermutation(argv);
        //Then check whether the first condition was done by trying to find the file
        bool hasFoundFailurePoint = false ;
        int i = 2 ;
        string p ;
        int nbOfTrialsDone = 0 ;
        int nbOfConditionsDone = 0 ;
        while(i>0 && hasFoundFailurePoint == false ){
            switch(sequenceOrder[i]){
                case 1:
                    p = path+MOUSE ;
                    if(boost::filesystem::exists(p)){
                        nbOfTrialsDone = getNextTrialToDo(p);
                        cout <<"Nb Done = " << nbOfTrialsDone << endl ;
                        if(nbOfTrialsDone < NBOFTRIALS){
                            cout << "Restart experiment"<<endl ;
                            mainmouse::launchMouseExp(argc, argv, trialTargets, path+TOUCH,nbOfTrialsDone);
                            hasFoundFailurePoint = true ;
                        }
                    }
                    break;
                case 2:
                    p = path+TOUCH ;
                    if(boost::filesystem::exists(p)){
                        nbOfTrialsDone = getNextTrialToDo(p);
                        cout << "Nb Done = " << nbOfTrialsDone << endl ;
                        if(nbOfTrialsDone < NBOFTRIALS){
                            cout << "Restart experiment"<<endl ;
                            maintouch::launchTouchExp(argc, argv, trialTargets, path+TOUCH,nbOfTrialsDone);
                            hasFoundFailurePoint = true ;
                        }
                    }
                    break;
                case 3:
                    p = path+TANGIBLE ;
                    if(boost::filesystem::exists(path+TANGIBLE)){
                        nbOfTrialsDone = getNextTrialToDo(p);
                        cout << "Nb Done = " << nbOfTrialsDone << endl ;
                        if(nbOfTrialsDone < NBOFTRIALS){
                            cout << "Restart experiment"<<endl ;
                            hasFoundFailurePoint = true ;
                        }
                    }
                    break;
            }
            cout <<"Un tour" << endl ;
            i--;
            nbOfConditionsDone ++ ;
        }
        /*for(int i = 0 ; i < 3 ; i++){       //Do the remaining conditions
            launchCondition(sequenceOrder[i], argc, argv);
             cout << "********************************************************************************************************************************" << endl
             << "The experiment is over, thanks for your participation" << endl 
             << "********************************************************************************************************************************" << endl ;
        }*/
    }
    return 0 ;

}


