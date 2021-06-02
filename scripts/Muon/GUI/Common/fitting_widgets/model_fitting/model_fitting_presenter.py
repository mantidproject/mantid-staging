# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2021 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
from Muon.GUI.Common.fitting_widgets.basic_fitting.basic_fitting_presenter import BasicFittingPresenter
from Muon.GUI.Common.fitting_widgets.model_fitting.model_fitting_model import ModelFittingModel
from Muon.GUI.Common.fitting_widgets.model_fitting.model_fitting_view import ModelFittingView


class ModelFittingPresenter(BasicFittingPresenter):
    """
    The ModelFittingPresenter has a ModelFittingView and ModelFittingModel and derives from BasicFittingPresenter.
    """

    def __init__(self, view: ModelFittingView, model: ModelFittingModel):
        """Initialize the ModelFittingPresenter. Sets up the slots and event observers."""
        super(ModelFittingPresenter, self).__init__(view, model)

        self.view.set_slot_for_results_table_changed(self.handle_results_table_changed)
        self.view.set_slot_for_selected_x_changed(self.handle_selected_x_and_y_changed)
        self.view.set_slot_for_selected_y_changed(self.handle_selected_x_and_y_changed)

    def initialize_model_options(self) -> None:
        """Returns the fitting options to be used when initializing the model."""
        super().initialize_model_options()

    def handle_results_table_changed(self) -> None:
        """Handles when the selected results table has changed, and discovers the possible X's and Y's."""
        self.model.current_result_table_index = self.view.current_result_table_index

        self.model.discover_and_create_x_and_y_parameter_combinations()

        self.view.update_x_and_y_parameters(self.model.x_parameter_names, self.model.y_parameters_names)
        self.view.set_datasets_in_function_browser(self.model.dataset_names)
        self.view.update_dataset_name_combo_box(self.model.dataset_names)

        self.handle_selected_x_and_y_changed()

    def handle_selected_x_and_y_changed(self) -> None:
        """Handles when the selected X and Y parameters are changed."""
        dataset_name = self.view.x_parameter() + "_" + self.view.y_parameter()
        self.model.current_dataset_index = self.model.dataset_names.index(dataset_name)
        self.view.current_dataset_name = dataset_name

    def update_dataset_names_in_view_and_model(self) -> None:
        """Updates the results tables currently displayed."""
        self.model.result_table_names = self.model.get_workspace_names_to_display_from_context()
        # Triggers handle_results_table_changed
        self.view.update_result_table_names(self.model.result_table_names)
