#include "DQMOffline/RecoB/interface/TagCorrelationPlotter.h"
#include "DQMOffline/RecoB/interface/Tools.h"

#include <string>

using namespace std;
using namespace RecoBTag;

TagCorrelationPlotter::TagCorrelationPlotter(const std::string& tagName1, const std::string& tagName2,
                                             const EtaPtBin& etaPtBin, const edm::ParameterSet& pSet,
                                             const unsigned int& mc, const bool doCTagPlots, const bool finalize, DQMStore::IBooker & ibook) :
					     BaseBTagPlotter(tagName2 + "_vs_" + tagName1, etaPtBin),
  					     lowerBound1_(pSet.getParameter<double>("Discr1Start")),
  					     lowerBound2_(pSet.getParameter<double>("Discr2Start")),
  					     upperBound1_(pSet.getParameter<double>("Discr1End")),
  					     upperBound2_(pSet.getParameter<double>("Discr2End")),
                                             nBinEffPur_(pSet.getParameter<int>("nBinEffPur")),
					     startEffPur_(pSet.getParameter<double>("startEffPur")),
					     endEffPur_(pSet.getParameter<double>("endEffPur")),
					     createProfile_(pSet.getParameter<bool>("CreateProfile")),
                                             fixedEff_(pSet.getParameter<vector<double>>("fixedEff")),
					     mcPlots_(mc), doCTagPlots_(doCTagPlots), finalize_(finalize)
  {
  if(finalize_) return;  
  correlationHisto_ = new FlavourHistograms2D<double, double>("correlation" + theExtensionString, tagName2 + " discr vs " + tagName1 + " discr",
                                                              102, lowerBound1_, upperBound1_, 102, lowerBound2_, upperBound2_, false, 
                                                              "TagCorrelation" + theExtensionString, mc, createProfile_, ibook);
  correlationHisto_->settitle(tagName1.c_str(), tagName2.c_str());
}

TagCorrelationPlotter::~TagCorrelationPlotter() {
  if(finalize_) return;
  delete correlationHisto_;
}

void TagCorrelationPlotter::epsPlot(const std::string & name)
{
  effPurFromHistos2D->epsPlot(name);
}

void TagCorrelationPlotter::analyzeTags(const reco::JetTag& jetTag1, const reco::JetTag& jetTag2, const int& jetFlavour, const float & w) {
  correlationHisto_->fill(jetFlavour, jetTag1.second, jetTag2.second, w);
}

void TagCorrelationPlotter::analyzeTags(const reco::JetTag& jetTag1, const reco::JetTag& jetTag2, const int& jetFlavour) {
  analyzeTags(jetTag1, jetTag2, jetFlavour, 1.0);
}

void TagCorrelationPlotter::analyzeTags(const float& discr1, const float& discr2, const int& jetFlavour, const float & w) {
  correlationHisto_->fill(jetFlavour, discr1, discr2, w);
}

void TagCorrelationPlotter::analyzeTags(const float& discr1, const float& discr2, const int& jetFlavour) {
  analyzeTags(discr1, discr2, jetFlavour, 1.0);
}

void TagCorrelationPlotter::finalize(DQMStore::IBooker & ibook_, DQMStore::IGetter & igetter_)
{

  correlationHisto_ = new FlavourHistograms2D<double, double>("correlation" + theExtensionString, " discr vs discr",
                                                              102, lowerBound1_, upperBound1_, 102, lowerBound2_, upperBound2_,
                                                              "TagCorrelation" + theExtensionString, mcPlots_, createProfile_, igetter_);

  effPurFromHistos2D = new EffPurFromHistos2D ( correlationHisto_, "TagCorrelation" + theExtensionString, mcPlots_, ibook_, 
						nBinEffPur_, startEffPur_, endEffPur_);
  effPurFromHistos2D->doCTagPlots(doCTagPlots_);
  effPurFromHistos2D->compute(ibook_, fixedEff_);  
}
