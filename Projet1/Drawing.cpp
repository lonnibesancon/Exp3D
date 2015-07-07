#include "Drawing.h"

vtkConeSource *cone ;
vtkPolyDataMapper *mapper ;
vtkSmartPointer<vtkTransformFilter> transformFilter;

Drawing::Drawing(void)
{
	translation = vtkSmartPointer<vtkTransform>::New();
	ctxView = vtkContextView::New();
	win = ctxView->GetRenderWindow();
	ren = ctxView->GetRenderer();
	cam = ren->GetActiveCamera();
	iren = vtkRenderWindowInteractor::New();
	ren->SetBackground(.0,.0,.0);
	filename = BUNNY;
}


Drawing::~Drawing(void)
{
}

void Drawing::setFileName(std::string file){
	Drawing::filename = file ;
}

void Drawing::scale(float k){
	//vtkSmartPointer<vtkTransform> transform =vtkSmartPointer<vtkTransform>::New();
	//transform->Scale(5,1,1);
	//vtkSmartPointer<vtkTransformFilter> transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
	//transformFilter->SetInputConnection(cone->GetOutputPort());
	//transformFilter->SetTransform(transform);
	//mapper->SetInputConnection(transformFilter->GetOutputPort());
	mainActor->SetScale(k);
	ren->Render();
}

void Drawing::translate(float x, float y, float z){
	cout << "Translate: " << x << " - " << " - " << y << " - " << z << endl ;
	vtkSmartPointer<vtkTransform> transform1a = vtkSmartPointer<vtkTransform>::New();
	//transform1a->Translate(x,y,z);
	//transformFilter->SetTransform(transform1a);
	//transformFilter->Update();
	double* position = mainActor->GetPosition();
	mainActor->SetPosition(position[0]+x,position[1]+y,position[2]+z);
	ren->Render();
}

void Drawing::rotate(float x, float y, float z){
	cout << "Rotate: " << x << " - " << " - " << y << " - " << z << endl ;
	vtkSmartPointer<vtkTransform> transform1a = vtkSmartPointer<vtkTransform>::New();
	//transform1a->PostMultiply();
	//transform1a->RotateX(x);
	//transform1a->RotateY(y);
	//transform1a->RotateZ(z);
	//mainActor->SetUserTransform(transform1a);
	mainActor->RotateWXYZ(20,1,0,0);
	ren->Render();
}

void Drawing::rotateCamera(float x, float y, float z){
	vtkCamera* camera = ctxView->GetRenderer()->GetActiveCamera();

	camera->Yaw(x);
	camera->Pitch(y);
	camera->Roll(z);

	//camera->UpdateViewport();
}

void Drawing::defineClipping(){

	 // Define a clipping plane
	vtkSmartPointer<vtkPlane> clipPlane = vtkSmartPointer<vtkPlane>::New();
	clipPlane->SetNormal(1.0, 1.0, 0.0);
	clipPlane->SetOrigin(0.0, 0.0, 0.0);
	// Clip the source with the plane
	vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
	clipper->SetInputData(inputPolyData);
	clipper->SetClipFunction(clipPlane);  
	clipper->Update();
	//Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> superquadricMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	superquadricMapper->SetInputConnection(clipper->GetOutputPort());
	vtkSmartPointer<vtkActor> superquadricActor = vtkSmartPointer<vtkActor>::New();
	superquadricActor->SetMapper(superquadricMapper);
	vtkSmartPointer<vtkProperty> backFaces = vtkSmartPointer<vtkProperty>::New();
	backFaces->SetSpecular(0.0);
	backFaces->SetDiffuse(0.0);
	backFaces->SetAmbient(1.0);
	backFaces->SetAmbientColor(1.0000, 0.3882, 0.2784);
	superquadricActor->GetProperty()->SetColor(0,1,0);
	superquadricActor->SetBackfaceProperty(backFaces);
	// Create a renderer
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	//Add actors to the renderers
	renderer->AddActor(superquadricActor);
	win->AddRenderer(renderer);
	//win->Render();
}

/*int rotate_camera( vtkContextView* env){
	vtkCamera* cam = env->GetRenderer()->GetActiveCamera();

	float angle = 0;

	angle = msg.GetDataReal(8);
	cam->Yaw(angle);

	angle = msg.GetDataReal(4);
	cam->Pitch(angle);

	angle = msg.GetDataReal(0);
	cam->Roll(angle);


	//env.ren->ResetCameraClippingRange();
	//env.iren->GetRenderWindow()->StereoUpdate();
	//env.iren->Render();
	//env->Render();
	return 0;
}*/

void Drawing::read(){

	// Read all the data from the file
	vtkSmartPointer<vtkXMLPolyDataReader> reader =vtkSmartPointer<vtkXMLPolyDataReader>::New();
	reader->SetFileName(filename.c_str());
	reader->Update();
	inputPolyData = reader->GetOutput();

	cout << "File Found and Loaded : " << filename << endl ;

	vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
	translation->Translate(0.3, -0.05, 0);
	transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	//transformFilter->SetInputConnection(reader->GetOutputPort());
	transformFilter->SetInputData(inputPolyData);
	transformFilter->SetTransform(translation);
	//transformFilter->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(transformFilter->GetOutputPort());

	mainActor = vtkSmartPointer<vtkActor>::New();
	mainActor->SetMapper(mapper);

	ren->AddActor(mainActor);

	iren->SetRenderWindow(win);
	vtkInteractorStyleMultiTouchCamera *style =
	vtkInteractorStyleMultiTouchCamera::New();
	iren->SetInteractorStyle(style);
	
	//Start the event loop
	iren->Initialize();
	iren->Start();

	//defineClipping();
	win->PolygonSmoothingOn();
	win->Render();
	win->Start();

	ctxView->Render();

	
}

void Drawing::readOriginal(){

	std::string filename = BUNNY;
	// Read all the data from the file
	vtkSmartPointer<vtkXMLPolyDataReader> reader =vtkSmartPointer<vtkXMLPolyDataReader>::New();
	reader->SetFileName(filename.c_str());
	reader->Update();

	cout << "File Found and Loaded : " << filename << endl ;

	vtkContourFilter* contour = vtkContourFilter::New();
	contour->SetValue(0,0.26);
	contour->SetInputConnection(reader->GetOutputPort());
 
	// Visualize
	vtkLookupTable* lu = vtkLookupTable::New();
	lu->SetNumberOfTableValues(256);
	lu->SetHueRange(0.6667, 0);
	lu->SetTableRange(0.981, 1.01);
	lu->Build();
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(reader->GetOutputPort());
	mapper->SetLookupTable(lu);
	mapper->SetInterpolateScalarsBeforeMapping(1);
	mapper->SetScalarMode(VTK_SCALAR_MODE_USE_POINT_FIELD_DATA);
	mapper->SelectColorArray("RHO");
	mapper->SetUseLookupTableScalarRange(1);

	// Add ids to the points and cells of the sphere
	/*vtkSmartPointer<vtkIdFilter> idFilter = 
	vtkSmartPointer<vtkIdFilter>::New();
	idFilter->SetInputConnection(sphereSource->GetOutputPort());
	idFilter->Update();

	vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
	plane->SetOrigin(0, 0, 0);
	plane->SetNormal(1, 1, 1); // Convert the DataSet to a GenericDataSet
	vtkSmartPointer<vtkBridgeDataSet> bridgeDataSet = vtkSmartPointer<vtkBridgeDataSet>::New();
	bridgeDataSet->SetDataSet(idFilter->GetOutput());*/

	vtkSmartPointer<vtkGenericClip> clipper =
	vtkSmartPointer<vtkGenericClip>::New();
	//clipper->SetClipFunction(plane);
	//clipper->SetInputData(bridgeDataSet);
	//clipper->Update();


	mainActor = vtkSmartPointer<vtkActor>::New();
	mainActor->SetMapper(mapper);
 
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
 
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renderWindow);

	vtkInteractorStyleMultiTouchCamera *style =
	vtkInteractorStyleMultiTouchCamera::New();
	iren->SetInteractorStyle(style);

	vtkBoxWidget *boxWidget = vtkBoxWidget::New();
	boxWidget->SetInteractor(iren);
	boxWidget->SetPlaceFactor(1.25);
	vtkMyCallback *callback = vtkMyCallback::New();
	boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);
	boxWidget->On();

	renderer->AddActor(mainActor);
	renderer->SetBackground(0,0,0); // Background color green
 
	renderWindow->PolygonSmoothingOn();
	renderWindow->Render();
	renderWindowInteractor->Start();
}

void Drawing::dummy (){
	
	// Next we create an instance of vtkConeSource and set some of its
	// properties. The instance of vtkConeSource "cone" is part of a
	// visualization pipeline (it is a source process object); it produces data
	// (output type is vtkPolyData) which other filters may process.
	//
	cone = vtkConeSource::New();
	cone->SetHeight( 3.0 );
	cone->SetRadius( 1.0 );
	cone->SetResolution( 10 );

	//
	// In this example we terminate the pipeline with a mapper process object.
	// (Intermediate filters such as vtkShrinkPolyData could be inserted in
	// between the source and the mapper.)  We create an instance of
	// vtkPolyDataMapper to map the polygonal data into graphics primitives. We
	// connect the output of the cone souece to the input of this mapper.
	//
	mapper = vtkPolyDataMapper::New();
	mapper->SetInputConnection( cone->GetOutputPort() );

	//
	// Create an actor to represent the cone. The actor orchestrates rendering
	// of the mapper's graphics primitives. An actor also refers to properties
	// via a vtkProperty instance, and includes an internal transformation
	// matrix. We set this actor's mapper to be coneMapper which we created
	// above.
	//
	mainActor = vtkActor::New();
	mainActor->SetMapper( mapper );

	//
	// Create the Renderer and assign actors to it. A renderer is like a
	// viewport. It is part or all of a window on the screen and it is
	// responsible for drawing the actors it has.  We also set the background
	// color here.
	//
	ren = vtkRenderer::New();
	ren->AddActor( mainActor );
	ren->SetBackground( 0.1, 0.2, 0.4 );

	//
	// Finally we create the render window which will show up on the screen.
	// We put our renderer into the render window using AddRenderer. We also
	// set the size to be 300 pixels by 300.
	//
	win = vtkRenderWindow::New();
	win->AddRenderer( ren );
	win->SetSize( 300, 300 );

	//
	// The vtkRenderWindowInteractor class watches for events (e.g., keypress,
	// mouse) in the vtkRenderWindow. These events are translated into
	// event invocations that VTK understands (see VTK/Common/vtkCommand.h
	// for all events that VTK processes). Then observers of these VTK
	// events can process them as appropriate.
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(win);

	//
	// By default the vtkRenderWindowInteractor instantiates an instance
	// of vtkInteractorStyle. vtkInteractorStyle translates a set of events
	// it observes into operations on the camera, actors, and/or properties
	// in the vtkRenderWindow associated with the vtkRenderWinodwInteractor.
	// Here we specify a particular interactor style.
	vtkInteractorStyleMultiTouchCamera *style =
	vtkInteractorStyleMultiTouchCamera::New();
	iren->SetInteractorStyle(style);

	//
	// Here we use a vtkBoxWidget to transform the underlying coneActor (by
	// manipulating its transformation matrix). Many other types of widgets
	// are available for use, see the documentation for more details.
	//
	// The SetInteractor method is how 3D widgets are associated with the render
	// window interactor. Internally, SetInteractor sets up a bunch of callbacks
	// using the Command/Observer mechanism (AddObserver()). The place factor
	// controls the initial size of the widget with respect to the bounding box
	// of the input to the widget.
	/*vtkBoxWidget *boxWidget = vtkBoxWidget::New();
	boxWidget->SetInteractor(iren);
	boxWidget->SetPlaceFactor(1.25);
	vtkSphereWidget *sphereWidget = vtkSphereWidget::New();
	sphereWidget->SetInteractor(iren);
	sphereWidget->SetProp3D(mainActor);
	sphereWidget->PlaceWidget();
	vtkMyCallback *callback = vtkMyCallback::New();
	sphereWidget->AddObserver(vtkCommand::InteractionEvent, callback);

	//
	// Place the interactor initially. The input to a 3D widget is used to
	// initially position and scale the widget. The EndInteractionEvent is
	// observed which invokes the SelectPolygons callback.
	//
  

	//
	// Normally the user presses the "i" key to bring a 3D widget to life. Here
	// we will manually enable it so it appears with the cone.
	//
	//boxWidget->On();
	sphereWidget->On();

	//
	// Start the event loop.
	//
	*/
	iren->Initialize();
	iren->Start();

	//
	// Free up any objects we created. All instances in VTK are deleted by
	// using the Delete() method.
	//
	cone->Delete();
	mapper->Delete();
	mainActor->Delete();
	//callback->Delete();
	//boxWidget->Delete();
	ren->Delete();
	win->Delete();
	iren->Delete();
	style->Delete();

}

