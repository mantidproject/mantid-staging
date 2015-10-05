from mantid.simpleapi import *
from mantid.api import *
from mantid.kernel import Direction, FloatArrayProperty
import mantid

COMPRESS_TOL_TOF = .01


class PDToPDFgetN(DataProcessorAlgorithm):

    def category(self):
        return "Workflow\\Diffraction;PythonAlgorithms"

    def name(self):
        return "PDToPDFgetN"

    def summary(self):
        return "The algorithm used converting raw data to pdfgetn input files"

    def PyInit(self):
        self.declareProperty(FileProperty(name="Filename",
                                          defaultValue="", action=FileAction.Load,
                                          extensions=["_event.nxs", ".nxs.h5"]),
                             "Event file")
        self.declareProperty("MaxChunkSize", 0.0,
                             "Specify maximum Gbytes of file to read in one chunk.  Default is whole file.")
        self.declareProperty("FilterBadPulses", 95.,
                             doc="Filter out events measured while proton " +
                             "charge is more than 5% below average")

        self.declareProperty(MatrixWorkspaceProperty("InputWorkspace", "",
                                                     direction=Direction.Input,
                                                     optional=PropertyMode.Optional),
                             doc="Handle to reduced workspace")

        self.declareProperty(FileProperty(name="CalibrationFile",
                                          defaultValue="", action=FileAction.OptionalLoad,
                                          extensions=[".h5", ".hd5", ".hdf", ".cal"]))
        self.declareProperty(FileProperty(name="CharacterizationRunsFile", defaultValue="",
                                          action=FileAction.OptionalLoad,
                                          extensions=["txt"]),
                             "File with characterization runs denoted")

        self.declareProperty("RemovePromptPulseWidth", 0.0,
                             "Width of events (in microseconds) near the prompt pulse to remove. 0 disables")
        self.declareProperty("CropWavelengthMin", 0.,
                             "Crop the data at this minimum wavelength.")
        self.declareProperty("CropWavelengthMax", 0.,
                             "Crop the data at this maximum wavelength.")

        self.declareProperty(FloatArrayProperty("Binning", values=[0., 0., 0.],
                             direction=Direction.Input), "Positive is linear bins, negative is logorithmic")
        self.declareProperty("ResampleX", 0,
                             "Number of bins in x-axis. Non-zero value overrides \"Params\" property. " +
                             "Negative value means logorithmic binning.")

        self.declareProperty(MatrixWorkspaceProperty("OutputWorkspace", "",
                                                     direction=Direction.Output),
                             doc="Handle to reduced workspace")
        self.declareProperty(FileProperty(name="PDFgetNFile", defaultValue="", action=FileAction.Save,
                                          extensions=[".getn"]), "Output filename")

    def _loadCharacterizations(self):
        self._focusPos = {}
        self._iparmFile = None

        charFilename = self.getProperty("CharacterizationRunsFile").value

        if charFilename is None or len(charFilename) <= 0:
            return

        results = PDLoadCharacterizations(Filename=charFilename,
                                          OutputWorkspace="characterizations")
        self._iparmFile = results[1]
        self._focusPos['PrimaryFlightPath'] = results[2]
        self._focusPos['SpectrumIDs'] = results[3]
        self._focusPos['L2'] = results[4]
        self._focusPos['Polar'] = results[5]
        self._focusPos['Azimuthal'] = results[6]

    def PyExec(self):
        filename = self.getProperty("Filename").value

        self._loadCharacterizations()

        wksp = LoadEventAndCompress(Filename=self.getProperty("Filename").value,
                                    OutputWorkspace=self.getPropertyValue("OutputWorkspace"),
                                    MaxChunkSize=self.getProperty("MaxChunkSize").value,
                                    FilterBadPulses=self.getProperty("FilterBadPulses").value,
                                    CompressTOFTolerance=COMPRESS_TOL_TOF)

        charac = ""
        if mtd.doesExist("characterizations"):
            charac = "characterizations"

        # get the correct row of the table
        PDDetermineCharacterizations(InputWorkspace=wksp,
                                     Characterizations=charac,
                                     ReductionProperties="__snspowderreduction")

        wksp = AlignAndFocusPowder(InputWorkspace=wksp, OutputWorkspace=wksp,
                                   CalFileName=self.getProperty("CalibrationFile").value,
                                   Params=self.getProperty("Binning").value,
                                   ResampleX=self.getProperty("ResampleX").value, Dspacing=True,
                                   PreserveEvents=False,
                                   RemovePromptPulseWidth=self.getProperty("RemovePromptPulseWidth").value,
                                   CompressTolerance=COMPRESS_TOL_TOF,
                                   CropWavelengthMin=self.getProperty("CropWavelengthMin").value,
                                   CropWavelengthMax=self.getProperty("CropWavelengthMax").value,
                                   ReductionProperties="__snspowderreduction",
                                   **(self._focusPos))
        wksp = NormaliseByCurrent(InputWorkspace=wksp, OutputWorkspace=wksp)
        wksp.getRun()['gsas_monitor'] = 1
        if self._iparmFile is not None:
            wksp.getRun()['iparm_file'] = self._iparmFile

        wksp = SetUncertainties(InputWorkspace=wksp, OutputWorkspace=wksp,
                                SetError="sqrt")
        SaveGSS(InputWorkspace=wksp,
                Filename=self.getProperty("PDFgetNFile").value,
                SplitFiles=False, Append=False,
                MultiplyByBinWidth=False,
                Bank=mantid.pmds["__snspowderreduction"]["bank"].value,
                Format="SLOG", ExtendedHeader=True)

        self.setProperty("OutputWorkspace", wksp)

# Register algorithm with Mantid.
AlgorithmFactory.subscribe(PDToPDFgetN)
