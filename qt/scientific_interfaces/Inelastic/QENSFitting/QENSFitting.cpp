// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#include "QENSFitting.h"
#include "Common/Settings.h"

#include "FitTab.h"
#include "TabFactory.h"

namespace MantidQt::CustomInterfaces::Inelastic {
DECLARE_SUBWINDOW(QENSFitting)

QENSFitting::QENSFitting(QWidget *parent)
    : IndirectInterface(parent), m_settingsGroup("CustomInterfaces/IndirectAnalysis/") {
  m_uiForm.setupUi(this);
  m_uiForm.pbSettings->setIcon(Settings::icon());

  auto const tabFactory = std::make_unique<TabFactory>(m_uiForm.twIDATabs);
  m_tabs.emplace(MSD_FIT, tabFactory->makeMSDFitTab(MSD_FIT));
  m_tabs.emplace(IQT_FIT, tabFactory->makeIqtFitTab(IQT_FIT));
  m_tabs.emplace(CONV_FIT, tabFactory->makeConvFitTab(CONV_FIT));
  m_tabs.emplace(FQ_FIT, tabFactory->makeFqFitTab(FQ_FIT));
}

void QENSFitting::applySettings(std::map<std::string, QVariant> const &settings) {
  for (auto tab = m_tabs.begin(); tab != m_tabs.end(); ++tab) {
    tab->second->setFileExtensionsByName(settings.at("RestrictInput").toBool());
  }
}

/**
 * Initialised the layout of the interface.  MUST be called.
 */
void QENSFitting::initLayout() {
  // Set up all tabs
  for (auto &tab : m_tabs) {
    tab.second->setupTab();
    connect(tab.second, SIGNAL(showMessageBox(const QString &)), this, SLOT(showMessageBox(const QString &)));
  }

  connect(m_uiForm.pbPythonExport, SIGNAL(clicked()), this, SLOT(exportTabPython()));
  connect(m_uiForm.pbSettings, SIGNAL(clicked()), this, SLOT(settings()));
  connect(m_uiForm.pbHelp, SIGNAL(clicked()), this, SLOT(help()));
  connect(m_uiForm.pbManageDirs, SIGNAL(clicked()), this, SLOT(manageUserDirectories()));

  IndirectInterface::initLayout();
}

std::string QENSFitting::documentationPage() const { return "Inelastic QENS Fitting"; }

/**
 * Handles exporting a Python script for the current tab.
 */
void QENSFitting::exportTabPython() {
  unsigned int currentTab = m_uiForm.twIDATabs->currentIndex();
  m_tabs[currentTab]->exportPythonScript();
}

} // namespace MantidQt::CustomInterfaces::Inelastic
