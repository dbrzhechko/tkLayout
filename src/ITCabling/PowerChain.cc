#include "ITCabling/PowerChain.hh"
#include "ITCabling/HvLine.hh"


PowerChain::PowerChain(const int powerChainId, const bool isPositiveZEnd, const bool isPositiveXSide, const std::string subDetectorName, const int layerDiskNumber, const int phiRef, const int ringQuarterIndex) :
  isPositiveZEnd_(isPositiveZEnd),
  isPositiveXSide_(isPositiveXSide),
  subDetectorName_(subDetectorName),
  layerDiskNumber_(layerDiskNumber),
  phiRef_(phiRef),
  ringQuarterIndex_(ringQuarterIndex)
{
  myid(powerChainId);
  isBarrel_ = inner_cabling_functions::isBarrel(subDetectorName);
  ringNumber_ = inner_cabling_functions::computeRingNumber(ringQuarterIndex);
  isRingInnerEnd_ = inner_cabling_functions::isRingInnerEnd(ringQuarterIndex);

  powerChainType_ = computePowerChainType(isBarrel_, layerDiskNumber, ringNumber_);

  plotColor_ = computePlotColor(isBarrel_, isPositiveZEnd, phiRef, ringQuarterIndex);

  // BUILD HVLINE, TO WHICH THE MODULES OF THE POWER CHAIN ARE ALL CONNECTED
  buildHvLine(powerChainId);
};


PowerChain::~PowerChain() {
  delete hvLine_;    // TO DO: switch to smart pointers and remove this!
  hvLine_ = nullptr;
}


void PowerChain::addModule(Module* m) { 
  modules_.push_back(m);
  hvLine_->addModule(m);
  m->setHvLine(hvLine_);
}




// TO DO: WOULD BE NICER TO COMPUTE THIS AS A FUNCTION OF MODULE TYPE (1x2 or 2x2)
const PowerChainType PowerChain::computePowerChainType(const bool isBarrel, const int layerDiskNumber, const int ringNumber) const {
  PowerChainType powerChainType = PowerChainType::IUNDEFINED;
  if (isBarrel) {
    if (layerDiskNumber <= 2) powerChainType = PowerChainType::I4A;
    else powerChainType = PowerChainType::I8A;
  }

  else {
    if (ringNumber <= 2) powerChainType = PowerChainType::I4A;
    else powerChainType = PowerChainType::I8A;
  }

  return powerChainType;
}



const int PowerChain::computePlotColor(const bool isBarrel, const bool isPositiveZEnd, const int phiRef, const int ringQuarterIndex) const {
  int plotColor = 0;

  const int plotPhi = femod(phiRef, 2);

  if (isBarrel) {
    const int plotZEnd = (isPositiveZEnd ? 0 : 1);
    plotColor = plotZEnd * 2 + plotPhi + 6;
  }
  else {
    const int plotRingQuarter = femod(ringQuarterIndex, 6);
    plotColor = plotRingQuarter * 2 + plotPhi + 1;
  }

  return plotColor;
}



/* Build HvLine asociated to the power chain.
 */
void PowerChain::buildHvLine(const int powerChainId) {
  std::string hvLineName = computeHvLineName(powerChainId);
  HvLine* hvLine = GeometryFactory::make<HvLine>(hvLineName);
  hvLine->setPowerChain(this);
  hvLine_ = hvLine;
}


/* Compute HvLine name.
 */
const std::string PowerChain::computeHvLineName(const int powerChainId) const {
  std::ostringstream hvLineNameStream;
  hvLineNameStream << powerChainId << "_HV";
  const std::string hvLineName = hvLineNameStream.str();
  return hvLineName;
}
