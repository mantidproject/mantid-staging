#ifndef WORKFLOWALGORITHMHELPERS_H_
#define WORKFLOWALGORITHMHELPERS_H_

#include <string>
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidKernel/EmptyValues.h"
#include "MantidKernel/PropertyManager.h"

namespace WorkflowAlgorithmHelpers
{
  using namespace Mantid;

  /// Function to get double property or instrument parameter value
  double getDblPropOrParam(const std::string &algProp,
      Kernel::PropertyManager_sptr &pm, const std::string &instParam,
      API::MatrixWorkspace_sptr &ws,
      const double overrideValue = Mantid::EMPTY_DBL());

  /// Function to get boolean property or instrument parameter value
  bool getBoolPropOrParam(const std::string &algProp,
      Kernel::PropertyManager_sptr &pm, const std::string &instParam,
      API::MatrixWorkspace_sptr &ws,
      const bool overrideValue = false);
}

#endif /* WORKFLOWALGORITHMHELPERS_H_ */
