// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2022 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#include "ALFInstrumentPresenter.h"

#include "ALFAnalysisPresenter.h"
#include "ALFInstrumentModel.h"
#include "ALFInstrumentView.h"

#include "MantidAPI/FileFinder.h"

namespace MantidQt::CustomInterfaces {

ALFInstrumentPresenter::ALFInstrumentPresenter(IALFInstrumentView *view, std::unique_ptr<IALFInstrumentModel> model)
    : m_view(view), m_model(std::move(model)) {
  m_view->subscribePresenter(this);
  m_view->setUpInstrument(m_model->loadedWsName());
}

QWidget *ALFInstrumentPresenter::getLoadWidget() { return m_view->generateLoadWidget(); }

MantidWidgets::InstrumentWidget *ALFInstrumentPresenter::getInstrumentView() { return m_view->getInstrumentView(); }

void ALFInstrumentPresenter::subscribeAnalysisPresenter(IALFAnalysisPresenter *presenter) {
  m_analysisPresenter = presenter;
}

void ALFInstrumentPresenter::loadRunNumber() {
  auto const filepath = m_view->getFile();
  if (!filepath) {
    return;
  }

  m_analysisPresenter->clearTwoThetas();
  if (auto const message = loadAndTransform(*filepath)) {
    m_view->warningBox(*message);
  }
  m_view->setRunQuietly(std::to_string(m_model->runNumber()));
}

std::optional<std::string> ALFInstrumentPresenter::loadAndTransform(const std::string &pathToRun) {
  try {
    return m_model->loadAndTransform(pathToRun);
  } catch (std::exception const &ex) {
    return ex.what();
  }
}

void ALFInstrumentPresenter::extractSingleTube() {
  if (auto const twoTheta = m_model->extractSingleTube()) {
    m_analysisPresenter->notifyTubeExtracted(*twoTheta);
    m_analysisPresenter->notifyUpdateEstimateClicked();
  }
}

void ALFInstrumentPresenter::averageTube() {
  auto const numberOfTubes = m_analysisPresenter->numberOfTubes();
  if (auto const twoTheta = m_model->averageTube(numberOfTubes)) {
    m_analysisPresenter->notifyTubeAveraged(*twoTheta);
  }
}

bool ALFInstrumentPresenter::checkDataIsExtracted() const {
  return m_analysisPresenter->numberOfTubes() > 0u && m_model->checkDataIsExtracted();
}

std::string ALFInstrumentPresenter::extractedWsName() const { return m_model->extractedWsName(); }

} // namespace MantidQt::CustomInterfaces
