// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2022 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once
#include "MantidCurveFitting/DllConfig.h"

#include <Eigen/Core>

namespace Mantid::CurveFitting {
typedef Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic> dynamic_stride;
typedef Eigen::Map<Eigen::MatrixXd, 0, dynamic_stride> map_type;
typedef Eigen::Map<const Eigen::MatrixXd, 0, dynamic_stride> const_map_type;

class MANTID_CURVEFITTING_DLL EigenMatrix_View {
public:
  // EigenMatrix_View Constructors
  // default constructor
  EigenMatrix_View();

  // constructor: array->matrix view
  EigenMatrix_View(double *base, const size_t nTotalRows, size_t nTotalCols, int nElements_1 = -1, int nElements_2 = -1,
                   const size_t &startElement_1 = 0, const size_t &startElement_2 = 0);

  // constructor: matrix->matrix view
  EigenMatrix_View(Eigen::MatrixXd &matrix, int nElements_1 = -1, int nElements_2 = -1,
                   const size_t &startElement_1 = 0, const size_t &startElement_2 = 0);

  // constructor: map->matrix view
  EigenMatrix_View(map_type &matrix, int nElements_1 = -1, int nElements_2 = -1, const size_t &startElement_1 = 0,
                   const size_t &startElement_2 = 0);

  // CONST constructor: array->matrix view
  EigenMatrix_View(const double *base, const size_t nTotalRows, size_t nTotalCols, int nElements_1 = -1,
                   int nElements_2 = -1, const size_t &startElement_1 = 0, const size_t &startElement_2 = 0);

  // CONST constructor: matrix->matrix view
  EigenMatrix_View(const Eigen::MatrixXd &matrix, int nElements_1 = -1, int nElements_2 = -1,
                   const size_t &startElement_1 = 0, const size_t &startElement_2 = 0);

  // CONST constructor: map->matrix view
  EigenMatrix_View(const map_type &matrix, int nElements_1 = -1, int nElements_2 = -1, const size_t &startElement_1 = 0,
                   const size_t &startElement_2 = 0);

  // copy constructor
  EigenMatrix_View(EigenMatrix_View &v);

  // CONST copy constructor
  EigenMatrix_View(const EigenMatrix_View &v);

  map_type &matrix_mutator();
  inline const map_type matrix_inspector() const { return m_view; };
  inline map_type matrix_copy() const { return m_view; };
  inline size_t rows() const { return m_view.rows(); }
  inline size_t cols() const { return m_view.cols(); };
  inline size_t outerStride() const { return m_view.outerStride(); }
  inline size_t innerStride() const { return m_view.innerStride(); }

  EigenMatrix_View &operator=(EigenMatrix_View &V);
  EigenMatrix_View &operator=(EigenMatrix_View &&V);

protected:
  map_type m_view;
  bool isConst = false;
};
} // namespace Mantid::CurveFitting