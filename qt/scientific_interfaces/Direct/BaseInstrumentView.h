// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2014 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTIDQT_CUSTOMINTERFACES_BASEINSTRUMENTVIEW_H_
#define MANTIDQT_CUSTOMINTERFACES_BASEINSTRUMENTVIEW_H_

#include "DllConfig.h"
#include "MantidQtWidgets/Common/FunctionBrowser.h"
#include "MantidQtWidgets/Common/MWRunFiles.h"
#include "MantidQtWidgets/Common/ObserverPattern.h"
#include "MantidQtWidgets/InstrumentView/InstrumentWidget.h"
#include "MantidQtWidgets/Plotting/PreviewPlot.h"

#include <QObject>
#include <QSplitter>
#include <QString>
#include <string>
#include <QPushButton>

namespace MantidQt {
namespace CustomInterfaces {

class BaseInstrumentView : public QSplitter {
  Q_OBJECT

public:
  explicit BaseInstrumentView(const std::string &instrument,
                              QWidget *parent = nullptr);
  std::string getFile();
  void setRunQuietly(const std::string &runNumber);
  void observeLoadRun(Observer *listener) {
    m_loadRunObservable->attach(listener);
  };
  void warningBox(const std::string &message);
  void setInstrumentWidget(MantidWidgets::InstrumentWidget *instrument) {
    m_instrumentWidget = instrument;
  };
  MantidWidgets::InstrumentWidget *getInstrumentView() {
    return m_instrumentWidget;
  };
  virtual void
  setUpInstrument(const std::string fileName,
                  std::vector<std::function<bool(std::map<std::string, bool>)>>
                      &instrument);
  virtual void addObserver(std::tuple<std::string, Observer *> &listener){};
  void setupInstrumentPlotFitSplitters();

public slots:
  void fileLoaded();

protected:
  MantidWidgets::PreviewPlot *m_plot;
  MantidWidgets::FunctionBrowser *m_fitBrowser;
  QLineEdit *m_start, *m_end;

private:
  QWidget *generateLoadWidget();
  void warningBox(const QString &message);
  void setupPlotFitSplitter();
  Observable *m_loadRunObservable;
  API::MWRunFiles *m_files;
  QString m_instrument;
  MantidWidgets::InstrumentWidget *m_instrumentWidget;
  QSplitter *m_fitPlotLayout;
  QPushButton *m_fitButton;
};
} // namespace CustomInterfaces
} // namespace MantidQt

#endif /* MANTIDQT_CUSTOMINTERFACES_BASEINSTRUMENTVIEW_H_ */
