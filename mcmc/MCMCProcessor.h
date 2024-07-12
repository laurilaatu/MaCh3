#pragma once

#ifndef _UNDEF_
#define _UNDEF_ 1234567890
#endif

// C++ includes
#include <complex>

// ROOT includes
#include "TObjArray.h"
#include "TObjString.h"
#include "TFile.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLegend.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TVectorD.h"
#include "TColor.h"
#include "TStyle.h"
#include "TStopwatch.h"
#include "TText.h"
#include "TGaxis.h"
#include "TTree.h"
#include "TROOT.h"
#include "TKey.h"
#include "TRandom3.h"
#include "TGraphPolar.h"
#include "TMath.h"
#include "TMatrixDSymEigen.h"

// MaCh3 includes
#include "mcmc/StatisticalUtils.h"

//KS: Joy of forward declaration https://gieseanw.wordpress.com/2018/02/25/the-joys-of-forward-declarations-results-from-the-real-world/
class TChain;

// TODO
// Apply reweighted weight to plotting and Bayes Factor
// 2D Reweighing like DayaBay
// Implement Diagnostics/GetPenaltyTerm.cpp here

/// KS: Enum for different covariance classes
enum ParameterEnum {
  kXSecPar  = 0, //KS: This hold both xsec and flux
  kNDPar = 1,
  kFDDetPar = 2,
  kOSCPar   = 3,
  
  kNParameterEnum = 4 //KS: keep it at the end to keep track of all parameters
};

/// @brief Class responsible for processing MCMC chains, performing diagnostics, generating plots, and managing Bayesian analysis.
/// @details This class provides utilities to handle MCMC output generated by mcmc::runMCMC. It is particularly useful for extracting values from previous MCMC runs and initiating new MCMC runs with those values. Inspired by nd280_utils/DrawComp.cpp.
/// @see For more details and examples, visit the [Wiki](https://github.com/mach3-software/MaCh3/wiki/09.-Bayesian-Analysis,-Plotting-and-MCMC-Processor).
class MCMCProcessor {
  public:
    /// @brief Constructs an MCMCProcessor object with the specified input file and options.
    /// @param InputFile The path to the input file containing MCMC data.
    /// @param MakePostfitCorr A boolean indicating whether to apply post-fit corrections during processing.
    MCMCProcessor(const std::string &InputFile, bool MakePostfitCorr);
    /// @brief Destroys the MCMCProcessor object.
    virtual ~MCMCProcessor();

    /// @brief Scan chain, what parameters we have and load information from covariance matrices
    void Initialise();
    /// @brief Make 1D projection for each parameter and prepare structure
    void MakePostfit();
    /// @brief Calculate covariance by making 2D projection of each combination of parameters
    void MakeCovariance();
    /// @brief KS:By caching each step we use multithreading
    void CacheSteps();
    /// @brief Calculate covariance by making 2D projection of each combination of parameters using multithreading
    /// @param Mute Allow silencing many messages, especially important if we calculate matrix many times
    void MakeCovariance_MP(const bool Mute = false);
    /// @brief Make and Draw SubOptimality
    void MakeSubOptimality(const int NIntervals = 10);

    /// @brief Reset 2D posteriors, in case we would like to calculate in again with different BurnInCut
    void ResetHistograms();
        
    /// @brief Draw the post-fit comparisons
    void DrawPostfit();
    /// @brief Make and Draw Violin
    void MakeViolin();
    /// @brief Make and Draw Credible intervals
    /// @param CredibleIntervals Vector with values of credible intervals, must be in descending order
    /// @param CredibleIntervalsColours Color_t telling what colour to use for each Interval line
    /// @param CredibleInSigmas Bool telling whether intervals are in percentage or in sigmas, then special conversions is used
    void MakeCredibleIntervals(const std::vector<double>& CredibleIntervals = {0.99, 0.90, 0.68 },
                               const std::vector<Color_t>& CredibleIntervalsColours = {kCyan+4, kCyan-2, kCyan-10},
                               bool CredibleInSigmas = false
                               );
    /// @brief Draw the post-fit covariances
    void DrawCovariance();
    /// @brief Make and Draw Credible Regions
    /// @param CredibleRegions Vector with values of credible intervals, must be in descending order
    /// @param CredibleRegionStyle Style_t telling what line style to use for each Interval line
    /// @param CredibleRegionColor Color_t telling what colour to use for each Interval line
    /// @param CredibleInSigmas Bool telling whether intervals are in percentage or in sigmas, then special conversions is used
    void MakeCredibleRegions(const std::vector<double>& CredibleRegions = {0.99, 0.90, 0.68},
                             const std::vector<Style_t>& CredibleRegionStyle = {kDashed, kSolid, kDotted},
                             const std::vector<Color_t>& CredibleRegionColor = {kGreen-3, kGreen-10, kGreen},
                             bool CredibleInSigmas = false
                             );
    /// @brief Make fancy triangle plot for selected parameters
    /// @param CredibleIntervals Vector with values of credible intervals, must be in descending order
    /// @param CredibleIntervalsColours Color_t telling what colour to use for each Interval line
    /// @param CredibleInSigmas Bool telling whether intervals are in percentage or in sigmas, then special conversions is used
    /// @param CredibleRegions Vector with values of credible intervals, must be in descending order
    /// @param CredibleRegionStyle Style_t telling what line style to use for each Interval line
    /// @param CredibleRegionColor Color_t telling what colour to use for each Interval line
    /// @param CredibleInSigmas Bool telling whether intervals are in percentage or in sigmas, then special conversions is used
    void MakeTrianglePlot(const std::vector<std::string>& ParNames,
                          // 1D
                          const std::vector<double>& CredibleIntervals = {0.99, 0.90, 0.68 },
                          const std::vector<Color_t>& CredibleIntervalsColours = {kCyan+4, kCyan-2, kCyan-10},
                          //2D
                          const std::vector<double>& CredibleRegions = {0.99, 0.90, 0.68},
                          const std::vector<Style_t>& CredibleRegionStyle = {kDashed, kSolid, kDotted},
                          const std::vector<Color_t>& CredibleRegionColor = {kGreen-3, kGreen-10, kGreen},
                          // Other
                          bool CredibleInSigmas = false
                          );
    /// @brief Make funny polar plot
    /// @param ParNames Vector with parameter names for which Polar Plot will be made
    void GetPolarPlot(const std::vector<std::string>& ParNames);

    /// @brief Calculate Bayes factor for vector of params, and model boundaries
    /// @param ParName Vector with parameter names for which we calculate Bayes factor
    /// @param Model1Bounds Lower and upper bound for hypothesis 1. Within this bound we calculate integral used later for Bayes Factor
    /// @param Model2Bounds Lower and upper bound for hypothesis 2. Within this bound we calculate integral used later for Bayes Factor
    /// @param ModelNames Names for hypothesis 1 and 2
    void GetBayesFactor(const std::vector<std::string>& ParName,
                        const std::vector<std::vector<double>>& Model1Bounds,
                        const std::vector<std::vector<double>>& Model2Bounds,
                        const std::vector<std::vector<std::string>>& ModelNames);
    /// @brief Calculate Bayes factor for point like hypothesis using SavageDickey
    void GetSavageDickey(const std::vector<std::string>& ParName,
                         const std::vector<double>& EvaluationPoint,
                         const std::vector<std::vector<double>>& Bounds);
    /// @brief Reweight Prior by giving new central value and new error
    /// @param ParName Parameter names for which we do reweighting
    /// @param NewCentral New central value for which we reweight
    /// @param NewError New error used for calculating weight
    void ReweightPrior(const std::vector<std::string>& Names,
                       const std::vector<double>& NewCentral,
                       const std::vector<double>& NewError);
    
    /// @brief KS: Perform MCMC diagnostic including Autocorrelation, Trace etc.
    void DiagMCMC();
    
    // Get the number of parameters
    /// @brief Get total number of used parameters
    inline int GetNParams() { return nDraw; };
    inline int GetNFlux() { return nFlux; };
    inline int GetNXSec() { return nParam[kXSecPar]; };
    inline int GetNND() { return nParam[kNDPar]; };
    inline int GetNFD() { return nParam[kFDDetPar]; };
    inline int GetOSC() { return nParam[kOSCPar]; };
        
    /// @brief Get 1D posterior for a given parameter
    /// @param i parameter index
    inline TH1D* GetHpost(const int i) { return hpost[i]; };
    /// @brief Get 2D posterior for a given parameter combination
    /// @param i parameter index X
    /// @param j parameter index Y
    inline TH2D* GetHpost2D(const int i, const int j) { return hpost2D[i][j]; };
    /// @brief Get Violin plot for all parameters with posterior values
    inline TH2D* GetViolin() { return hviolin; };
    /// @brief Get Violin plot for all parameters with prior values
    inline TH2D* GetViolinPrior() { return hviolin_prior; };

    //Covariance getters
    inline std::vector<std::string> const & GetXSecCov()  const { return CovPos[kXSecPar]; };
    inline std::string const & GetNDCov() const { return CovPos[kNDPar].back(); };
    inline std::string const & GetFDCov()    const { return CovPos[kFDDetPar].back(); };
    inline std::string const & GetOscCov()   const { return CovPos[kOSCPar].back(); };

    /// @brief Get the post-fit results (arithmetic and Gaussian)
    void GetPostfit(TVectorD *&Central, TVectorD *&Errors, TVectorD *&Central_Gauss, TVectorD *&Errors_Gauss, TVectorD *&Peaks);
    /// @brief Get the post-fit covariances and correlations
    /// @param Cov Covariance matrix
    /// @param Corr Correlation matrix
    void GetCovariance(TMatrixDSym *&Cov, TMatrixDSym *&Corr);
    /// @brief Or the individual post-fits
    void GetPostfit_Ind(TVectorD *&Central, TVectorD *&Errors, TVectorD *&Peaks, ParameterEnum kParam);
    
    /// @brief Get the vector of branch names from root file
    const std::vector<TString>& GetBranchNames() const { return BranchNames;};
    /// @brief Get properties of parameter by passing it number
    void GetNthParameter(const int param, double &Prior, double &PriorError, TString &Title);
    /// @brief Get parameter number based on name
    int GetParamIndexFromName(const std::string Name);
    /// @brief Get Number of entries that Chain has, for merged chains will not be the same Nsteps
    inline int GetnEntries(){return nEntries;};
    /// @brief Get Number of Steps that Chain has, for merged chains will not be the same nEntries
    inline int GetnSteps(){return nSteps;};
    
    /// @brief Set the step cutting by string
    /// @param Cuts string telling cut value
    void SetStepCut(const std::string& Cuts);
    /// @brief Set the step cutting by int
    /// @param Cuts integer telling cut value
    void SetStepCut(const int Cuts);

    /// @brief You can set relative to prior or relative to generated. It is advised to use relate to prior
    /// @param PlotOrNot bool controlling plotRelativeToPrior argument
    inline void SetPlotRelativeToPrior(const bool PlotOrNot){plotRelativeToPrior = PlotOrNot; };
    inline void SetPrintToPDF(const bool PlotOrNot){printToPDF = PlotOrNot; };
    /// @brief Set whether you want to plot error for parameters which have flat prior
    inline void SetPlotErrorForFlatPrior(const bool PlotOrNot){PlotFlatPrior = PlotOrNot; };
    inline void SetPlotBinValue(const bool PlotOrNot){plotBinValue = PlotOrNot; };
    inline void SetFancyNames(const bool PlotOrNot){FancyPlotNames = PlotOrNot; };
    /// @brief Set whether want to use smoothing for histograms using ROOT algorithm
    inline void SetSmoothing(const bool PlotOrNot){ApplySmoothing = PlotOrNot; };
    /// @brief Code will only plot 2D posteriors if Correlation are larger than defined threshold
    /// @param Threshold This threshold is compared with correlation value
    inline void SetPost2DPlotThreshold(const double Threshold){Post2DPlotThreshold = Threshold; };

    /// @brief Setter related what parameters we want to exclude from analysis, for example if cross-section parameters look like xsec_, then passing "xsec_" will
    /// @param Batches Vector with parameters type names we want to exclude
    inline void SetExcludedTypes(std::vector<std::string> Name){ExcludedTypes = Name; };
    inline void SetExcludedNames(std::vector<std::string> Name){ExcludedNames = Name; };

    /// @brief Set value of Nbatches used for batched mean, this need to be done earlier as batches are made when reading tree
    /// @param Batches Number of batches, default is 20
    inline void SetnBatches(const int Batches){nBatches = Batches; };
    inline void SetnLags(const int nLags){AutoCorrLag = nLags; };
    
    /// @brief Sett output suffix, this way jobs using the same file will have different names
    inline void SetOutputSuffix(const std::string Suffix){OutputSuffix = Suffix; };
    inline void SetPosterior1DCut(const std::string Cut){Posterior1DCut = Cut; };
  private:
    /// @brief Prepare prefit histogram for parameter overlay plot
    inline TH1D* MakePrefit();
    /// @brief prepare output root file and canvas to which we will save EVERYTHING
    inline void MakeOutputFile();
    /// @brief Draw 1D correlations which might be more helpful than looking at huge 2D Corr matrix
    inline void DrawCorrelations1D();

    /// @brief CW: Read the input Covariance matrix entries. Get stuff like parameter input errors, names, and so on
    inline void ReadInputCov();
    /// @brief Read the output MCMC file and find what inputs were used
    inline void FindInputFiles();
    /// @brief Read the xsec file and get the input central values and errors
    inline void ReadXSecFile();
    /// @brief Read the ND cov file and get the input central values and errors
    inline void ReadNDFile();
    /// @brief Read the FD cov file and get the input central values and errors
    inline void ReadFDFile();
    /// @brief Read the Osc cov file and get the input central values and errors
    inline void ReadOSCFile();
    /// @brief Remove parameter specified in config
    inline void RemoveParameters();
   
    /// @brief Scan Input etc.
    inline void ScanInput();
    /// @brief Scan order of params from a different groups
    inline void ScanParameterOrder();
    /// @brief Prepare all objects used for output
    inline void SetupOutput();

    //Analyse posterior distribution
    /// @brief Get Arithmetic mean from posterior
    /// @param hist histograms from which we extract arithmetic mean
    inline void GetArithmetic(TH1D * const hist, const int i);
    /// @brief Fit Gaussian to posterior
    /// @param hist histograms to which we fit gaussian
    inline void GetGaussian(TH1D *& hist, const int i);
    /// @brief Get Highest Posterior Density (HPD)
    /// @param hist histograms from which we HPD
    /// @param coverage What is defined coverage, by default 0.6827 (1 sigma)
    inline void GetHPD(TH1D * const hist, const int i, const double coverage = 0.6827);
    /// @brief Get 1D Credible Interval
    /// @param hist histograms based on which we calculate credible interval
    /// @param coverage What is defined coverage, by default 0.6827 (1 sigma)
    inline void GetCredibleInterval(TH1D* const hist, TH1D* hpost_copy, const double coverage = 0.6827);
    /// @brief Get 2D Credible Region
    /// @param hist2D histograms based on which we calculate credible regions
    /// @param coverage What is defined coverage, by default 0.6827 (1 sigma)
    inline void GetCredibleRegion(TH2D* hist2D, const double coverage = 0.6827);

    // MCMC Diagnostic
    /// @brief CW: Prepare branches etc. for DiagMCMC
    inline void PrepareDiagMCMC();
    /// @brief CW: Draw trace plots of the parameters i.e. parameter vs step
    inline void ParamTraces();
    /// @brief KS: Calculate autocorrelations supports both OpenMP and CUDA :)
    inline void AutoCorrelation();
    /// @brief KS: calc Effective Sample Size Following https://mc-stan.org/docs/2_18/reference-manual/effective-sample-size-section.html
    /// @param nLags Should be the same nLags as used in AutoCorrelation()
    ///
    /// This function computes the Effective Sample Size (ESS) using the autocorrelations
    /// calculated by AutoCorrelation(). Ensure that the parameter nLags here matches
    /// the number of lags used in AutoCorrelation() to obtain accurate results.
    inline void CalculateESS(const int nLags);
    /// @brief Get the batched means variance estimation and variable indicating if number of batches is sensible
    inline void BatchedAnalysis();
    /// @brief CW: Batched means, literally read from an array and chuck into TH1D
    inline void BatchedMeans();
    /// @brief Geweke Diagnostic based on https://www.math.arizona.edu/~piegorsch/675/GewekeDiagnostics.pdf
    inline void GewekeDiagnostic();
    /// @brief Acceptance Probability
    inline void AcceptanceProbabilities();
    /// @brief RC: Perform spectral analysis of MCMC based on http://arxiv.org/abs/astro-ph/0405462
    inline void PowerSpectrumAnalysis();

    /// Name of MCMC file
    std::string MCMCFile;
    /// Output file suffix useful when running over same file with different settings
    std::string OutputSuffix;
    /// Covariance matrix name position
    std::vector<std::vector<std::string>> CovPos;

    /// Main chain storing all steps etc
    TChain *Chain;
    /// BurnIn Cuts
    std::string StepCut;
    /// Cut used when making 1D Posterior distribution
    std::string Posterior1DCut;
    /// KS: Used only for SubOptimality
    int UpperCut;
    /// Value of burn in cut
    int BurnInCut;
    /// Number of branches in a TTree
    int nBranches;
    /// KS: For merged chains number of entries will be different from nSteps
    int nEntries;
    /// KS: For merged chains number of entries will be different from nSteps
    int nSteps;
    /// Number of sample PDF objects
    int nSamples;
    /// Number of covariance objects
    int nSysts;

    //Name of all branches as well as branches we don't want to include in the analysis
    std::vector<TString> BranchNames;
    std::vector<std::string> ExcludedTypes;
    std::vector<std::string> ExcludedNames;
    
    /// Number of all parameters used in the analysis
    int nDraw;
    
    /// Is the ith parameter varied
    std::vector<bool> IamVaried;
    /// Name of parameters which we are going to analyse
    std::vector<std::vector<TString>> ParamNames;
    /// Parameters central values which we are going to analyse
    std::vector<std::vector<double>>  ParamCentral;
    std::vector<std::vector<double>>  ParamNom;
    std::vector<std::vector<double>>  ParamErrors;
    std::vector<std::vector<bool>>    ParamFlat;
    /// Number of parameters per type
    std::vector<int> nParam;
    /// Make an enum for which class this parameter belongs to so we don't have to keep string comparing
    std::vector<ParameterEnum> ParamType;
    /// KS: in MCMC output there is order of parameters so for example first goes xsec then nd det etc.
    /// Idea is that this parameter will keep track of it so code is flexible
    std::vector<int> ParamTypeStartPos;
    
    /// In XsecMatrix we have both xsec and flux parameters, this is just for some plotting options
    std::vector<bool>   IsXsec; 
    /// This keep number of Flux params in xsec matrix
    int nFlux;

    /// Vector of each systematic
    std::vector<TString> SampleName_v;
    /// Vector of each sample PDF object
    std::vector<TString> SystName_v;
    
    /// Name of output files
    std::string OutputName;
    /// Name of canvas which help to save to the sample pdf
    TString CanvasName;

    //Plotting flags
    bool PlotXSec;
    bool PlotDet;
    /// whether we plot flat prior or not
    bool PlotFlatPrior;
    /// Will plot Jarlskog Invariant using information in the chain
    bool PlotJarlskog;
    
    //Even more flags
    /// Make correlation matrix or not
    bool MakeCorr;
    /// Whether we plot relative to prior or nominal, in most cases is prior
    bool plotRelativeToPrior;
    /// Sanity check if Postfit is already done to not make several times
    bool MadePostfit;
    /// Will plot all plot to PDF not only to root file
    bool printToPDF;
    /// Whether we want fancy plot names or not
    bool FancyPlotNames;
    /// If true it will print value on each bin of covariance matrix
    bool plotBinValue;
    /// Apply smoothing for 2D histos using root algorithm
    bool ApplySmoothing;
    /// KS: Set Threshold when to plot 2D posterior as by default we get a LOT of plots
    double Post2DPlotThreshold;

    std::vector< int > NDSamplesBins;
    std::vector< std::string > NDSamplesNames;

    /// Gaussian fitter
    TF1 *Gauss;

    /// The output file
    TFile *OutputFile;
    
    /// Fancy canvas used for our beautiful plots
    TCanvas *Posterior;

    //Vector of best fit points and errors obtained with different methods
    /// Vector with central value for each parameter
    TVectorD *Central_Value;
    /// Vector with mean values using Arithmetic Mean
    TVectorD *Means;
    /// Vector with errors values using RMS
    TVectorD *Errors;
    /// Vector with mean values using Gaussian fit
    TVectorD *Means_Gauss;
    /// Vector with error values using Gaussian fit
    TVectorD *Errors_Gauss;
    /// Vector with mean values using Highest Posterior Density
    TVectorD *Means_HPD;
    /// Vector with error values using Highest Posterior Density
    TVectorD *Errors_HPD; 
    /// Vector with positive error (right hand side) values using Highest Posterior Density
    TVectorD *Errors_HPD_Positive; 
    /// Vector with negative error (left hand side) values using Highest Posterior Density
    TVectorD *Errors_HPD_Negative; 

    /// Posterior Covariance Matrix
    TMatrixDSym *Covariance;
    /// Posterior Correlation Matrix
    TMatrixDSym *Correlation;

    /// Holds 1D Posterior Distributions
    TH1D **hpost;
    /// Holds 2D Posterior Distributions
    TH2D ***hpost2D;
    /// Holds violin plot for all dials
    TH2D *hviolin;
    /// Holds prior violin plot for all dials,
    TH2D *hviolin_prior;

    /// Array holding values for all parameters
    double** ParStep;
    /// Step number for step, important if chains were merged
    int* StepNumber;

    /// Number of bins
    int nBins;
    /// Drawrange for SetMaximum
    double DrawRange;
    
    /// MCMC Chain has been cached
    bool CacheMCMC;
    /// Doing MCMC Diagnostic
    bool doDiagMCMC;
    
    //Number of batches and LagL used in MCMC diagnostic
    /// Number of batches for Batched Mean
    int nBatches;
    /// LagL used in AutoCorrelation
    int AutoCorrLag;
    
    /// Total parameter sum for each param
    double *ParamSums;
    /// Values of batched average for every param and batch
    double **BatchedAverages;
    /// Value of LagL for each dial and each Lag
    double **LagL;

    /// Holds the sample values
    double **SampleValues;
    /// Holds the systs values
    double **SystValues;

    /// Holds all accProb
    double *AccProbValues;
    /// Holds all accProb in batches
    double *AccProbBatchedAverages;
    
  //Only if GPU is enabled
  #ifdef CUDA
    /// @brief Move stuff to GPU to perform auto correlation calculations there
    inline void PrepareGPU_AutoCorr(const int nLags);

    /// Value of each param that will be copied to GPU
    float* ParStep_cpu;
    float* NumeratorSum_cpu;
    float* ParamSums_cpu;
    float* DenomSum_cpu;

    /// Value of each param at GPU
    float* ParStep_gpu;
    float* NumeratorSum_gpu;
    float* ParamSums_gpu;
    float* DenomSum_gpu;
  #endif
};
