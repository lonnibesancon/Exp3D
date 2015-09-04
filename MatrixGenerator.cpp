#include <iostream>
#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

using namespace std ;



float maxTranslationRight = 300 ;
float maxTranslationUp = 350 ;
float maxTranslationForward = 380 ;
float minTranslationRight = -217 ;
float minTranslationUp = 40 ;
float minTranslationForward = 30 ;
float maxRotation = 2  ;
float minRotation = - 2 ;
int seed = 15169 ;


string tostring(glm::vec3 v){
    string s = "";
    s +=std::to_string(v[0])+","+to_string(v[1])+","+to_string(v[2]) ;   // On ajoute pas de ";" parce qu'il y en a déjà dans la fonction getMatrixHistory
    return s ;
}


string tostring(glm::mat4 mat){
    string s = "";
    for(int i = 0 ; i < 3 ; i ++){
        glm::vec4 currentvec = mat[i];
        s +=to_string(currentvec[0])+","+to_string(currentvec[1])+","+to_string(currentvec[2])+","+to_string(currentvec[3])+"," ;   //On doit ajouter un ";" pour chaque vec4 sauf le dernier qui en aura par la fonction getMatrixHistory();
    }
    glm::vec4 currentvec = mat[3];
    s +=to_string(currentvec[0])+","+to_string(currentvec[1])+","+to_string(currentvec[2])+","+to_string(currentvec[3]) ;
    return s ;
}


glm::mat4 id(1.0f);

glm::mat4 generateRandomModelMatrix(){
    float t1 = minTranslationRight + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxTranslationRight-minTranslationRight)));
    float t2 = minTranslationUp + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxTranslationUp-minTranslationUp)));
    float t3 = minTranslationForward + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxTranslationForward-minTranslationForward)));
    float r1 = minRotation + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxRotation-minRotation)));
    float r2 = minRotation + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxRotation-minRotation)));
    float r3 = minRotation + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxRotation-minRotation)));
    glm::mat4 targetModel = glm::translate(id, glm::vec3(t1,t2,t3));
    targetModel = glm::rotate(targetModel, r1, glm::vec3(1,0,0));
    targetModel = glm::rotate(targetModel, r1, glm::vec3(0,1,0));
    targetModel = glm::rotate(targetModel, r1, glm::vec3(0,0,1));
    //printMatrix(targetModel);

    return targetModel ;

}

int main(){
    srand(seed);
    for (int i = 0 ; i < 40 ; i++){
        cout <<"trialTargetsMouse.push_back(tuple<int,glm::mat4>("<<i<<",glm::mat4("<< tostring(generateRandomModelMatrix())<< ")));" << std::endl ;
    }
    return 0 ;
}



