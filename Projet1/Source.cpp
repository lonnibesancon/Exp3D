#include <iostream>

// First include the required header files for the VTK classes we are using.
#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)

#define JETLINES "jetLinesSmall2.vtp"
#define PARTICLES "particles.vtp"
#define BUNNY "bunny.vtp"

#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkCommand.h"
#include "vtkBoxWidget.h"
#include "vtkSphereWidget.h"
#include "vtkTransform.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkInteractorStyleTrackballActor.h"
#include "vtkInteractorStyleJoystickActor.h"
#include "vtkInteractorStyleMultiTouchCamera.h"
#include "vtkVolumeProperty.h"
#include "vtkStructuredPointsReader.h"

#include <vtkActor2D.h>
#include <vtkActorCollection.h>
#include <vtkAlgorithm.h>
#include <vtkCollection.h>
#include <vtkDataReader.h>
#include <vtkImageAlgorithm.h>
#include <vtkImageReader2.h>
#include <vtkMapper.h>
#include <vtkMoleculeReaderBase.h>
#include <vtkObject.h>
#include <vtkObjectBase.h>
#include <vtkProp3D.h>
#include <vtkRenderViewBase.h>
#include <vtkViewport.h>
#include <vtkVolumeMapper.h>
#include <vtkWindow.h>
#include <vtkXMLReader.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCallbackCommand.h>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkConeSource.h>
#include <vtkToolkits.h>
#include <vtkXMLHierarchicalBoxDataReader.h>
#include <vtkHierarchicalDataSetGeometryFilter.h>
#include <vtkShrinkPolyData.h>
#include <vtkHierarchicalPolyDataMapper.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkXMLMultiBlockDataReader.h>
#include <vtkOBJReader.h>
#include <vtkTestUtilities.h>
#include <vtkContextView.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkCubeSource.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkDataSetMapper.h>
#include <vtkBoundingBox.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTimerLog.h>
#include <vtkSmartPointer.h>
#include <vtkHAVSVolumeMapper.h>
#include <vtkStructuredPointsReader.h>
#include <vtkSLCReader.h>
#include <vtkStructuredPoints.h>
#include <vtkUnstructuredGrid.h>
#include <vtkThreshold.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>

//Protein ribbon
#include <vtkProteinRibbonFilter.h>

#include <vtkVolumeTextureMapper2D.h>
#include <vtkVolumeTextureMapper3D.h>

#include <vtkXMLPolyDataReader.h>
#include <vtkPDBReader.h>
#include <vtkPDBReader.h>
#include <vtkSphereSource.h>
#include <vtkTubeFilter.h>
#include <vtkGlyph3D.h>
#include <vtkLODActor.h>
#include <vtkNew.h>
#include <vtkMoleculeMapper.h>
#include <vtkLight.h>
#include <vtkRibbonFilter.h>
#include <vtkImageReader.h>
#include <vtkOutlineFilter.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
//Configuration of the multiple passes
#include <vtkCameraPass.h>
#include <vtkLightsPass.h>
#include <vtkOpaquePass.h>
#include <vtkRenderPassCollection.h>
#include <vtkSequencePass.h>
#include <vtkOpenGLRenderer.h>
#include <vtkImplicitPlaneRepresentation.h>

using namespace std ;




void setupClippingPlane()
{
	/*
	// Define a clipping plane
	clipPlane = vtkSmartPointer<vtkPlane>::New();
	clipPlane->SetNormal(0, 0, -1);

	vtkSmartPointer<vtkImageData> box = vtkSmartPointer<vtkImageData>::New();
	box->SetDimensions(2,2,2);
	int boxSize = 100000;
	double origin[3];
	clipPlane->GetOrigin(origin);
	origin[0] -= boxSize / 2;
	origin[1] -= boxSize / 2;
	origin[2] -= boxSize / 2;
	box->SetOrigin(origin);
	box->SetSpacing(boxSize,boxSize,boxSize);

	vtkSmartPointer<vtkOutlineFilter> outline = vtkSmartPointer<vtkOutlineFilter>::New();
	outline->SetInput(box);
	vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInput(box);
	cutter->SetCutFunction(clipPlane);


	//vtkSmartPointer<vtkPlaneSource> clipPlaneSource = vtkSmartPointer<vtkPlaneSource>::New();
	//clipPlaneSource->SetOrigin(0, 0, 60);
	//clipPlaneSource->SetPoint1(0,60,0);
	//clipPlaneSource->SetPoint2(60,0,60);
	// Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapperPlaneSource = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapperPlaneSource->SetInput(cutter->GetOutput());

	actorPlaneSource = vtkSmartPointer<vtkActor>::New();
	actorPlaneSource->SetMapper(mapperPlaneSource);
	//ren[0]->AddActor(actorPlaneSource);*/
}





