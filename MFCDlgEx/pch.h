#ifndef PCH_H
#define PCH_H

#include "framework.h"

#include <vtkAutoInit.h>
#define vtkRenderingCore_AUTOINIT \
4(vtkRenderingOpenGL2,vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingContextOpenGL2)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL2)

#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkConeSource.h>

#endif
