# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
from mantid.simpleapi import (
    AddSampleLog,
    LoadWANDSCD,
)
import unittest
import numpy as np


class LoadWANDTest(unittest.TestCase):

    def test(self):
        LoadWANDTest_ws = LoadWANDSCD('HB2C_7000.nxs.h5,HB2C_7001.nxs.h5')
        self.assertTrue(LoadWANDTest_ws)
        self.assertEqual(LoadWANDTest_ws.getNumDims(), 3)
        self.assertEqual(LoadWANDTest_ws.getNPoints(), 1966080*2)
        self.assertEqual(LoadWANDTest_ws.getSignalArray().max(), 7)

        d0 = LoadWANDTest_ws.getDimension(0)
        self.assertEqual(d0.name, 'y')
        self.assertEqual(d0.getNBins(), 512)
        self.assertEqual(d0.getMinimum(), 0.5)
        self.assertEqual(d0.getMaximum(), 512.5)

        d1 = LoadWANDTest_ws.getDimension(1)
        self.assertEqual(d1.name, 'x')
        self.assertEqual(d1.getNBins(), 3840)
        self.assertEqual(d1.getMinimum(), 0.5)
        self.assertEqual(d1.getMaximum(), 3840.5)

        d2 = LoadWANDTest_ws.getDimension(2)
        self.assertEqual(d2.name, 'scanIndex')
        self.assertEqual(d2.getNBins(), 2)
        self.assertEqual(d2.getMinimum(), 0.5)
        self.assertEqual(d2.getMaximum(), 2.5)

        self.assertEqual(LoadWANDTest_ws.getNumExperimentInfo(), 1)
        self.assertEqual(LoadWANDTest_ws.getExperimentInfo(0).getInstrument().getName(), 'WAND')

        run = LoadWANDTest_ws.getExperimentInfo(0).run()
        s1 = run.getProperty('s1').value
        self.assertEqual(len(s1), 2)
        self.assertEqual(s1[0], -142.6)
        self.assertEqual(s1[1], -142.5)
        run_number = run.getProperty('run_number').value
        self.assertEqual(len(run_number), 2)
        self.assertEqual(run_number[0], 7000)
        self.assertEqual(run_number[1], 7001)
        monitor_count=run.getProperty('monitor_count').value
        self.assertEqual(len(monitor_count), 2)
        self.assertEqual(monitor_count[0], 907880)
        self.assertEqual(monitor_count[1], 908651)
        duration = run.getProperty('duration').value
        self.assertEqual(len(duration), 2)
        self.assertAlmostEqual(duration[0], 40.05, 5)
        self.assertAlmostEqual(duration[1], 40.05, 5)

        # test that the goniometer has been set correctly
        self.assertEqual(run.getNumGoniometers(), 2)
        self.assertAlmostEqual(run.getGoniometer(0).getEulerAngles('YZY')[0], -142.6) # s1 from HB2C_7000
        self.assertAlmostEqual(run.getGoniometer(1).getEulerAngles('YZY')[0], -142.5) # s1 from HB2C_7001

        LoadWANDTest_ws.delete()

    def test_withNorm(self):
        # create van data
        van = LoadWANDSCD('HB2C_7000.nxs.h5')
        van.setSignalArray(np.full_like(van.getSignalArray(), 25))
        van.setErrorSquaredArray(np.full_like(van.getSignalArray(), 25))
        AddSampleLog(van, LogName='time', LogText='42', LogType='Number Series', NumberType='Double')
        AddSampleLog(van, LogName='monitor', LogText='420', LogType='Number Series', NumberType='Double')
        #
        LoadWANDTest_ws = LoadWANDSCD(Filename='HB2C_7000.nxs.h5,HB2C_7001.nxs.h5', VanadiumWorkspace=van, NormalizedBy='Monitor')
        self.assertAlmostEqual(LoadWANDTest_ws.getSignalArray().max(), 7 / 25, 5)
        self.assertAlmostEqual(LoadWANDTest_ws.getErrorSquaredArray().max(), 0.014336, 5)

        van = LoadWANDSCD('HB2C_7000.nxs.h5')
        van.setSignalArray(np.full_like(van.getSignalArray(), 25))
        van.setErrorSquaredArray(np.full_like(van.getSignalArray(), 25))
        AddSampleLog(van, LogName='time', LogText='42', LogType='Number Series', NumberType='Double')
        AddSampleLog(van, LogName='monitor', LogText='420', LogType='Number Series', NumberType='Double')
        #
        LoadWANDTest_ws = LoadWANDSCD(Filename='HB2C_7000.nxs.h5,HB2C_7001.nxs.h5', VanadiumWorkspace=van, NormalizedBy='Counts')
        self.assertAlmostEqual(LoadWANDTest_ws.getSignalArray().max(), 0.01120, 5)
        self.assertAlmostEqual(LoadWANDTest_ws.getErrorSquaredArray().max(), 2.29376e-5, 5)

        van = LoadWANDSCD('HB2C_7000.nxs.h5')
        van.setSignalArray(np.full_like(van.getSignalArray(), 25))
        van.setErrorSquaredArray(np.full_like(van.getSignalArray(), 25))
        AddSampleLog(van, LogName='time', LogText='42', LogType='Number Series', NumberType='Double')
        AddSampleLog(van, LogName='monitor', LogText='420', LogType='Number Series', NumberType='Double')
        #
        LoadWANDTest_ws = LoadWANDSCD(Filename='HB2C_7000.nxs.h5,HB2C_7001.nxs.h5', VanadiumWorkspace=van, NormalizedBy='Time')
        self.assertAlmostEqual(LoadWANDTest_ws.getSignalArray().max(), 7 / 25, 5)
        self.assertAlmostEqual(LoadWANDTest_ws.getErrorSquaredArray().max(), 0.014336, 5)

        LoadWANDTest_ws.delete()


if __name__ == '__main__':
    unittest.main()
