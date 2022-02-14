// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2021 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include <cxxtest/TestSuite.h>

#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/FunctionFactory.h"
#include "MantidCurveFitting/Functions/ActivationmeV.h"
#include "MantidCurveFitting/Jacobian.h"
#include "MantidFrameworkTestHelpers/WorkspaceCreationHelper.h"
#include "MantidKernel/PhysicalConstants.h"

using namespace Mantid::API;
using namespace Mantid::CurveFitting;
using namespace Mantid::CurveFitting::Functions;
using Mantid::MantidVec;

class ActivationmeVTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static ActivationmeVTest *createSuite() { return new ActivationmeVTest(); }
  static void destroySuite(ActivationmeVTest *suite) { delete suite; }

  void test_category() {
    ActivationmeV fn;
    TS_ASSERT_EQUALS(fn.category(), "Muon\\MuonModelling");
  }

  void test_function_parameter_settings() {
    auto activ = createTestActivationmeV();

    TS_ASSERT_THROWS(activ->setParameter("X", 1.0), const std::invalid_argument &);
    TS_ASSERT_THROWS(activ->setParameter("A9", 1.0), const std::invalid_argument &);
    TS_ASSERT_THROWS(activ->setAttributeValue("type", "thng"), const std::invalid_argument &);
  }

  void test_function_gives_expected_value_for_given_input() {
    auto activ = createTestActivationmeV();

    const double attemptRate = activ->getParameter("AttemptRate");
    const double barrier = activ->getParameter("Barrier");
    const double meVConv = Mantid::PhysicalConstants::meVtoKelvin;

    const std::size_t numPoints = 100;
    std::array<double, numPoints> xValues;
    std::iota(xValues.begin(), xValues.end(), 0);
    std::array<double, numPoints> yValues;
    activ->function1D(yValues.data(), xValues.data(), numPoints);

    for (size_t i = 0; i < numPoints; i++) {
      TS_ASSERT_DELTA(yValues[i], attemptRate * exp(-(meVConv * barrier) / xValues[i]), 1e-12);
    }
  }

  void test_jacobian_gives_expected_values() {
    auto activ = createTestActivationmeV();

    const size_t nData(1);
    std::vector<double> xValues(nData, 3.5);

    Mantid::CurveFitting::Jacobian jacobian(nData, 2);
    activ->functionDeriv1D(&jacobian, xValues.data(), nData);

    double dfdar = jacobian.get(0, 0);
    double dfdbarrier = jacobian.get(0, 1);

    TS_ASSERT_DELTA(dfdar, 0.0000017388, 1e-7);
    TS_ASSERT_DELTA(dfdbarrier, -0.000013260, 1e-7);
  }

private:
  class TestableActivationmeV : public ActivationmeV {
  public:
    void function1D(double *out, const double *xValues, const size_t nData) const override {
      ActivationmeV::function1D(out, xValues, nData);
    }
    void functionDeriv1D(Mantid::API::Jacobian *out, const double *xValues, const size_t nData) override {
      ActivationmeV::functionDeriv1D(out, xValues, nData);
    }
  };

  std::shared_ptr<TestableActivationmeV> createTestActivationmeV() {
    auto func = std::make_shared<TestableActivationmeV>();
    func->initialize();
    func->setParameter("AttemptRate", 2.3);
    func->setParameter("Barrier", 4.0);
    return func;
  }
};
