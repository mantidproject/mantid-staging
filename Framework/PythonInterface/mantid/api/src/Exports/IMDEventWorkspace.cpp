#include "MantidAPI/IMDEventWorkspace.h"
#include "MantidPythonInterface/kernel/GetPointer.h"
#include "MantidPythonInterface/kernel/Registry/RegisterWorkspacePtrToPython.h"
#include <boost/python/class.hpp>
#include <boost/python/register_ptr_to_python.hpp>

using namespace Mantid::API;
using Mantid::PythonInterface::Registry::RegisterWorkspacePtrToPython;
using namespace boost::python;

GET_POINTER_SPECIALIZATION(IMDEventWorkspace)

void export_IMDEventWorkspace() {
  // IMDEventWorkspace class
  class_<IMDEventWorkspace, bases<IMDWorkspace, MultipleExperimentInfos>,
         boost::noncopyable>("IMDEventWorkspace", no_init)
      .def("getNPoints", &IMDEventWorkspace::getNPoints, arg("self"),
           "Returns the total number of points (events) in this workspace")

      .def("getNumDims", &IMDEventWorkspace::getNumDims, arg("self"),
           "Returns the number of dimensions in this workspace")

      .def("getBoxController", (BoxController_sptr (IMDEventWorkspace::*)()) &
                                   IMDEventWorkspace::getBoxController,
           arg("self"), "Returns the BoxController used in this workspace")
      .def("setDisplayNormalization",
           &IMDEventWorkspace::setDisplayNormalization,
           (arg("self"), arg("normalization")),
           "Sets the visual normalization of"
           " the workspace.")
      .def("setDisplayNormalizationHisto",
           &IMDEventWorkspace::setDisplayNormalizationHisto,
           (arg("self"), arg("normalization")),
           "For :class:`~mantid.api.IMDEventWorkspace`s sets"
           " the visual normalization of dervied "
           ":class`~mantid.api.IMDHistoWorkspace`s.");

  RegisterWorkspacePtrToPython<IMDEventWorkspace>();
}
