// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2024 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +

#pragma once

#include "MantidAlgorithms/PolarizationCorrections/SpinStateValidator.h"

#include <boost/algorithm/string/join.hpp>
#include <cxxtest/TestSuite.h>

using namespace Mantid::Algorithms;

class SpinStateValidatorTest : public CxxTest::TestSuite {
public:
  void testSingleCorrectInputs() {
    auto validator = std::make_shared<SpinStateValidator>(std::unordered_set<int>{1});
    auto correctInputs = std::vector<std::string>{"01", "00", "10", "11", " 01", " 00 ", "11 "};
    checkAllInputs(validator, correctInputs, true);
  }

  void testSingleIncorrectInputs() {
    auto validator = std::make_shared<SpinStateValidator>(std::unordered_set<int>{1});
    auto incorrectInputs = std::vector<std::string>{"0 1", "2", "01,10", "!", "001", "", " "};
    checkAllInputs(validator, incorrectInputs, false);
  }

  void testDuplicateEntry() {
    auto validator = std::make_shared<SpinStateValidator>(std::unordered_set<int>{2, 3});
    auto duplicates = std::vector<std::string>{"01, 01", "11,10,11", "00,00"};
    checkAllInputs(validator, duplicates, false);
  }

  void testMultipleStatesCorrectInputs() {
    auto validator = std::make_shared<SpinStateValidator>(std::unordered_set<int>{2, 3, 4});
    auto correctInputs = std::vector<std::string>{"01, 11", "00,10,11", "11,10, 00,01", "00, 10 "};
    checkAllInputs(validator, correctInputs, true);
  }

  void testAllFourSpinStateCombos() {
    auto validator = std::make_shared<SpinStateValidator>(std::unordered_set<int>{4});
    auto correctInputs = std::vector<std::string>();
    std::vector<std::string> initialSpinConfig{{"01", "11", "10", "00"}};
    std::sort(initialSpinConfig.begin(), initialSpinConfig.end());
    correctInputs.push_back(boost::algorithm::join(initialSpinConfig, ","));
    while (std::next_permutation(initialSpinConfig.begin(), initialSpinConfig.end())) {
      correctInputs.push_back(boost::algorithm::join(initialSpinConfig, ","));
    }
    checkAllInputs(validator, correctInputs, true);
  }

private:
  void checkAllInputs(const std::shared_ptr<SpinStateValidator> validator, const std::vector<std::string> &inputsToTest,
                      const bool shouldBeValid) {
    std::string result = "";
    for (const auto &input : inputsToTest) {
      result = validator->isValid(input);
      ASSERT_TRUE(result.empty() == shouldBeValid);
    }
  }
};