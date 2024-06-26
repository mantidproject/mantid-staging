# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2024 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
# pylint: disable=no-init,invalid-name

"""
@author Spencer Howells, ISIS
@date December 05, 2013
"""

import numpy as np

from mantid.api import IFunction1D, FunctionFactory
from scipy.special import spherical_jn  # bessel


class TeixeiraWaterIqt(IFunction1D):

    def category(self):
        return "QuasiElastic"

    def init(self):
        # Fitting parameters
        self.declareParameter("Amp", 1.00, "Amplitude")
        self.declareParameter("Tau1", 0.05, "Relaxation time")
        self.declareParameter("Gamma", 1.2, "Line Width")
        self.addConstraints("Tau1 > 0")
        # Non-fitting parameters
        self.declareAttribute("Q", 0.4)
        self.declareAttribute("a", 0.98)

    def setAttributeValue(self, name, value):
        if name == "Q":
            self.Q = value
        if name == "a":
            self.radius = value

    def function1D(self, xvals):
        amp = self.getParameterValue("Amp")
        tau1 = self.getParameterValue("Tau1")
        gamma = self.getParameterValue("Gamma")
        xvals = np.array(xvals)

        qr = np.array(self.Q * self.radius)
        j0 = spherical_jn(0, qr)
        j1 = spherical_jn(1, qr)
        j2 = spherical_jn(2, qr)
        with np.errstate(divide="ignore"):
            rotational = j0 * j0 + 3 * j1 * j1 * np.exp(-xvals / (3 * tau1)) + 5 * j2 * j2 * np.exp(-xvals / tau1)
            translational = np.exp(-gamma * xvals)
            iqt = amp * rotational * translational
        return iqt

    def functionDeriv1D(self, xvals, jacobian):
        amp = self.getParameterValue("Amp")
        tau1 = self.getParameterValue("Tau1")
        gamma = self.getParameterValue("Gamma")

        qr = np.array(self.Q * self.radius)
        j0 = spherical_jn(0, qr)
        j1 = spherical_jn(1, qr)
        j2 = spherical_jn(2, qr)
        with np.errstate(divide="ignore"):
            for i, x in enumerate(xvals, start=0):
                rotational = j0 * j0 + 3 * j1 * j1 * np.exp(-x / (3 * tau1)) + 5 * j2 * j2 * np.exp(-x / tau1)
                translational = np.exp(-gamma * x)
                partial_tau = (x / np.square(tau1)) * (j1 * j1 * np.exp(-x / (3 * tau1)) + 5 * j2 * j2 * np.exp(-x / tau1))
                jacobian.set(i, 0, rotational * translational)
                jacobian.set(i, 1, amp * rotational * partial_tau)
                jacobian.set(i, 2, -x * amp * rotational * translational)


# Required to have Mantid recognise the new function
FunctionFactory.subscribe(TeixeiraWaterIqt)
