# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2019 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
#  This file is part of the mantid workbench.
#
from __future__ import (absolute_import, division, print_function,
                        unicode_literals)

import matplotlib

matplotlib.use("AGG")

import matplotlib.pyplot as plt  # noqa: E402
from qtpy.QtWidgets import QMenu  # noqa: E402
from mantid.plots import MantidAxes  # noqa: E402
from mantid.simpleapi import CreateWorkspace  # noqa: E402
from mantidqt.utils.qt.testing import GuiTest  # noqa: E402
from workbench.plotting.figureerrorsmanager import FigureErrorsManager  # noqa: E402


class FigureErrorsManagerTest(GuiTest):
    """
    Test class that covers the interaction of the FigureErrorsManager with plots
    that use the mantid projection and have MantidAxes
    """

    @classmethod
    def setUpClass(cls):
        cls.ws2d_histo = CreateWorkspace(DataX=[10, 20, 30, 10, 20, 30, 10, 20, 30],
                                         DataY=[2, 3, 4, 5, 3, 5],
                                         DataE=[1, 2, 3, 4, 1, 1],
                                         NSpec=3,
                                         Distribution=True,
                                         UnitX='Wavelength',
                                         VerticalAxisUnit='DeltaE',
                                         VerticalAxisValues=[4, 6, 8],
                                         OutputWorkspace='ws2d_histo')
        # initialises the QApplication
        super(cls, FigureErrorsManagerTest).setUpClass()

    def setUp(self):
        self.fig, self.ax = plt.subplots(subplot_kw={'projection': 'mantid'})

        self.errors_manager = FigureErrorsManager(self.fig.canvas)

    def tearDown(self):
        plt.close('all')
        del self.fig
        del self.ax
        del self.errors_manager

    def test_add_error_bars_menu(self):
        main_menu = QMenu()
        self.errors_manager.add_error_bars_menu(main_menu)

        # Check the expected sub-menu with buttons is added
        added_menu = main_menu.children()[1]
        self.assertTrue(
            any(FigureErrorsManager.SHOW_ERROR_BARS_BUTTON_TEXT == child.text() for child in added_menu.children()))
        self.assertTrue(
            any(FigureErrorsManager.HIDE_ERROR_BARS_BUTTON_TEXT == child.text() for child in added_menu.children()))
        self.assertEqual(3, len(added_menu.children()))

    def test_show_error_bar_for(self):
        self.ax.plot(self.ws2d_histo, specNum=1)
        # assert plot does not have errors
        self.assertEqual(0, len(self.ax.containers))

        self.errors_manager.show_error_bar_for(0)

        # check that the errors have been added
        self.assertEqual(1, len(self.ax.containers))
        # check the errors have the correct line
        self.assertEqual(self.ax.lines[0], self.ax.containers[0][0])
        # check that the errors are visible
        self.assertTrue(self.ax.containers[0][2][0].get_visible())

        # Add an actual line with errors
        self.ax.errorbar(self.ws2d_histo, specNum=2, errors_visible=False)
        # should be hidden because of errors_visible=False
        self.assertFalse(self.ax.containers[1][2][0].get_visible())
        # so show it
        self.errors_manager.show_error_bar_for(1)
        # now it should be visible
        self.assertTrue(self.ax.containers[1][2][0].get_visible())

    def test_hide_error_bar_for(self):
        # default error plot shows the errors
        self.ax.errorbar(self.ws2d_histo, specNum=1)
        self.assertEqual(1, len(self.ax.containers))

        self.assertTrue(self.ax.containers[0][2][0].get_visible())

        self.errors_manager.hide_error_bar_for(0)
        # assert that the errors have been hidden
        self.assertFalse(self.ax.containers[0][2][0].get_visible())

        # Add an actual line with errors
        self.ax.errorbar(self.ws2d_histo, specNum=2, errors_visible=True)
        # should be visible because of errors_visible=True
        self.assertTrue(self.ax.containers[1][2][0].get_visible())
        # so hide it
        self.errors_manager.hide_error_bar_for(1)
        # now it should be hidden
        self.assertFalse(self.ax.containers[1][2][0].get_visible())

    def test_toggle_all_error_bars_make_visible_False_single_line(self):
        """
        Test checking that clicking `Hide all` does not try to
        toggle errors on a single line that does not have errors plotted.
        Instead it just skips it
        :return:
        """
        # add one errorbar with visible and one with hidden errors
        self.ax.plot(self.ws2d_histo, specNum=1)
        self.assertEqual(0, len(self.ax.containers))

        self.errors_manager.toggle_all_error_bars(make_visible=False)

        # nothing should be done as the state being forced is False,
        # which should not add new errors for lines
        self.assertEqual(0, len(self.ax.containers))

    def test_toggle_all_error_bars_make_visible_True_single_line(self):
        """
        Test checking that clicking `Show all` adds errors to the line.
        :return:
        """
        # add one errorbar with visible and one with hidden errors
        self.ax.plot(self.ws2d_histo, specNum=1)
        self.assertEqual(0, len(self.ax.containers))

        self.errors_manager.toggle_all_error_bars(make_visible=True)

        # a container for the error should have been added
        self.assertEqual(1, len(self.ax.containers))
        self.assertTrue(self.ax.containers[0][2][0].get_visible())

    def test_toggle_all_error_bars(self):
        # add one errorbar with visible and one with hidden errors
        self.ax.errorbar(self.ws2d_histo, specNum=1)
        self.ax.errorbar(self.ws2d_histo, specNum=2, errors_visible=False)
        # first is visible
        self.assertTrue(self.ax.containers[0][2][0].get_visible())
        # second is hidden
        self.assertFalse(self.ax.containers[1][2][0].get_visible())

        # toggle will now force all the states to visible
        self.errors_manager.toggle_all_error_bars(make_visible=True)

        self.assertTrue(self.ax.containers[0][2][0].get_visible())
        self.assertTrue(self.ax.containers[1][2][0].get_visible())

        # toggle will now force all the states to hidden
        self.errors_manager.toggle_all_error_bars(make_visible=False)
        self.assertFalse(self.ax.containers[0][2][0].get_visible())
        self.assertFalse(self.ax.containers[1][2][0].get_visible())

    def test_get_data_lines(self):
        self.ax.errorbar(self.ws2d_histo, specNum=1)
        data_lines = self.errors_manager.get_data_lines(self.ax)
        self.assertEqual(1, len(data_lines))

        self.ax.errorbar(self.ws2d_histo, specNum=2)
        data_lines = self.errors_manager.get_data_lines(self.ax)
        self.assertEqual(2, len(data_lines))

    def test_get_data_lines_keeps_order(self):
        self.ax.errorbar(self.ws2d_histo, specNum=1)
        self.ax.errorbar(self.ws2d_histo, specNum=2)
        self.ax.errorbar(self.ws2d_histo, specNum=3)

        data_lines = self.errors_manager.get_data_lines(self.ax)

        index = 0
        # iterate all lines
        for line in data_lines:
            # while the current line is not in the `lines` list, we keep moving forward
            # if the order is kept this will pass
            # if the order is changed, this will fail with an IndexError
            while line != self.ax.lines[index]:
                index += 1


class ScriptedPlotFigureErrorsManagerTest(GuiTest):
    """
    Test class that covers the interaction of the FigureErrorsManager with plots that
    do not use the MantidAxes, which happens if they are scripted.
    """

    @classmethod
    def setUpClass(cls):
        cls.ws2d_histo = CreateWorkspace(DataX=[10, 20, 30, 10, 20, 30, 10, 20, 30],
                                         DataY=[2, 3, 4, 5, 3, 5],
                                         DataE=[1, 2, 3, 4, 1, 1],
                                         NSpec=3,
                                         Distribution=True,
                                         UnitX='Wavelength',
                                         VerticalAxisUnit='DeltaE',
                                         VerticalAxisValues=[4, 6, 8],
                                         OutputWorkspace='ws2d_histo')
        # initialises the QApplication
        super(cls, ScriptedPlotFigureErrorsManagerTest).setUpClass()

    def setUp(self):
        self.fig, self.ax = plt.subplots()  # type: matplotlib.figure.Figure, MantidAxes

        self.errors_manager = FigureErrorsManager(self.fig.canvas)  # type: FigureErrorsManager

    def tearDown(self):
        plt.close('all')
        del self.fig
        del self.ax
        del self.errors_manager

    def test_context_menu_not_added_for_scripted_plot_without_errors(self):
        self.ax.plot([0, 15000], [0, 15000], label='MyLabel')
        self.ax.plot([0, 15000], [0, 14000], label='MyLabel 2')

        main_menu = QMenu()
        # QMenu always seems to have 1 child when empty,
        # but just making sure the count as expected at this point in the test
        self.assertEqual(1, len(main_menu.children()))

        # plot above doesn't have errors, nor is a MantidAxes
        # so no context menu will be added
        self.errors_manager.add_error_bars_menu(main_menu)

        # number of children should remain unchanged
        self.assertEqual(1, len(main_menu.children()))

    def test_scripted_plot_line_without_label_handled_properly(self):
        # having the special nolabel is usually present on lines with errors,
        # but sometimes can be present on lines without errors, this test covers that case
        self.ax.plot([0, 15000], [0, 15000], label=MantidAxes.MPL_NOLEGEND)
        self.ax.plot([0, 15000], [0, 15000], label=MantidAxes.MPL_NOLEGEND)

        main_menu = QMenu()
        # QMenu always seems to have 1 child when empty,
        # but just making sure the count as expected at this point in the test
        self.assertEqual(1, len(main_menu.children()))

        # plot above doesn't have errors, nor is a MantidAxes
        # so no context menu will be added
        self.errors_manager.add_error_bars_menu(main_menu)

        # number of children should remain unchanged
        self.assertEqual(1, len(main_menu.children()))

    def test_context_menu_added_for_scripted_plot_with_errors(self):
        self.ax.plot([0, 15000], [0, 15000], label='MyLabel')
        self.ax.errorbar([0, 15000], [0, 14000], yerr=[10, 10000], label='MyLabel 2')

        main_menu = QMenu()
        # QMenu always seems to have 1 child when empty,
        # but just making sure the count as expected at this point in the test
        self.assertEqual(1, len(main_menu.children()))

        # plot above doesn't have errors, nor is a MantidAxes
        # so no context menu will be added
        self.errors_manager.add_error_bars_menu(main_menu)

        added_menu = main_menu.children()[1]

        # actions should have been added now, which for this case are only `Show all` and `Hide all`
        self.assertTrue(
            any(FigureErrorsManager.SHOW_ERROR_BARS_BUTTON_TEXT == child.text() for child in added_menu.children()))
        self.assertTrue(
            any(FigureErrorsManager.HIDE_ERROR_BARS_BUTTON_TEXT == child.text() for child in added_menu.children()))
        self.assertEqual(2, len(main_menu.children()))

    def test_scripted_plot_hide_error_for(self):
        self.ax.plot([0, 15000], [0, 15000], label='MyLabel')
        self.ax.errorbar([0, 15000], [0, 14000], yerr=[10, 10000], label='MyLabel 2')

        self.assertTrue(self.ax.containers[0][2][0].get_visible())

        self.errors_manager.hide_error_bar_for(1)

        self.assertFalse(self.ax.containers[0][2][0].get_visible())

    def test_scripted_plot_show_error_for(self):
        self.ax.plot([0, 15000], [0, 15000], label='MyLabel')
        self.ax.errorbar([0, 15000], [0, 14000], yerr=[10, 10000], label='MyLabel 2')

        self.assertTrue(self.ax.containers[0][2][0].get_visible())

        self.errors_manager.hide_error_bar_for(1)

        self.assertFalse(self.ax.containers[0][2][0].get_visible())

        self.errors_manager.show_error_bar_for(1)

        self.assertTrue(self.ax.containers[0][2][0].get_visible())

    def test_scripted_plot_toggle_all(self):
        self.ax.plot([0, 15000], [0, 15000], label='MyLabel')
        self.ax.errorbar([0, 15000], [0, 14000], yerr=[10, 10000], label='MyLabel 2')

        self.assertTrue(self.ax.containers[0][2][0].get_visible())

        self.errors_manager.toggle_all_error_bars(make_visible=False)

        self.assertFalse(self.ax.containers[0][2][0].get_visible())

        self.errors_manager.toggle_all_error_bars(make_visible=True)

        self.assertTrue(self.ax.containers[0][2][0].get_visible())

    def test_scripted_plot_with_y_x_errors(self):
        self.ax.errorbar([0, 15000], [0, 14000], yerr=[10, 10000], label='MyLabel 2')
        self.ax.errorbar([0, 15000], [0, 14000], yerr=[10, 10000], xerr=[10, 10000], label='MyLabel 2')

        self.assertTrue(self.ax.containers[0][2][0].get_visible())
        # line with X Errors
        self.assertTrue(self.ax.containers[1][2][0].get_visible())

        self.errors_manager.toggle_all_error_bars(make_visible=False)

        self.assertFalse(self.ax.containers[0][2][0].get_visible())
        # line with X Errors is not affected by the toggle
        self.assertTrue(self.ax.containers[1][2][0].get_visible())

    def test_scripted_plot_with_only_x_errors(self):
        self.ax.errorbar([0, 15000], [0, 14000], yerr=[10, 10000], xerr=[10, 10000], label='MyLabel 2')
        self.assertTrue(self.ax.containers[0][2][0].get_visible())

        self.errors_manager.toggle_all_error_bars(make_visible=False)
        # state does not change, because the toggle does not affect X errors
        self.assertTrue(self.ax.containers[0][2][0].get_visible())

    def test_scripted_plot_show_unsupported_axis_raises_not_implemented(self):
        self.ax.plot([0, 15000], [0, 15000], label='MyLabel')

        self.assertRaisesRegexp(ValueError, FigureErrorsManager.AXES_NOT_MANTIDAXES_ERR_MESSAGE,
                                self.errors_manager.show_error_bar_for, 0)
