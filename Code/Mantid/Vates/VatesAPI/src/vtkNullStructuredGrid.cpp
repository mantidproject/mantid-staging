#include "MantidVatesAPI/vtkNullStructuredGrid.h"

#include <vtkNew.h>
#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>
#include <iostream>
namespace Mantid {
namespace VATES {

/// Constructor
vtkNullStructuredGrid::vtkNullStructuredGrid() {}

/// Destructor
vtkNullStructuredGrid::~vtkNullStructuredGrid() {}

/**
 * Creates a default vtkDataSet.
 *@returns A pointer to the default vtkDataSet
 */
vtkStructuredGrid *vtkNullStructuredGrid::createNullData() {

  vtkStructuredGrid *dataSet = vtkStructuredGrid::New();
  dataSet->SetDimensions(1, 1, 1);
  vtkNew<vtkPoints> points;
  points->Allocate(1);
  points->InsertNextPoint(0.0, 0.0, 0.0);
  dataSet->SetPoints(points.GetPointer());

  vtkNew<vtkFloatArray> signal;
  signal->SetNumberOfComponents(1);
  signal->InsertNextTuple1(0.0);
  dataSet->GetPointData()->SetScalars(signal.GetPointer());

  dataSet->Squeeze();

  return dataSet;
}
}
}
