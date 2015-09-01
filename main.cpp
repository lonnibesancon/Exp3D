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
//#include "maintangible.cpp"


std::vector<int> sequenceOrder ;
std::vector<glm::mat4> targetMatrices ;
std::vector <std::tuple<int,glm::mat4>> trialTargetsMouse ;
std::vector <std::tuple<int,glm::mat4>> trialTargetsTouch ;
std::vector <std::tuple<int,glm::mat4>> trialTargetsTangible ;
string path ;
ofstream* outfile ;

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
    outfile = new ofstream(path+"/info.txt");
    *outfile << "Subject ID: " << subID << endl ;
    *outfile << "Order: " << order[0] << " - " << order[1] << " - " << order[2] << std::endl ;
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
    int subjectID = atoi(argv[1]);
    int seed = subjectID * condition ;
    cout << "SEED = " << seed << endl ;
    srand(seed);
    switch(condition){
        case MOUSECONDITION:
            for (int i=0; i<(NBOFTRIALS); i++) {
                int r = i + (rand() % (NBOFTRIALS-i)); // Random remaining position.
                tuple<int,glm::mat4> temp = trialTargetsMouse[i]; 
                trialTargetsMouse[i] = trialTargetsMouse[r]; 
                trialTargetsMouse[r] = temp;
            }

            for(int i=0;i<NBOFTRIALS;i++){
                std::cout << get<0>(trialTargetsMouse[i]) << " ; " ;
                //*outfile << get<0>(trialTargets[i]) << " ; " ;
            }
            cout << endl ;
        break;
        case TOUCHCONDITION:
            for (int i=0; i<(NBOFTRIALS); i++) {
                int r = i + (rand() % (NBOFTRIALS-i)); // Random remaining position.
                tuple<int,glm::mat4> temp = trialTargetsTouch[i]; 
                trialTargetsTouch[i] = trialTargetsTouch[r]; 
                trialTargetsTouch[r] = temp;
            }

            for(int i=0;i<NBOFTRIALS;i++){
                std::cout << get<0>(trialTargetsTouch[i]) << " ; " ;
                //*outfile << get<0>(trialTargets[i]) << " ; " ;
            }
            cout << endl ;
        break;
        case TANGIBLECONDITION:
            for (int i=0; i<(NBOFTRIALS); i++) {
                int r = i + (rand() % (NBOFTRIALS-i)); // Random remaining position.
                tuple<int,glm::mat4> temp = trialTargetsTangible[i]; 
                trialTargetsTangible[i] = trialTargetsTangible[r]; 
                trialTargetsTangible[r] = temp;
            }

            for(int i=0;i<NBOFTRIALS;i++){
                std::cout << get<0>(trialTargetsTangible[i]) << " ; " ;
                //*outfile << get<0>(trialTargets[i]) << " ; " ;
            }
            cout << endl ;
        break;

    }
    
    //*outfile << endl ;

}


/*void getPermutationFromFile(short conditon){
    ifstream in = new ifstream(path+MOUSE+"/info.txt");
    cout << " Path = " << path+MOUSE+"/info.txt" << endl ;
    string line ;
    bool found = false ;
    switch(condition){
        case MOUSECONDITION:
            while(found == false && in >> line){
                if (string::npos !=line.find("Mouse condition")){
                    found = true ;
                }
            }
            //std::vector<std::string> split(line, ":");
            std::vector<std::string> sequenceString = split(split(line,":")[1], ";");
            for (int i = 0 ; i < sequenceString ; i++){
                trialTargets[0] = stod()
            }
        break ;


    }

    outfile = new ofstream(path+"/info.txt", std::ios_base::app); 
}*/


void loadTargets(){
    /*trialTargets.push_back(tuple<int,glm::mat4>(1,glm::mat4(0.993176, -0.075523, 0.088865, 0.000000, 0.082323, 0.993740, -0.075523, 0.000000, -0.082605, 0.082323, 0.993176, 0.000000, -0.999984, -0.736924, 0.511211, 1.000000)));
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
    trialTargets.push_back(tuple<int,glm::mat4>(15,glm::mat4(1.0f)));*/

    trialTargetsMouse.push_back(tuple<int,glm::mat4>(0,glm::mat4(0.012214,0.219008,0.975647,0.000000,-0.109840,-0.969521,0.219008,0.000000,0.993874,-0.109840,0.012214,0.000000,-209.671295,-117.715202,-25.787109,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(1,glm::mat4(0.092643,-0.566108,0.819109,0.000000,0.289932,-0.771662,-0.566108,0.000000,0.952553,0.289932,0.092643,0.000000,-169.455475,-14.548248,-111.534592,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(2,glm::mat4(0.236854,0.796558,0.556234,0.000000,-0.425152,-0.429816,0.796558,0.000000,0.873582,-0.425152,0.236854,0.000000,179.129120,51.244812,10.695312,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(3,glm::mat4(0.951662,-0.167324,0.257570,0.000000,0.214480,0.962290,-0.167324,0.000000,-0.219859,0.214480,0.951662,0.000000,87.435608,-40.206970,139.895752,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(4,glm::mat4(0.765237,-0.218485,0.605538,0.000000,0.423851,0.878985,-0.218485,0.000000,-0.484523,0.423851,0.765237,0.000000,112.069122,-124.547279,83.988800,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(5,glm::mat4(0.364408,0.864947,0.345071,0.000000,-0.481264,-0.142311,0.864947,0.000000,0.797240,-0.481264,0.364408,0.000000,-98.583603,4.359116,142.829926,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(6,glm::mat4(0.023012,-0.001735,0.999734,0.000000,0.149940,0.988694,-0.001735,0.000000,-0.988427,0.149940,0.023012,0.000000,-22.965271,2.281967,-128.785141,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(7,glm::mat4(0.724778,0.680933,-0.105009,0.000000,-0.446626,0.580393,0.680933,0.000000,0.524616,-0.446626,0.724778,0.000000,129.908600,-166.924438,-236.966431,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(8,glm::mat4(0.000117,0.021641,0.999766,0.000000,-0.010821,-0.999707,0.021641,0.000000,0.999941,-0.010821,0.000117,0.000000,-144.130310,131.150421,-19.979401,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(9,glm::mat4(0.970208,-0.140669,0.197255,0.000000,0.170014,0.975350,-0.140669,0.000000,-0.172605,0.170014,0.970208,0.000000,-192.031769,-160.208511,-243.396637,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(10,glm::mat4(0.823489,-0.221077,0.522486,0.000000,0.381255,0.897647,-0.221077,0.000000,-0.420133,0.381255,0.823489,0.000000,-107.661423,-85.453110,-109.263596,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(11,glm::mat4(0.549088,-0.163457,0.819624,0.000000,0.497585,0.851875,-0.163457,0.000000,-0.671500,0.497585,0.549088,0.000000,-125.740555,95.916077,93.174072,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(12,glm::mat4(0.165521,0.032148,0.985682,0.000000,-0.371650,0.927816,0.032148,0.000000,-0.913498,-0.371650,0.165521,0.000000,-116.528893,-33.194672,-6.149170,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(13,glm::mat4(0.764973,-0.218455,0.605883,0.000000,0.424016,0.878913,-0.218455,0.000000,-0.484796,0.424016,0.764973,0.000000,4.647064,-13.301865,133.063599,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(14,glm::mat4(0.032174,0.350064,0.936173,0.000000,-0.176463,-0.919954,0.350064,0.000000,0.983781,-0.176463,0.032174,0.000000,-67.628128,-85.861092,-176.224854,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(15,glm::mat4(0.618828,0.785526,-0.000887,0.000000,-0.485675,0.383496,0.785526,0.000000,0.617391,-0.485675,0.618828,0.000000,180.633270,116.193329,-166.114319,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(16,glm::mat4(0.653706,-0.195802,0.730979,0.000000,0.475788,0.857489,-0.195802,0.000000,-0.588467,0.475788,0.653706,0.000000,-68.206772,-138.521912,-235.616882,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(17,glm::mat4(1.000000,-0.000135,0.000135,0.000000,0.000135,1.000000,-0.000135,0.000000,-0.000135,0.000135,1.000000,0.000000,-190.643646,26.122192,62.667358,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(18,glm::mat4(0.450038,-0.128556,0.883708,0.000000,0.497497,0.857887,-0.128556,0.000000,-0.741594,0.497497,0.450038,0.000000,203.360657,-62.550522,-57.716049,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(19,glm::mat4(0.066708,0.490565,0.868847,0.000000,-0.249515,-0.834918,0.490565,0.000000,0.966070,-0.249515,0.066708,0.000000,209.977539,-135.090347,16.575287,1.000000)));

    for (int i = 0 ; i < trialTargetsMouse.size() ; i++){
        trialTargetsTouch.push_back(trialTargetsMouse[i]);
        trialTargetsTangible.push_back(trialTargetsMouse[i]);
    }

    /*trialTargetsTouch.push_back(tuple<int,glm::mat4>(0,glm::mat4(0.012214,0.219008,0.975647,0.000000,-0.109840,-0.969521,0.219008,0.000000,0.993874,-0.109840,0.012214,0.000000,-209.671295,-117.715202,-25.787109,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(1,glm::mat4(0.092643,-0.566108,0.819109,0.000000,0.289932,-0.771662,-0.566108,0.000000,0.952553,0.289932,0.092643,0.000000,-169.455475,-14.548248,-111.534592,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(2,glm::mat4(0.236854,0.796558,0.556234,0.000000,-0.425152,-0.429816,0.796558,0.000000,0.873582,-0.425152,0.236854,0.000000,179.129120,51.244812,10.695312,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(3,glm::mat4(0.951662,-0.167324,0.257570,0.000000,0.214480,0.962290,-0.167324,0.000000,-0.219859,0.214480,0.951662,0.000000,87.435608,-40.206970,139.895752,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(4,glm::mat4(0.765237,-0.218485,0.605538,0.000000,0.423851,0.878985,-0.218485,0.000000,-0.484523,0.423851,0.765237,0.000000,112.069122,-124.547279,83.988800,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(5,glm::mat4(0.364408,0.864947,0.345071,0.000000,-0.481264,-0.142311,0.864947,0.000000,0.797240,-0.481264,0.364408,0.000000,-98.583603,4.359116,142.829926,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(6,glm::mat4(0.023012,-0.001735,0.999734,0.000000,0.149940,0.988694,-0.001735,0.000000,-0.988427,0.149940,0.023012,0.000000,-22.965271,2.281967,-128.785141,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(7,glm::mat4(0.724778,0.680933,-0.105009,0.000000,-0.446626,0.580393,0.680933,0.000000,0.524616,-0.446626,0.724778,0.000000,129.908600,-166.924438,-236.966431,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(8,glm::mat4(0.000117,0.021641,0.999766,0.000000,-0.010821,-0.999707,0.021641,0.000000,0.999941,-0.010821,0.000117,0.000000,-144.130310,131.150421,-19.979401,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(9,glm::mat4(0.970208,-0.140669,0.197255,0.000000,0.170014,0.975350,-0.140669,0.000000,-0.172605,0.170014,0.970208,0.000000,-192.031769,-160.208511,-243.396637,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(10,glm::mat4(0.823489,-0.221077,0.522486,0.000000,0.381255,0.897647,-0.221077,0.000000,-0.420133,0.381255,0.823489,0.000000,-107.661423,-85.453110,-109.263596,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(11,glm::mat4(0.549088,-0.163457,0.819624,0.000000,0.497585,0.851875,-0.163457,0.000000,-0.671500,0.497585,0.549088,0.000000,-125.740555,95.916077,93.174072,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(12,glm::mat4(0.165521,0.032148,0.985682,0.000000,-0.371650,0.927816,0.032148,0.000000,-0.913498,-0.371650,0.165521,0.000000,-116.528893,-33.194672,-6.149170,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(13,glm::mat4(0.764973,-0.218455,0.605883,0.000000,0.424016,0.878913,-0.218455,0.000000,-0.484796,0.424016,0.764973,0.000000,4.647064,-13.301865,133.063599,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(14,glm::mat4(0.032174,0.350064,0.936173,0.000000,-0.176463,-0.919954,0.350064,0.000000,0.983781,-0.176463,0.032174,0.000000,-67.628128,-85.861092,-176.224854,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(15,glm::mat4(0.618828,0.785526,-0.000887,0.000000,-0.485675,0.383496,0.785526,0.000000,0.617391,-0.485675,0.618828,0.000000,180.633270,116.193329,-166.114319,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(16,glm::mat4(0.653706,-0.195802,0.730979,0.000000,0.475788,0.857489,-0.195802,0.000000,-0.588467,0.475788,0.653706,0.000000,-68.206772,-138.521912,-235.616882,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(17,glm::mat4(1.000000,-0.000135,0.000135,0.000000,0.000135,1.000000,-0.000135,0.000000,-0.000135,0.000135,1.000000,0.000000,-190.643646,26.122192,62.667358,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(18,glm::mat4(0.450038,-0.128556,0.883708,0.000000,0.497497,0.857887,-0.128556,0.000000,-0.741594,0.497497,0.450038,0.000000,203.360657,-62.550522,-57.716049,1.000000)));
    trialTargetsTouch.push_back(tuple<int,glm::mat4>(19,glm::mat4(0.066708,0.490565,0.868847,0.000000,-0.249515,-0.834918,0.490565,0.000000,0.966070,-0.249515,0.066708,0.000000,209.977539,-135.090347,16.575287,1.000000)));

    trialTargetsTangible.push_back(tuple<int,glm::mat4>(0,glm::mat4(0.012214,0.219008,0.975647,0.000000,-0.109840,-0.969521,0.219008,0.000000,0.993874,-0.109840,0.012214,0.000000,-209.671295,-117.715202,-25.787109,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(1,glm::mat4(0.092643,-0.566108,0.819109,0.000000,0.289932,-0.771662,-0.566108,0.000000,0.952553,0.289932,0.092643,0.000000,-169.455475,-14.548248,-111.534592,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(2,glm::mat4(0.236854,0.796558,0.556234,0.000000,-0.425152,-0.429816,0.796558,0.000000,0.873582,-0.425152,0.236854,0.000000,179.129120,51.244812,10.695312,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(3,glm::mat4(0.951662,-0.167324,0.257570,0.000000,0.214480,0.962290,-0.167324,0.000000,-0.219859,0.214480,0.951662,0.000000,87.435608,-40.206970,139.895752,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(4,glm::mat4(0.765237,-0.218485,0.605538,0.000000,0.423851,0.878985,-0.218485,0.000000,-0.484523,0.423851,0.765237,0.000000,112.069122,-124.547279,83.988800,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(5,glm::mat4(0.364408,0.864947,0.345071,0.000000,-0.481264,-0.142311,0.864947,0.000000,0.797240,-0.481264,0.364408,0.000000,-98.583603,4.359116,142.829926,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(6,glm::mat4(0.023012,-0.001735,0.999734,0.000000,0.149940,0.988694,-0.001735,0.000000,-0.988427,0.149940,0.023012,0.000000,-22.965271,2.281967,-128.785141,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(7,glm::mat4(0.724778,0.680933,-0.105009,0.000000,-0.446626,0.580393,0.680933,0.000000,0.524616,-0.446626,0.724778,0.000000,129.908600,-166.924438,-236.966431,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(8,glm::mat4(0.000117,0.021641,0.999766,0.000000,-0.010821,-0.999707,0.021641,0.000000,0.999941,-0.010821,0.000117,0.000000,-144.130310,131.150421,-19.979401,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(9,glm::mat4(0.970208,-0.140669,0.197255,0.000000,0.170014,0.975350,-0.140669,0.000000,-0.172605,0.170014,0.970208,0.000000,-192.031769,-160.208511,-243.396637,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(10,glm::mat4(0.823489,-0.221077,0.522486,0.000000,0.381255,0.897647,-0.221077,0.000000,-0.420133,0.381255,0.823489,0.000000,-107.661423,-85.453110,-109.263596,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(11,glm::mat4(0.549088,-0.163457,0.819624,0.000000,0.497585,0.851875,-0.163457,0.000000,-0.671500,0.497585,0.549088,0.000000,-125.740555,95.916077,93.174072,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(12,glm::mat4(0.165521,0.032148,0.985682,0.000000,-0.371650,0.927816,0.032148,0.000000,-0.913498,-0.371650,0.165521,0.000000,-116.528893,-33.194672,-6.149170,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(13,glm::mat4(0.764973,-0.218455,0.605883,0.000000,0.424016,0.878913,-0.218455,0.000000,-0.484796,0.424016,0.764973,0.000000,4.647064,-13.301865,133.063599,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(14,glm::mat4(0.032174,0.350064,0.936173,0.000000,-0.176463,-0.919954,0.350064,0.000000,0.983781,-0.176463,0.032174,0.000000,-67.628128,-85.861092,-176.224854,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(15,glm::mat4(0.618828,0.785526,-0.000887,0.000000,-0.485675,0.383496,0.785526,0.000000,0.617391,-0.485675,0.618828,0.000000,180.633270,116.193329,-166.114319,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(16,glm::mat4(0.653706,-0.195802,0.730979,0.000000,0.475788,0.857489,-0.195802,0.000000,-0.588467,0.475788,0.653706,0.000000,-68.206772,-138.521912,-235.616882,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(17,glm::mat4(1.000000,-0.000135,0.000135,0.000000,0.000135,1.000000,-0.000135,0.000000,-0.000135,0.000135,1.000000,0.000000,-190.643646,26.122192,62.667358,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(18,glm::mat4(0.450038,-0.128556,0.883708,0.000000,0.497497,0.857887,-0.128556,0.000000,-0.741594,0.497497,0.450038,0.000000,203.360657,-62.550522,-57.716049,1.000000)));
    trialTargetsTangible.push_back(tuple<int,glm::mat4>(19,glm::mat4(0.066708,0.490565,0.868847,0.000000,-0.249515,-0.834918,0.490565,0.000000,0.966070,-0.249515,0.066708,0.000000,209.977539,-135.090347,16.575287,1.000000)));
    */


}

void launchCondition(int ind, int argc, char * argv[]){
	switch(ind){
		case 1:
            createConditionDirectory(path+MOUSE);
            //*outfile << "Mouse condition order of trials:" ;
            //getPermutation(argv,100);
            for(int i = 0 ; i < trialTargetsMouse.size() ; i++){
                *outfile << get<0>(trialTargetsMouse[i]) << ";" ;
            }
            *outfile << endl ;
			mainmouse::launchMouseExp(argc, argv, trialTargetsMouse, path+MOUSE);
			break;
		case 2:
            createConditionDirectory(path+TOUCH);
            //*outfile << "Touch condition order of trials:" ;
            //getPermutation(argv,200);
            for(int i = 0 ; i < trialTargetsTouch.size() ; i++){
                *outfile << get<0>(trialTargetsTouch[i]) << ";" ;
            }
            *outfile << endl ;
			maintouch::launchTouchExp(argc, argv, trialTargetsTouch, path+TOUCH);
			break ;
		case 3:
            //*outfile << "Tangible condition order of trials:" ;
			createConditionDirectory(path+TANGIBLE);
            //getPermutation(argv,300);
            for(int i = 0 ; i < trialTargetsTangible.size() ; i++){
                *outfile << get<0>(trialTargetsTangible[i]) << ";" ;
            }
            *outfile << endl ;
            //maintangible::launchTangibleExp(argc, argv, trialTargets, path+TANGIBLE);
			break ;
		default:
			cerr << "Error, not a valid condition number" << std::endl ;
			break ;
	}
}

int getNextTrialToDo(string path){
    int nbOfFiles = 0 ;
    vector<int> trialsDone ;
    cout << "Checking for files in : " << path << endl ;
    for(boost::filesystem::directory_iterator it(path); it != boost::filesystem::directory_iterator(); ++it){
        if(boost::filesystem::is_regular_file(it->status())){
            cout << it->path().stem() << endl ;
            trialsDone.push_back(atoi(it->path().stem().c_str()));
            nbOfFiles++ ;
        }
    }
    cout << "Found " << nbOfFiles << "files" << endl ;
    for(std::vector<string>::size_type i = 0 ; i!=trialsDone.size(); i++){
        cout << "File = " << trialsDone[i] << endl ;
    }
    nbOfFiles /= 3 ;

    if(NBOFTRIALS == nbOfFiles){
        return -1 ;
    }
    else{
        //nbOfFiles -- ;  // Because we start counting from 0 for the vector ;)
        nbOfFiles -- ;  // It is very likely that the last file is corrupted
        return nbOfFiles ;
    }

}




int main(int argc, char *argv[])
{

    string a ;

    glutInit(&argc, argv);

	if(argc < 2){
        cerr << "Please enter subject ID!!\n" ;
        return -1 ;
    }
    path = "./results/"+string(argv[1]) ;
    sequenceOrder = getPermutation(argv);
    loadTargets();
    
    if(createDirectory()!=-1){
        getPermutation(argv, MOUSECONDITION);
        getPermutation(argv, TOUCHCONDITION);
        getPermutation(argv, TANGIBLECONDITION);
    	createSubjectFileInfo(argv[1], sequenceOrder);
        launchCondition(sequenceOrder[0], argc, argv);
        cout << "********************************************************************************************************************************" << endl 
             << "This part of the experiment is over" << endl << "Please fill in the questionnaire and press enter" << endl 
             << "********************************************************************************************************************************" << endl ;
        
        cout << "Press enter to get to next experiment" << endl ;
        getline(cin,a);

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
        outfile = new ofstream(path+"/info.txt", std::ios_base::app);       // To prevent segfault
        *outfile << "Recovery mode launched" << endl ;
        //Then check whether the first condition was done by trying to find the file
        getPermutation(argv, MOUSECONDITION);
        getPermutation(argv, TOUCHCONDITION);
        getPermutation(argv, TANGIBLECONDITION);
        bool hasFoundFailurePoint = false ;
        int i = 2 ;
        string p ;
        int nbOfTrialsDone = 0 ;
        int nbOfConditionsDone = 3 ;
        bool skipTangible = false ;
        bool skipTouch = false ;
        bool skipMouse = false ;
        while(i>=0 && hasFoundFailurePoint == false ){
            switch(sequenceOrder[i]){
                case 1:
                    p = path+MOUSE ;
                    if(skipMouse) break ;
                    if(boost::filesystem::exists(p)){
                        nbOfTrialsDone = getNextTrialToDo(p);
                        cout <<"Nb Done Mouse= " << nbOfTrialsDone << endl ;
                        if(nbOfTrialsDone < NBOFTRIALS && nbOfTrialsDone != -1){
                            cout << "Restart experiment Mouse "<<endl ;
                            //getPermutation(argv,100);
                            mainmouse::launchMouseExp(argc, argv, trialTargetsMouse, path+MOUSE,nbOfTrialsDone);
                            hasFoundFailurePoint = true ;
                        }
                        else{
                            cout << "Mouse experiment complete" << endl ;
                            skipMouse = true ;
                            
                        }
                        nbOfConditionsDone ++ ;
                    }
                    else{
                        cout << "Mouse Not done yet" << endl ;
                        nbOfConditionsDone -- ;
                    }
                    break;
                case 2:
                    p = path+TOUCH ;
                    if(skipTouch) break ;
                    if(boost::filesystem::exists(p)){
                        nbOfTrialsDone = getNextTrialToDo(p);
                        cout << "Nb Done Touch= " << nbOfTrialsDone << endl ;
                        if(nbOfTrialsDone < NBOFTRIALS && nbOfTrialsDone != -1){
                            cout << "Restart experiment Touch"<<endl ;
                            //getPermutation(argv,200);
                            maintouch::launchTouchExp(argc, argv, trialTargetsTouch, path+TOUCH,nbOfTrialsDone);
                            hasFoundFailurePoint = true ;
                        }
                        else{
                            cout << "Touch experiment complete" << endl ;
                            skipTouch = true ;
                        }
                        nbOfConditionsDone ++ ;
                    }
                    else{
                        cout << "Touch Not done yet" << endl ;
                        nbOfConditionsDone -- ;
                    }
                    break;
                case 3:
                    p = path+TANGIBLE ;
                    cout << "TANGIBLE " << endl ;
                    if(skipTangible) break ;
                    if(boost::filesystem::exists(path+TANGIBLE)){
                        nbOfTrialsDone = getNextTrialToDo(p);
                        cout << "Nb Done Tangible= " << nbOfTrialsDone << endl ;
                        if(nbOfTrialsDone < NBOFTRIALS && nbOfTrialsDone != -1 ){
                            cout << "Restart experiment Tangible"<<endl ;
                            //getPermutation(argv,300);
                            hasFoundFailurePoint = true ;
                        }
                        else{
                            cout <<"Tangible experiment complete" << endl ;
                            skipTangible = true ;
                        }
                        nbOfConditionsDone ++ ;
                    }
                    else{
                        cout << "Tangible Not done yet" << endl ;
                        nbOfConditionsDone -- ;
                    }
                    break;
            }
            i--;
        }
        cout << "End of recovery mode" << endl;
        cout << "Nb of conditions done = " << nbOfConditionsDone << endl ;
        if(hasFoundFailurePoint == false){
            cout << "This ID has been previously used without any bug, change ID" << endl ;
        }

        
        for(int i = nbOfConditionsDone ; i < 3 ; i++){       //Do the remaining conditions
            launchCondition(sequenceOrder[i], argc, argv);
             cout << "********************************************************************************************************************************" << endl
             << "The experiment is over, thanks for your participation" << endl 
             << "********************************************************************************************************************************" << endl ;
        }
    }
    return 0 ;

}


