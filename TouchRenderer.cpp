#include "TouchRenderer.h"

using namespace std ;

TouchRenderer::TouchRenderer(void)
{
}


TouchRenderer::~TouchRenderer(void)
{
}


void TouchRenderer::add(long id, double x, double y){
	TouchPoint t = TouchPoint(id,x,y);
	touchpoints.push_back(t);
}
void TouchRenderer::remove(long id){

}
void TouchRenderer::update(long id, double x, double y){

}