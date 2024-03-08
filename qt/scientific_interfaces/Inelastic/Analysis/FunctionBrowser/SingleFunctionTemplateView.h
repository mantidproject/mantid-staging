// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2019 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "Analysis/FunctionBrowser/FunctionTemplateView.h"
#include "DllConfig.h"
#include "SingleFunctionTemplatePresenter.h"

#include <QMap>
#include <QWidget>

class QtProperty;

namespace MantidQt {
namespace CustomInterfaces {
namespace IDA {

class IDAFunctionParameterEstimation;
/**
 * Class FunctionTemplateView implements QtPropertyBrowser to display
 * and set properties that can be used to generate a fit function.
 *
 */
class MANTIDQT_INELASTIC_DLL SingleFunctionTemplateView : public FunctionTemplateView {
  Q_OBJECT
public:
  explicit SingleFunctionTemplateView(QWidget *parent = nullptr);
  virtual ~SingleFunctionTemplateView() = default;

  void updateParameterNames(const QMap<int, std::string> &parameterNames) override;
  void setGlobalParametersQuiet(std::vector<std::string> const &globals) override;
  void clear() override;
  void setBackgroundA0(double) override;
  void setResolution(const std::vector<std::pair<std::string, size_t>> &) override;
  void setQValues(const std::vector<double> &) override;
  void addParameter(std::string const &parameterName, std::string const &parameterDescription);
  void setParameterValue(std::string const &parameterName, double parameterValue, double parameterError);
  void setParameterValueQuietly(std::string const &parameterName, double parameterValue, double parameterError);
  void setDataType(std::vector<std::string> const &allowedFunctionsList);
  void setEnumValue(int enumIndex);
  void updateAvailableFunctions(const std::map<std::string, std::string> &functionInitialisationStrings);

protected slots:
  void enumChanged(QtProperty *) override;
  void parameterChanged(QtProperty *) override;

private:
  void createProperties() override;

  QtProperty *m_fitType;
  QMap<std::string, QtProperty *> m_parameterMap;

private:
  friend class SingleFunctionTemplatePresenter;
};

} // namespace IDA
} // namespace CustomInterfaces
} // namespace MantidQt