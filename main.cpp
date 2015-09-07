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
#include "mainTangible.cpp"


std::vector<int> sequenceOrder ;
std::vector<glm::mat4> targetMatrices ;
std::vector <std::tuple<int,glm::mat4>> trialTargetsMouse ;
std::vector <std::tuple<int,glm::mat4>> trialTargetsTouch ;
std::vector <std::tuple<int,glm::mat4>> trialTargetsTangible ;
string path ;
std::string lang ;
ofstream* outfile ;
int subId ;

std::string localURL = "file:///home/lonni/Questionnaire/" ;

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
    int seed = subjectID * condition * 40 ;
    cout << "SEED = " << seed << endl ;
    srand(seed);
    switch(condition){
        case MOUSECONDITION:
            for (int i=0; i<(NBOFTRIALS); i++) {
                int r = i + (rand() % (NBOFTRIALS-i)); // Random remaining position.
                if( i != 0 && r != 0){
                    tuple<int,glm::mat4> temp = trialTargetsMouse[i]; 
                    trialTargetsMouse[i] = trialTargetsMouse[r]; 
                    trialTargetsMouse[r] = temp;
                }
                
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
                if( i != 0 && r != 0){
                    tuple<int,glm::mat4> temp = trialTargetsTouch[i]; 
                    trialTargetsTouch[i] = trialTargetsTouch[r]; 
                    trialTargetsTouch[r] = temp;
                }
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
                if( i != 0 && r != 0){
                    tuple<int,glm::mat4> temp = trialTargetsTangible[i]; 
                    trialTargetsTangible[i] = trialTargetsTangible[r]; 
                    trialTargetsTangible[r] = temp;
                }
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

    /* Previous good targets
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
    */

    //Nouvelle version
    /*trialTargetsMouse.push_back(tuple<int,glm::mat4>(0,glm::mat4(0.658681,-0.197141,0.726137,0.000000,0.474152,0.858088,-0.197141,0.000000,-0.584225,0.474152,0.658681,0.000000,28.004944,-113.787292,135.759277,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(1,glm::mat4(0.549088,-0.163457,0.819624,0.000000,0.497585,0.851875,-0.163457,0.000000,-0.671500,0.497585,0.549088,0.000000,-125.740555,95.916077,214.761047,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(2,glm::mat4(0.764973,-0.218455,0.605883,0.000000,0.424016,0.878913,-0.218455,0.000000,-0.484796,0.424016,0.764973,0.000000,4.647064,-13.301865,274.595337,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(3,glm::mat4(0.040655,0.390922,0.919526,0.000000,-0.197489,-0.898987,0.390922,0.000000,0.979462,-0.197489,0.040655,0.000000,98.075043,-123.505524,-101.267014,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(4,glm::mat4(0.618828,0.785526,-0.000887,0.000000,-0.485675,0.383496,0.785526,0.000000,0.617391,-0.485675,0.618828,0.000000,180.633270,116.193329,-174.171478,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(5,glm::mat4(0.321540,0.851785,0.413612,0.000000,-0.467067,-0.237299,0.851785,0.000000,0.823687,-0.467067,0.321540,0.000000,169.384460,68.591141,-227.182419,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(6,glm::mat4(0.450038,-0.128556,0.883708,0.000000,0.497497,0.857887,-0.128556,0.000000,-0.741594,0.497497,0.450038,0.000000,203.360657,-62.550522,-11.574066,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(7,glm::mat4(0.875099,-0.213766,0.434173,0.000000,0.330607,0.919241,-0.213766,0.000000,-0.353414,0.330607,0.875099,0.000000,140.816284,-0.269684,-198.456253,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(8,glm::mat4(0.009652,-0.195069,0.980742,0.000000,0.097771,-0.975904,-0.195069,0.000000,0.995162,0.097771,0.009652,0.000000,-209.993423,-80.554298,6.726379,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(9,glm::mat4(0.002447,0.000060,0.999997,0.000000,-0.049407,0.998779,0.000060,0.000000,-0.998776,-0.049407,0.002447,0.000000,134.669464,8.294662,14.943573,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(10,glm::mat4(0.607742,0.794049,0.011626,0.000000,-0.488254,0.362069,0.794049,0.000000,0.626305,-0.488254,0.607742,0.000000,-209.973709,-152.217224,-273.094452,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(11,glm::mat4(0.226662,-0.050493,0.972664,0.000000,0.418672,0.906733,-0.050493,0.000000,-0.879397,0.418672,0.226662,0.000000,113.120758,-96.753731,141.431488,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(12,glm::mat4(0.164420,0.031839,0.985877,0.000000,-0.370656,0.928224,0.031839,0.000000,-0.914101,-0.370656,0.164420,0.000000,-171.312073,-115.043640,73.523010,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(13,glm::mat4(0.000020,0.000000,1.000000,0.000000,-0.004445,0.999990,0.000000,0.000000,-0.999990,-0.004445,0.000020,0.000000,153.534790,-16.804626,184.873169,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(14,glm::mat4(0.364408,0.864947,0.345071,0.000000,-0.481264,-0.142311,0.864947,0.000000,0.797240,-0.481264,0.364408,0.000000,-98.583603,4.359116,289.244873,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(15,glm::mat4(0.144224,0.676314,0.722357,0.000000,-0.351317,-0.647438,0.676314,0.000000,0.925082,-0.351317,0.144224,0.000000,91.782562,-56.775108,283.820801,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(16,glm::mat4(0.724778,0.680933,-0.105009,0.000000,-0.446626,0.580393,0.680933,0.000000,0.524616,-0.446626,0.724778,0.000000,129.908600,-166.924438,-280.449646,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(17,glm::mat4(0.294588,-0.072988,0.952833,0.000000,0.455857,0.887055,-0.072988,0.000000,-0.839888,0.455857,0.294588,0.000000,9.671982,13.739090,93.380737,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(18,glm::mat4(0.064338,0.482683,0.873429,0.000000,-0.245353,-0.840725,0.482683,0.000000,0.967296,-0.245353,0.064338,0.000000,-133.568115,16.719925,-296.400482,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(19,glm::mat4(0.735385,0.668047,-0.113673,0.000000,-0.441128,0.599265,0.668047,0.000000,0.514407,-0.441128,0.735385,0.000000,157.833710,-6.860657,-82.924911,1.000000)));
    /*trialTargetsMouse.push_back(tuple<int,glm::mat4>(20,glm::mat4(0.846957,-0.219183,0.484378,0.000000,0.360029,0.906829,-0.219183,0.000000,-0.391207,0.360029,0.846957,0.000000,179.785645,143.544708,-148.763046,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(21,glm::mat4(0.217853,0.777852,0.589480,0.000000,-0.412787,-0.473870,0.777852,0.000000,0.884391,-0.412787,0.217853,0.000000,165.888885,109.012848,-142.573776,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(22,glm::mat4(0.367862,-0.098821,0.924615,0.000000,0.482223,0.870457,-0.098821,0.000000,-0.795071,0.482223,0.367862,0.000000,-150.306610,78.430542,3.668823,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(23,glm::mat4(0.860797,-0.217007,0.460365,0.000000,0.346158,0.912734,-0.217007,0.000000,-0.373099,0.346158,0.860797,0.000000,159.037445,50.794205,-73.222916,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(24,glm::mat4(0.111685,-0.611847,0.783051,0.000000,0.314979,-0.725556,-0.611847,0.000000,0.942505,0.314979,0.111685,0.000000,179.191071,-125.668167,-43.893219,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(25,glm::mat4(0.000007,0.005218,0.999986,0.000000,-0.002609,-0.999983,0.005218,0.000000,0.999997,-0.002609,0.000007,0.000000,168.291809,162.766510,-41.361633,1.000000)));
    */  
    //Good for translation down pour l'offset
    
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(0,glm::mat4(0.000555,0.047107,0.998890,0.000000,-0.023557,-0.998612,0.047107,0.000000,0.999722,-0.023557,0.000555,0.000000,-193.181595,75.380508,163.629272,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(1,glm::mat4(0.069996,0.009091,0.997506,0.000000,-0.255140,0.966861,0.009091,0.000000,-0.964367,-0.255140,0.069996,0.000000,71.454041,142.369324,36.583443,1.000000)));
    //trialTargetsMouse.push_back(tuple<int,glm::mat4>(2,glm::mat4(0.143439,-0.026112,0.989315,0.000000,0.350520,0.936191,-0.026112,0.000000,-0.925506,0.350520,0.143439,0.000000,-57.417648,321.582672,153.845856,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(2,glm::mat4(0.969477,0.202077,-0.138854,0.000000,-0.172023,0.964144,0.202077,0.000000,0.174710,-0.172023,0.969477,0.000000,262.138916,95.215485,113.147049,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(3,glm::mat4(0.000065,0.000000,1.000000,0.000000,-0.008050,0.999968,0.000000,0.000000,-0.999968,-0.008050,0.000065,0.000000,-8.931381,14.859100,35.254559,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(19,glm::mat4(0.254607,-0.059525,0.965211,0.000000,0.435640,0.898151,-0.059525,0.000000,-0.863361,0.435640,0.254607,0.000000,48.188202,198.111969,60.742920,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(5,glm::mat4(0.873385,0.450869,-0.184162,0.000000,-0.332541,0.828332,0.450869,0.000000,0.355829,-0.332541,0.873385,0.000000,-178.737488,95.699677,120.482513,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(6,glm::mat4(0.150037,-0.686338,0.711638,0.000000,0.357108,-0.633572,-0.686338,0.000000,0.921934,0.357108,0.150037,0.000000,163.179352,40.879269,111.113747,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(7,glm::mat4(0.348375,-0.091845,0.932845,0.000000,0.476456,0.874388,-0.091845,0.000000,-0.807233,0.476456,0.348375,0.000000,-206.264679,66.496765,203.010254,1.000000)));
    //trialTargetsMouse.push_back(tuple<int,glm::mat4>(8,glm::mat4(0.108831,-0.017435,0.993907,0.000000,0.311428,0.950110,-0.017435,0.000000,-0.944017,0.311428,0.108831,0.000000,-187.877090,85.485489,32.279442,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(8,glm::mat4(0.248593,-0.057553,0.966897,0.000000,0.432197,0.899941,-0.057553,0.000000,-0.866837,0.432197,0.248593,0.000000,263.883179,58.691277,115.530609,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(9,glm::mat4(0.074366,-0.009944,0.997181,0.000000,0.262365,0.964918,-0.009944,0.000000,-0.962099,0.262365,0.074366,0.000000,96.353516,44.863907,73.929245,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(10,glm::mat4(0.035053,0.364575,0.930514,0.000000,-0.183914,-0.912831,0.364575,0.000000,0.982317,-0.183914,0.035053,0.000000,-91.444267,6.498459,262.595062,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(11,glm::mat4(0.531007,-0.157281,0.832643,0.000000,0.499038,0.852188,-0.157281,0.000000,-0.684830,0.499038,0.531007,0.000000,-161.527725,53.267868,231.236435,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(12,glm::mat4(0.430622,-0.121527,0.894313,0.000000,0.495163,0.860258,-0.121527,0.000000,-0.754572,0.495163,0.430622,0.000000,48.420273,2.468903,244.188538,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(13,glm::mat4(0.000555,0.047107,0.998890,0.000000,-0.023557,-0.998612,0.047107,0.000000,0.999722,-0.023557,0.000555,0.000000,-193.181595,75.380508,163.629272,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(14,glm::mat4(0.017078,-0.258014,0.965990,0.000000,0.129563,-0.957414,-0.258015,0.000000,0.991424,0.129563,0.017078,0.000000,-193.971252,-20.562592,159.316574,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(15,glm::mat4(0.961740,-0.154303,0.226379,0.000000,0.191824,0.969224,-0.154303,0.000000,-0.195602,0.191824,0.961740,0.000000,-88.035118,74.919800,152.037613,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(16,glm::mat4(0.856344,0.483679,-0.180915,0.000000,-0.350741,0.801895,0.483679,0.000000,0.379020,-0.350741,0.856344,0.000000,30.477890,64.557098,142.537338,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(17,glm::mat4(0.009425,0.192788,0.981195,0.000000,-0.096622,-0.976472,0.192788,0.000000,0.995277,-0.096622,0.009425,0.000000,-101.480362,106.469299,34.195065,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(18,glm::mat4(0.845715,-0.219337,0.486474,0.000000,0.361221,0.906317,-0.219337,0.000000,-0.392791,0.361221,0.845715,0.000000,-196.430435,329.216064,340.812378,1.000000)));
    trialTargetsMouse.push_back(tuple<int,glm::mat4>(19,glm::mat4(0.254607,-0.059525,0.965211,0.000000,0.435640,0.898151,-0.059525,0.000000,-0.863361,0.435640,0.254607,0.000000,48.188202,198.111969,60.742920,1.000000)));
    //trialTargetsMouse.push_back(tuple<int,glm::mat4>(20,glm::mat4(0.345669,-0.090881,0.933945,0.000000,0.475586,0.874962,-0.090881,0.000000,-0.808907,0.475586,0.345669,0.000000,166.775848,347.226471,343.508270,1.000000)));
    
    



    
    for (int i = 0 ; i < trialTargetsMouse.size() ; i++){
        trialTargetsTouch.push_back(trialTargetsMouse[i]);
        trialTargetsTangible.push_back(trialTargetsMouse[i]);
    }


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
            maintangible::launchTangibleExp(argc, argv, trialTargetsTangible, path+TANGIBLE);
			break ;
		default:
			cerr << "Error, not a valid condition number. " << ind << " is not valid." << std::endl ;
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

	if(argc < 3){
        cerr << "Please enter subject ID and language!!\n" ;
        return -1 ;
    }
    lang = argv[2];
    cout << "Lang = " << lang << endl ;
    if(lang != "eng" && lang != "fr"){
        cerr << "Please enter a valid language" << endl ;
        return -1 ;
    }
    subId = atoi(argv[1]);
    path = "./results/"+string(argv[1]) ;
    sequenceOrder = getPermutation(argv);
    loadTargets();
    
    if(createDirectory()!=-1){
        getPermutation(argv, MOUSECONDITION);
        getPermutation(argv, TOUCHCONDITION);
        getPermutation(argv, TANGIBLECONDITION);
    	createSubjectFileInfo(argv[1], sequenceOrder);
        //First questionnaire
        if(lang=="fr") system(("firefox '"+localURL+"initialFr.html#id="+to_string(subId)+"'").c_str());
        else if(lang=="eng") system(("google-chrome "+localURL+"initial.html?id="+to_string(subId)).c_str());
        cout << "URL == " << ("google-chrome "+localURL+"initialFr.html?id="+to_string(subId)).data() <<endl ;
        
        cout << "Si le questionnaire est rempli, appuyez sur entree" << endl ;
        getline(cin,a);


        launchCondition(sequenceOrder[0], argc, argv);

        cout << "********************************************************************************************************************************" << endl 
            << "Cette partie est terminée, merci de remplir le questionnaire correspondant" << endl 
            << "********************************************************************************************************************************" << endl ;
        
            cout << "Si le questionnaire est rempli, appuyez sur entree" << endl ;
        getline(cin,a);

        launchCondition(sequenceOrder[1], argc, argv);
        cout << "********************************************************************************************************************************" << endl 
            << "Cette partie est terminée, merci de remplir le questionnaire correspondant" << endl 
            << "********************************************************************************************************************************" << endl ;
        
        cout << "Si le questionnaire est rempli, appuyez sur entree" << endl ;
        getline(cin,a);

        launchCondition(sequenceOrder[2], argc, argv);
        cout << "********************************************************************************************************************************" << endl 
            << "Cette partie est terminée, merci de remplir le questionnaire correspondant" << endl 
            << "********************************************************************************************************************************" << endl ;
        if(lang=="fr") system(("google-chrome "+localURL+"TLXFr.html?id="+to_string(subId)+"&condition=tangible").c_str());
        else if(lang=="eng") system(("google-chrome "+localURL+"TLXFr.html?id="+to_string(subId)+"&condition=tangible").c_str());
        cout << "Si le questionnaire est rempli, appuyez sur entree" << endl ;
        getline(cin,a);
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
            cout << "********************************************************************************************************************************" << endl 
            << "Cette partie est terminée, merci de remplir le questionnaire correspondant" << endl 
            << "********************************************************************************************************************************" << endl ;
        
            cout << "Si le questionnaire est rempli, appuyez sur entree" << endl ;
            getline(cin,a);
        }

        launchCondition(sequenceOrder[i], argc, argv);
             cout << "********************************************************************************************************************************" << endl
             << "Cette partie est terminée, merci de remplir le questionnaire correspondant" << endl 
             << "********************************************************************************************************************************" << endl ;
    }
    return 0 ;

}


