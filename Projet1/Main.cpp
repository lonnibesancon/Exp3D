#ifndef MAIN
#define MAIN

#include "Server/tcp_server.h"
#include "TouchRenderer.h"
#include "TouchListener.h"
#include "Drawing.h"

TouchRenderer* touchRenderer ;
TouchListener* touchListener ;

Drawing* drawing ;



void start(){

	touchRenderer = new TouchRenderer();
	touchListener = new TouchListener(touchRenderer);
	drawing = new Drawing();
	drawing->read();

}

int main()
{
	/*vtkContextView* ctxView = vtkContextView::New();
	vtkCamera* cam = ctxView->GetRenderer()->GetActiveCamera();
	ctxView->GetRenderer()->SetBackground(0.0f, 0.0f, 0.0f);
	vtkRenderWindow *win1 = ctxView->GetRenderWindow();
	vtkRenderer *ren1= vtkRenderer::New();
	loadPDBRibbons(ctxView->GetRenderer(), "");
	ctxView->Render();*/
	start();

  return 0;
}

#endif
