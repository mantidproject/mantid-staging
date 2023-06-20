#include "MantidAlgorithms/WorkspaceBoundingBox.h"
#include "MantidAPI/MatrixWorkspace.h"

namespace Mantid::Algorithms {

namespace {
constexpr int HISTOGRAM_INDEX{0};
}

WorkspaceBoundingBox::WorkspaceBoundingBox(const API::MatrixWorkspace_const_sptr &workspace, const double beamRadius,
                                           const bool ignoreDirectBeam, const double cenX, const double cenY)
    : m_workspace(workspace), m_beamRadiusSq(beamRadius * beamRadius), m_ignoreDirectBeam(ignoreDirectBeam) {
  if (m_workspace->y(0).size() != 1)
    throw std::runtime_error("This object only works with integrated workspaces");

  m_spectrumInfo = &workspace->spectrumInfo();
  m_numSpectra = m_workspace->getNumberHistograms();

  this->setCenterPrev(cenX, cenY);
  this->initOverallRangeAndFindFirstCenter();
}

WorkspaceBoundingBox::WorkspaceBoundingBox() {
  m_spectrumInfo = nullptr; // certain functionality is not available
}

WorkspaceBoundingBox::~WorkspaceBoundingBox() = default;

Kernel::V3D WorkspaceBoundingBox::position(const std::size_t index) const {
  if (!m_spectrumInfo)
    throw std::runtime_error("SpectrumInfo object is not initialized");

  return m_spectrumInfo->position(index);
}

// find the min/max for x/y coords in set of valid spectra, update position of bounding box
void WorkspaceBoundingBox::initOverallRangeAndFindFirstCenter() {
  // reset temporary place for the calculation
  this->resetIntermediatePosition();
  double totalCount = 0;

  for (std::size_t i = 0; i < m_numSpectra; i++) {
    if (!isValidIndex(i))
      continue;

    updateMinMax(i);

    if (includeInIntegration(i))
      totalCount += updatePositionAndReturnCount(i);
  }
  this->normalizePosition(totalCount);
}

// In subsequent iterations check if spectra fits in the normalized bounding box(generated by previous iterations)
// If so, update position
void WorkspaceBoundingBox::findNewCenterPosition() {
  // reset temporary place for the calculation
  this->resetIntermediatePosition();
  double totalCount = 0;

  const auto &spectrumInfo = m_workspace->spectrumInfo();
  for (std::size_t i = 0; i < m_numSpectra; i++) {
    if (!isValidIndex(i))
      continue;

    const V3D position = spectrumInfo.position(i);

    if (this->symmetricRegionContainsPoint(position.X(), position.Y()) && includeInIntegration(position)) {
      totalCount += updatePositionAndReturnCount(i);
    }
  }
  this->normalizePosition(totalCount);
}

double WorkspaceBoundingBox::countsValue(const std::size_t index) const {
  return m_workspace->y(index)[HISTOGRAM_INDEX];
}

void WorkspaceBoundingBox::resetIntermediatePosition() {
  this->m_centerXPosCurr = 0.;
  this->m_centerYPosCurr = 0.;
}

void WorkspaceBoundingBox::setCenterPrev(const double x, const double y) {
  this->m_centerXPosPrev = x;
  this->m_centerYPosPrev = y;
}

/**
 * Update the symmetric (in x and y separately) range of space that is symmetric around the beam center
 */
void WorkspaceBoundingBox::setBounds(const double xMin, const double xMax, const double yMin, const double yMax) {
  this->m_xBoxMin = xMin;
  this->m_xBoxMax = xMax;
  this->m_yBoxMin = yMin;
  this->m_yBoxMax = yMax;
}

/** Performs checks on the spectrum located at index to determine if
 *  it is acceptable to be operated on
 *
 *  @param index :: index of spectrum data
 *  @return true/false if its valid
 */
bool WorkspaceBoundingBox::isValidIndex(const std::size_t index) const {
  if (!m_spectrumInfo)
    throw std::runtime_error("SpectrumInfo object is not initialized");
  if (!m_spectrumInfo->hasDetectors(index)) {
    g_log.warning() << "Workspace index " << index << " has no detector assigned to it - discarding\n";
    return false;
  }
  // Skip if we have a monitor or if the detector is masked.
  if (this->m_spectrumInfo->isMonitor(index) || this->m_spectrumInfo->isMasked(index))
    return false;

  // Get the current spectrum
  const auto YIn = this->countsValue(index);
  // Skip if NaN of inf
  if (std::isnan(YIn) || std::isinf(YIn))
    return false;
  return true;
}

/** Sets member variables x/y to new x/y based on
 *  spectrum info and historgram data at the given index
 *
 *  @param index :: index of spectrum data
 *  @return number of points of histogram data at index
 */
double WorkspaceBoundingBox::updatePositionAndReturnCount(const std::size_t index) {
  const auto counts = this->countsValue(index);
  const auto &position = this->position(index);

  this->m_centerXPosCurr += counts * position.X();
  this->m_centerYPosCurr += counts * position.Y();

  return counts;
}

/** Compare current mins and maxs to the coordinates of the spectrum at index
 *  expnd mins and maxs to include this spectrum
 *
 *  @param index :: index of spectrum data
 */
void WorkspaceBoundingBox::updateMinMax(const std::size_t index) {
  const auto &position = this->position(index);
  const double x = position.X();
  const double y = position.Y();

  this->m_xPosMin = std::min(x, this->m_xPosMin);
  this->m_xPosMax = std::max(x, this->m_xPosMax);
  this->m_yPosMin = std::min(y, this->m_yPosMin);
  this->m_yPosMax = std::max(y, this->m_yPosMax);
}

/** Checks to see if spectrum at index should be included in the integration
 *
 *  @param index :: index of spectrum data
 */
bool WorkspaceBoundingBox::includeInIntegration(const std::size_t index) {
  return this->includeInIntegration(this->position(index));
}

bool WorkspaceBoundingBox::includeInIntegration(const Kernel::V3D &position) {
  if (m_ignoreDirectBeam) {
    const double dx = position.X() - this->m_centerXPosPrev;
    const double dy = position.Y() - this->m_centerYPosPrev;

    return bool(dx * dx + dy * dy >= m_beamRadiusSq);
  }
  return true;
}

double WorkspaceBoundingBox::distanceFromPrevious() const {
  const auto xExtent = (m_centerXPosPrev - m_centerXPosCurr);
  const auto yExtent = (m_centerYPosPrev - m_centerYPosCurr);
  return sqrt(xExtent * xExtent + yExtent * yExtent);
}

/**
 * This only has effect if the integral is ignoring the beam center as a whole
 */
bool WorkspaceBoundingBox::centerOfMassWithinBeamCenter() {
  if (m_ignoreDirectBeam) {
    const double radiusX = this->calculateRadiusX();
    const double radiusY = this->calculateRadiusY();
    if (radiusX * radiusX <= m_beamRadiusSq || radiusY * radiusY <= m_beamRadiusSq) {
      return true;
    }
  }
  return false;
}

/**
 * Copy the current center to the previous and update the x/y range for overall integration
 */
void WorkspaceBoundingBox::prepareCenterCalculation() {
  this->setCenterPrev(m_centerXPosCurr, m_centerYPosCurr);

  const double radiusX = this->calculateRadiusX();
  const double radiusY = this->calculateRadiusY();
  this->setBounds(m_centerXPosCurr - radiusX, m_centerXPosCurr + radiusX, m_centerYPosCurr - radiusY,
                  m_centerYPosCurr + radiusY);
}

double WorkspaceBoundingBox::calculateRadiusX() const {
  return std::min((m_centerXPosCurr - m_xPosMin), (m_xPosMax - m_centerXPosCurr));
}

double WorkspaceBoundingBox::calculateRadiusY() const {
  return std::min((m_centerYPosCurr - m_yPosMin), (m_yPosMax - m_centerYPosCurr));
}

/** Perform normalization on x/y coords over given values
 *
 *  @param x :: value to normalize member x over
 *  @param y :: value to normalize member y over
 */
void WorkspaceBoundingBox::normalizePosition(const double totalCounts) {
  this->m_centerXPosCurr /= std::fabs(totalCounts);
  this->m_centerYPosCurr /= std::fabs(totalCounts);
}

/** Checks if a given x/y coord is within the bounding box
 *
 *  @param x :: x coordinate
 *  @param y :: y coordinate
 *  @return true/false if it is within the mins/maxs of the box
 */
bool WorkspaceBoundingBox::symmetricRegionContainsPoint(double x, double y) {
  return (x <= this->m_xBoxMax && x >= this->m_xBoxMin && y <= m_yBoxMax && y >= m_yBoxMin);
}

} // namespace Mantid::Algorithms
