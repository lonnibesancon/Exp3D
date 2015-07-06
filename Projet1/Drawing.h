#ifndef DRAWING
#define DRAWING
#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)
#define vtkRenderingContext2D_AUTOINIT 1(vtkRenderingContextOpenGL)


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
#include <vtkIdFilter.h>
#include <vtkBridgeDataSet.h>
#include <vtkGenericClip.h>
//Configuration of the multiple passes
#include <vtkCameraPass.h>
#include <vtkLightsPass.h>
#include <vtkOpaquePass.h>
#include <vtkRenderPassCollection.h>
#include <vtkSequencePass.h>
#include <vtkOpenGLRenderer.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkTransformFilter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkMath.h>
#include <vtkVector.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkClipPolyData.h>

class Drawing
{
public:
	Drawing(void);
	~Drawing(void);

	/*Drawing related functions*/
	void rotate(float x, float y, float z);
	void translate(float x, float y, float z);
	void scale(float k);
	void rotateCamera(float x, float y, float z);
	void read();
	void readOriginal();
	void dummy();
	void defineClipping();

	/*Others*/
	void setFileName(std::string);
	


	vtkSmartPointer<vtkPlane> clipPlane;
	vtkSmartPointer<vtkImplicitPlaneRepresentation> planeRep;
	vtkSmartPointer<vtkActor> actorPlaneSource;
	vtkSmartPointer<vtkActor> mainActor;

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter;
	vtkSmartPointer<vtkTransform> translation ;
	vtkContextView* ctxView ;
	vtkRenderWindow* win ;
	vtkRenderer* ren ;
	vtkCamera* cam ;
	vtkRenderWindowInteractor *iren;
	vtkSmartPointer<vtkPolyData> inputPolyData;

protected:

	std::string filename ;

};

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback *New()
    { return new vtkMyCallback; }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
	  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
	  matrix->Identity();
	  matrix->SetElement(0,0,2);
	  matrix->SetElement(1,1,2);
	  matrix->SetElement(2,2,2);
	  matrix->SetElement(0,1,0.6);
	  matrix->SetElement(0,2,0.001);
	  matrix->SetElement(1,0,-0.6);
	  matrix->SetElement(1,2,0.02);
	  matrix->SetElement(2,0,-0.015);
	  matrix->SetElement(2,1,0.015);
      vtkTransform *t = vtkTransform::New();
      vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
      //int translation = widget->Get
      widget->GetProp3D()->SetUserMatrix(matrix);
      //t->Delete();
    }
};
#endif

