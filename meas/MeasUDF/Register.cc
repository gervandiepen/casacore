//# Register.cc: Register Measure UDFs
//# Copyright (C) 2011
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <casacore/meas/MeasUDF/Register.h>
#include <casacore/meas/MeasUDF/PositionUDF.h>
#include <casacore/meas/MeasUDF/EpochUDF.h>
#include <casacore/meas/MeasUDF/DirectionUDF.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>
#include <ostream>
#include <sstream>

using namespace casacore;

void register_meas()
{
  // Register the TaQL Meas UDFs.
  // All of them should be shown in the showFuncs functions below.
  UDFBase::registerUDF ("meas.SHOW",          ShowMeasUDF::makeSHOW);
  UDFBase::registerUDF ("meas.POS",           PositionUDF::makePOS);
  UDFBase::registerUDF ("meas.POSITION",      PositionUDF::makePOS);
  UDFBase::registerUDF ("meas.ITRFXYZ",       PositionUDF::makeITRFXYZ);
  UDFBase::registerUDF ("meas.ITRFLL",        PositionUDF::makeITRFLL);
  UDFBase::registerUDF ("meas.ITRFLONLAT",    PositionUDF::makeITRFLL);
  UDFBase::registerUDF ("meas.ITRFH",         PositionUDF::makeITRFH);
  UDFBase::registerUDF ("meas.ITRFHEIGHT",    PositionUDF::makeITRFH);
  UDFBase::registerUDF ("meas.WGS",           PositionUDF::makeWGSXYZ);
  UDFBase::registerUDF ("meas.WGSXYZ",        PositionUDF::makeWGSXYZ);
  UDFBase::registerUDF ("meas.WGSLL",         PositionUDF::makeWGSLL);
  UDFBase::registerUDF ("meas.WGSLONLAT",     PositionUDF::makeWGSLL);
  UDFBase::registerUDF ("meas.WGSH",          PositionUDF::makeWGSH);
  UDFBase::registerUDF ("meas.WGSHEIGHT",     PositionUDF::makeWGSH);
  UDFBase::registerUDF ("meas.EPOCH",         EpochUDF::makeEPOCH);
  UDFBase::registerUDF ("meas.LAST",          EpochUDF::makeLAST);
  UDFBase::registerUDF ("meas.LST",           EpochUDF::makeLAST);
  UDFBase::registerUDF ("meas.DIR",           DirectionUDF::makeDIR);
  UDFBase::registerUDF ("meas.DIRECTION",     DirectionUDF::makeDIR);
  UDFBase::registerUDF ("meas.HADEC",         DirectionUDF::makeHADEC);
  UDFBase::registerUDF ("meas.AZEL",          DirectionUDF::makeAZEL);
  UDFBase::registerUDF ("meas.APP",           DirectionUDF::makeAPP);
  UDFBase::registerUDF ("meas.APPARENT",      DirectionUDF::makeAPP);
  UDFBase::registerUDF ("meas.J2000",         DirectionUDF::makeJ2000);
  UDFBase::registerUDF ("meas.B1950",         DirectionUDF::makeB1950);
  UDFBase::registerUDF ("meas.ECL",           DirectionUDF::makeECL);
  UDFBase::registerUDF ("meas.ECLIPTIC",      DirectionUDF::makeECL);
  UDFBase::registerUDF ("meas.GAL",           DirectionUDF::makeGAL);
  UDFBase::registerUDF ("meas.GALACTIC",      DirectionUDF::makeGAL);
  UDFBase::registerUDF ("meas.SGAL",          DirectionUDF::makeSGAL);
  UDFBase::registerUDF ("meas.SUPERGAL",      DirectionUDF::makeSGAL);
  UDFBase::registerUDF ("meas.SUPERGALACTIC", DirectionUDF::makeSGAL);
  UDFBase::registerUDF ("meas.ITRFD",         DirectionUDF::makeITRF);
  UDFBase::registerUDF ("meas.ITRFDIR",       DirectionUDF::makeITRF);
  UDFBase::registerUDF ("meas.ITRFDIRECTION", DirectionUDF::makeITRF);
  UDFBase::registerUDF ("meas.RISET",         DirectionUDF::makeRISESET);
  UDFBase::registerUDF ("meas.RISESET",       DirectionUDF::makeRISESET);
}


namespace casacore {

  void ShowMeasUDF::showFuncsEpoch (std::ostream& os)
  {
    os << std::endl << "Epoch conversion functions:" << std::endl;
    os << "  MEAS.EPOCH (type, epoch [,position])           convert to given type" << std::endl;
    os << "  MEAS.LAST (epoch, position)                    convert to local sidereal time" << std::endl;
    os << "       LST is a synonym for LAST" << std::endl;
  }

  void ShowMeasUDF::showFuncsPosition (std::ostream& os)
  {
    os << std::endl << "Position conversion functions:" << std::endl;
    os << "  MEAS.POS (type, position)                      convert to given type" << std::endl;
    os << "       POSITION is a synonym for POS" << std::endl;
    os << "  MEAS.ITRFXYZ (position)                        convert to ITRF XYZ coord" << std::endl;
    os << "  MEAS.ITRFLL (position)                         convert to ITRF LonLat" << std::endl;
    os << "       ITRFLONLAT is a synonym for ITRFLL" << std::endl;
    os << "  MEAS.ITRFH (position)                          convert to ITRF height" << std::endl;
    os << "       ITRFHEIGHT is a synonym for ITRFH" << std::endl;
    os << "  MEAS.WGS (position)                            convert to WGS84 XYZ coord" << std::endl;
    os << "       WGSXYZ is a synonym for WGS" << std::endl;
    os << "  MEAS.WGSLL (position)                          convert to WGS84 LonLat" << std::endl;
    os << "       WGSLONLAT is a synonym for WGSLL" << std::endl;
    os << "  MEAS.WGSH (position)                           convert to WGS84 height" << std::endl;
    os << "       WGSHEIGHT is a synonym for WGSH" << std::endl;
  }

  void ShowMeasUDF::showFuncsDirection (std::ostream& os)
  {
    os << std::endl << "Direction conversion functions:" << std::endl;
    os << "  MEAS.DIR (type, direction [,epoch, position])  convert to given type" << std::endl;
    os << "       DIRECTION is a synonym for DIR" << std::endl;
    os << "  MEAS.HADEC (direction, epoch, position)        convert to Hourangle/Decl" << std::endl;
    os << "  MEAS.AZEL (direction, epoch, position)         convert to Azimuth/Elevation" << std::endl;
    os << "  MEAS.APP (direction, epoch, position)          convert to apparent" << std::endl;
    os << "       APPARENT is a synonym for APP" << std::endl;
    os << "  MEAS.J2000 (direction [,epoch, position])      convert to J2000" << std::endl;
    os << "  MEAS.B1950 (direction [,epoch, position])      convert to B1950" << std::endl;
    os << "  MEAS.ECL (direction [,epoch, position])" << std::endl;
    os << "       ECLIPTIC is a synonym for ECL" << std::endl;
    os << "  MEAS.GAL (direction [,epoch, position])" << std::endl;
    os << "       GALACTIC is a synonym for GAL" << std::endl;
    os << "  MEAS.SGAL (direction [,epoch, position])" << std::endl;
    os << "       SUPERGAL is a synonym for SGAL" << std::endl;
    os << "       SUPERGALACTIC is a synonym for SGAL" << std::endl;
    os << "  MEAS.ITRFD (direction [,epoch, position])      convert to ITRF" << std::endl;
    os << "       ITRFDIR is a synonym for ITRFD" << std::endl;
    os << "       ITRFDIRECTION is a synonym for ITRFD" << std::endl;
    os << "  MEAS.RISET (direction, epoch, position)        get rise/set time" << std::endl;
    os << "       RISESET is a synonym for RISET" << std::endl;
  }


  UDFBase* ShowMeasUDF::makeSHOW (const String&)
    { return new ShowMeasUDF(); }

  void ShowMeasUDF::setup (const Table&, const TaQLStyle&)
  {
    AlwaysAssert (operands().size() <= 1, AipsError);
    if (operands().size() == 1) {
      AlwaysAssert (operands()[0]->dataType()  == TableExprNodeRep::NTString  &&
                    operands()[0]->valueType() == TableExprNodeRep::VTScalar,
                    AipsError);
    }
    // Set datatype, shape, unit, etc.
    setDataType (TableExprNodeRep::NTString);
    setNDim (0);                  // scalar
    setConstant (True);
  }

  String ShowMeasUDF::getString (const TableExprId& id)
  {
    std::ostringstream os;
    String type;
    if (operands().size() == 1) {
      type = operands()[0]->getString(id);
      type.downcase();
    }
    if (type.empty()  ||  type == "position"  ||  type == "pos") {
      showFuncsPosition (os);
    }
    if (type.empty()  ||  type == "epoch") {
      showFuncsEpoch (os);
    }
    if (type.empty()  ||  type == "direction"  ||  type == "dir") {
      showFuncsDirection (os);
    }
    if (os.str().empty()) {
      os << type
         << " is an unknown meastype; use pos(ition), epoch or dir(ection)"
         << std::endl;
    } else {
      os << std::endl << "See also section 'Special Measures functions'"
        " at http://casacore.github.io/casacore-notes/199.html"
         << std::endl << std::endl;
    }
    return os.str();
  }

}
