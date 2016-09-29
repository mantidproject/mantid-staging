#ifndef MANTID_MANTIDWIDGETS_MANTIDDISPLAYBASE_H_
#define MANTID_MANTIDWIDGETS_MANTIDDISPLAYBASE_H_

#include "MantidKernel/System.h"
#include <MantidAPI/AlgorithmObserver.h>
#include <MantidAPI/IAlgorithm_fwd.h>
#include <MantidQtAPI/DistributionOptions.h>
#include <MantidQtAPI/GraphOptions.h>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

template <class Key, class T> class QHash;
template <class Key, class T> class QMultiMap;
class QString;
class QStringList;
class Table;
class MultiLayer;
class MantidMatrix;

namespace MantidQt {
namespace MantidWidgets {

class MantidSurfacePlotDialog;
class MantidWSIndexDialog;

/**
\class  MantidBase
\brief  Contains display methods which will be used by
QWorkspaceDockView.
\author Lamar Moore
\date   24-08-2016
\version 1.0


Copyright &copy; 2016 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
National Laboratory & European Spallation Source

This file is part of Mantid.

Mantid is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Mantid is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

File change history is stored at: <https://github.com/mantidproject/mantid>
*/
class DLLExport MantidDisplayBase {
public:
  virtual ~MantidDisplayBase() = default;

  // Data display and saving methods
  virtual void updateRecentFilesList(const QString &fname) = 0;
  virtual void enableSaveNexus(const QString &wsName) = 0;
  virtual void disableSaveNexus() = 0;
  virtual void deleteWorkspaces(const QStringList &wsNames = QStringList()) = 0;
  virtual void importWorkspace() = 0;
  virtual MantidMatrix *
  importMatrixWorkspace(const Mantid::API::MatrixWorkspace_sptr workspace,
                        int lower = -1, int upper = -1,
                        bool showDlg = true) = 0;
  virtual void importWorkspace(const QString &wsName, bool showDlg = true,
                               bool makeVisible = true) = 0;
  virtual void renameWorkspace(QStringList = QStringList()) = 0;
  virtual void showMantidInstrumentSelected() = 0;
  virtual Table *createDetectorTable(const QString &wsName,
                                     const std::vector<int> &indices,
                                     bool include_data = false) = 0;
  virtual void importBoxDataTable() = 0;
  virtual void showListData() = 0;
  virtual void importTransposed() = 0;

  // Algorithm Display and Execution Methods
  virtual Mantid::API::IAlgorithm_sptr createAlgorithm(const QString &algName,
                                                       int version = -1) = 0;
  virtual void showAlgorithmDialog(const QString &algName,
                                   int version = -1) = 0;
  virtual void showAlgorithmDialog(const QString &algName,
                                   QHash<QString, QString> paramList,
                                   Mantid::API::AlgorithmObserver *obs = NULL,
                                   int version = -1) = 0;
  virtual void executeAlgorithm(Mantid::API::IAlgorithm_sptr alg) = 0;
  virtual bool executeAlgorithmAsync(Mantid::API::IAlgorithm_sptr alg,
                                     const bool wait = false) = 0;

  virtual Mantid::API::Workspace_const_sptr
  getWorkspace(const QString &workspaceName) = 0;

  virtual QWidget *getParent() = 0;

  // Plotting Methods
  virtual MultiLayer *
  plot1D(const QMultiMap<QString, std::set<int>> &toPlot, bool spectrumPlot,
         MantidQt::DistributionFlag distr = MantidQt::DistributionDefault,
         bool errs = false, MultiLayer *plotWindow = NULL,
         bool clearWindow = false, bool waterfallPlot = false) = 0;
  virtual void drawColorFillPlots(
      const QStringList &wsNames,
      GraphOptions::CurveType curveType = GraphOptions::ColorMap) = 0;
  virtual void showMDPlot() = 0;
  virtual void showSurfacePlot() = 0;
  virtual void showContourPlot() = 0;

  // Interface Methods
  virtual void showVatesSimpleInterface() = 0;
  virtual void showSpectrumViewer() = 0;
  virtual void showSliceViewer() = 0;
  virtual void showLogFileWindow() = 0;
  virtual void showSampleMaterialWindow() = 0;
  virtual void showAlgorithmHistory() = 0;

  virtual MantidSurfacePlotDialog *
  createSurfacePlotDialog(int flags, QStringList wsNames,
                          const QString &plotType) = 0;
  virtual MantidWSIndexDialog *createWorkspaceIndexDialog(int flags,
                                                          QStringList wsNames,
                                                          bool showWaterfall,
                                                          bool showPlotAll) = 0;
};
}
}
#endif // MANTID_MANTIDWIDGETS_MANTIDDISPLAYBASE_H_