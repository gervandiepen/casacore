//# MArrayMathBase.h: Basic functions and classes for math on MArray objects
//# Copyright (C) 2012
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
//# $Id: MArrayMathBase.h 21262 2012-09-07 12:38:36Z gervandiepen $

#ifndef CASA_MARRAYMATHBASE_H
#define CASA_MARRAYMATHBASE_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/ArrayMathBase.h>

namespace casacore {

  // Forward declarations.
  template<typename T> class MArray;


  // <summary>
  // Basic functions and classes for math on MArray objects
  // </summary>
  //
  // <reviewed reviewer="UNKNOWN" date="" tests="tMArrayMath">
  //
  // <prerequisite>
  //   <li> <linkto class=MArray>MArray</linkto>
  // </prerequisite>
  //
  // <synopsis>
  // This header file defines several STL-like functions to work on
  // iterators with a mask.
  //
  // Furthermore, abstract base classes are defined for functors to be used
  // in functions like slidingXXX.
  // Virtual functions instead of templated functions are used to avoid
  // code bloat when used in functions like partialArrayMath. Because a
  // reduction operation usually takes much more time than the call, using
  // virtual functions hardly imposes a performance penalty.
  // </synopsis>


  // Define the base class for functors to perform a reduction function on an
  // MArray object. The functors themselves are defined elsewhere.
  template<typename T, typename RES=T> class MArrayFunctorBase {
  public:
    virtual ~MArrayFunctorBase() {}
    virtual RES operator() (const MArray<T>&) const = 0;
  };

  // </group>

} //# end namespace

#endif
