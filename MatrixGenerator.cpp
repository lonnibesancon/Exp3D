#include <iostream>
#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

using namespace std ;


float maxRotation = 1  ;
float maxTranslation = 1 ;
float minRotation = - 1 ;
float minTranslation = - 1 ;
int seed = 100 ;

glm::mat4 id(1.0f);

glm::mat4 generateRandomModelMatrix(){
    float t1 = minTranslation + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxTranslation-minTranslation)));
    float t2 = minTranslation + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxTranslation-minTranslation)));
    float t3 = minTranslation + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxTranslation-minTranslation)));
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
    for (int i = 0 ; i < 15 ; i++){
        cout << glm::to_string(generateRandomModelMatrix()) << '\n' ;
    }
    return 0 ;
}