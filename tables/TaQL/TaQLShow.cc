//# TaQLShow.cc: Class to get various TaQL-related info
//# Copyright (C) 2016
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


//# Includes
#include <casacore/tables/TaQL/TaQLShow.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include <casacore/tables/TaQL/ExprNodeSet.h>
#include <casacore/casa/Quanta/UnitMap.h>
#include <casacore/casa/Exceptions/Error.h>
#include <ostream>
#include <sstream>

using namespace std;

namespace casacore { //# NAMESPACE CASACORE - BEGIN

  String TaQLShow::getInfo (const Vector<String>& parts,
                            const TaQLStyle& style)
  {
    std::ostringstream os;
    if (parts.empty()) {
      os << "Possible show/help commands:" << endl;
      os << "  show table tablename             shows table information (a la showtable)" << endl;
      os << "  show syntax [command]            shows the syntax of TaQL commands" << endl;
      os << "  show operators                   shows available operators" << endl;
      os << "  show functions [type] [subtype]  shows available functions" << endl;
      os << "       possible types: math, comparison, conversion, datetime, string" << endl;
      os << "                       array, reduce, astro, misc, aggregate" << endl;
      os << "                name of UDF libraries (e.g., show functions mscal)" << endl;
      os << "  show meastypes [type]            shows available measure types" << endl;
      os << "       possible types: epoch, position, direction" << endl;
      os << "  show units [kind]                shows available units and/or prefixes" << endl;
      os << "       possible kinds: prefix, length, time, angle, temperature," << endl;
      os << "                       current, intensity, molar, mass, solidangle" << endl;
      os << "       If a unit is given as kind, all corresponding units are shown" << endl;
      os << "       Note that TaQL can convert between ANGLE and TIME" << endl;
    } else {
      String cmd(parts[0]);
      cmd.downcase();
      String type;
      if (parts.size() > 1) type = parts[1];
      type.downcase();
      if (cmd == "table") {
        showTable (os, parts);
      } else if (cmd == "syntax") {
        showSyntax (os, type);
      } else if (cmd == "operator"  ||  cmd == "operators") {
        showOpers (os);
      } else if (cmd == "func"  ||  cmd == "function"  ||
                 cmd == "functions") {
        showFuncs (os, type, parts, style);
      } else if (cmd == "meastype"  ||  cmd == "meastypes") {
        showMeasTypes (os, type);
      } else if (cmd == "unit"  ||  cmd == "units") {
        showUnits (os, type);
      } else {
        throw AipsError (cmd + " is an unknown SHOW command");
      }
    }
    return os.str();
  }

  void TaQLShow::showTable (ostream& os, const Vector<String>& parts)
  {
    if (parts.size() < 2  ||  parts[1].empty()) {
      os << "Usage:   show table tablename [opt1 opt2 ...]" << endl;
      os << "    Options   default"<<endl;
      os << "         dm      nodm   show data managers?" << endl;
      os << "        col       col   show column descriptions?" << endl;
      os << "       sort    nosort   show columns alphabetically?" << endl;
      os << "        key     nokey   show table and column keywords?"<< endl;
      os << "     tabkey  notabkey   show table keywords?" << endl;
      os << "     colkey  nocolkey   show column keywords?" << endl;
      os << "      recur   norecur   show subtables recursively?" << endl;
      return;
    }
    Table table(parts[1]);
    Bool showdm = False;
    Bool showcol = True;
    Bool showsub = False;
    Bool sortcol = False;
    Bool tabkey = False;
    Bool colkey = False;
    for (uInt i=2; i<parts.size(); ++i) {
      String opt(parts[i]);
      opt.downcase();
      Bool fop = True;
      if (opt.size() > 2   &&  opt.substr(0,2) == "no") {
        fop = False;
        opt = opt.substr(2);
      }
      if (opt == "dm") {
        showdm = fop;
      } else if (opt == "col") {
        showcol = fop;
      } else if (opt == "sort") {
        sortcol = fop;
      } else if (opt == "key") {
        tabkey = fop;
        colkey = fop;
      } else if (opt == "tabkey") {
        tabkey = fop;
      } else if (opt == "colkey") {
        colkey = fop;
      } else if (opt == "recur") {
        showsub = fop;
      } else {
        throw AipsError (parts[i] + " is an unknown show table option; use: "
                         "dm col sort key colkey recur");
      }
    }
    table.showStructure (os, showdm, showcol, showsub, sortcol);
    table.showKeywords (os, showsub, tabkey, colkey);
  }

  void TaQLShow::showSyntax (ostream& os, const String& cmd)
  {
    if (cmd.empty()) {
      os << "SELECT [[DISTINCT] expression_list]" << endl;
      os << "  [INTO table [AS options]]" << endl;
      os << "  [FROM table_list]" << endl;
      os << "  [WHERE expression]" << endl;
      os << "  [GROUPBY expression_list]" << endl;
      os << "  [HAVING expression]" << endl;
      os << "  [ORDERBY [DISTINCT] sort_list]" << endl;
      os << "  [LIMIT expression] [OFFSET expression]" << endl;
      os << "  [GIVING table [AS options] | set]" << endl;
      os << "  [DMINFO datamanagers]" << endl;
      os << "CALC expression [FROM table_list]" << endl;
      os << "UPDATE table_list SET update_list [FROM table_list]" << endl;
      os << "  [WHERE ...] [ORDERBY ...] [LIMIT ...] [OFFSET ...]" << endl;
      os << "INSERT INTO table_list SET column=expr, column=expr, ..." << endl;
      os << "INSERT INTO table_list [(column_list)] VALUES (expr_list)" << endl;
      os << "INSERT INTO table_list [(column_list)] SELECT_command" << endl;
      os << "DELETE FROM table_list" << endl;
      os << "  [WHERE ...] [ORDERBY ...] [LIMIT ...] [OFFSET ...]" << endl;
      os << "CREATE TABLE table [AS options]" << endl;
      os << "  [column_specs]" << endl;
      os << "  [LIMIT ...]" << endl;
      os << "  [DMINFO datamanagers]" << endl;
      os << "ALTER TABLE table" << endl;
      os << "  [ADD COLUMN [column_specs] [DMINFO datamanagers]" << endl;
      os << "  [RENAME COLUMN old TO new, old TO new, ...]" << endl;
      os << "  [DROP COLUMN col, col, ...]" << endl;
      os << "  [SET KEYWORD key=value, key=value]" << endl;
      os << "  [COPY KEYWORD key=other, key=other, ...]" << endl;
      os << "  [RENAME KEYWORD old TO new, old TO new, ...]" << endl;
      os << "  [DROP KEYWORD key, key, ...]" << endl;
      os << "  [ADD ROW nrow]" << endl;
      os << "COUNT [column_list] FROM table_list [WHERE ...]" << endl;
      os << endl;
      os << "Use 'show syntax <command>' for more information about the command" << endl;
    } else if (cmd == "select") {
      showSyntaxSelect (os);
    } else if (cmd == "calc") {
      showSyntaxCalc (os);
    } else if (cmd == "update") {
      showSyntaxUpdate (os);
    } else if (cmd == "insert") {
      showSyntaxInsert (os);
    } else if (cmd == "delete") {
      showSyntaxDelete (os);
    } else if (cmd == "create") {
      showSyntaxCreate (os);
    } else if (cmd == "alter") {
      showSyntaxAlter (os);
    } else if (cmd == "count") {
      showSyntaxCount (os);
    } else {
      os << cmd + " is an unknown command for 'show syntax <command>'" << endl;
      os << "   use select, calc, update, insert, delete, create, alter or count" << endl;
    }
  }

  void TaQLShow::showSyntaxSelect (ostream& os)
  {
    os << "'show syntax select' not implemented yet" << endl;
  }

  void TaQLShow::showSyntaxCalc (ostream& os)
  {
    os << "'show syntax calc' not implemented yet" << endl;
  }

  void TaQLShow::showSyntaxUpdate (ostream& os)
  {
    os << "'show syntax update' not implemented yet" << endl;
  }

  void TaQLShow::showSyntaxInsert (ostream& os)
  {
    os << "'show syntax insert' not implemented yet" << endl;
  }

  void TaQLShow::showSyntaxDelete (ostream& os)
  {
    os << "'show syntax delete' not implemented yet" << endl;
  }

  void TaQLShow::showSyntaxCreate (ostream& os)
  {
    os << "'show syntax create' not implemented yet" << endl;
  }

  void TaQLShow::showSyntaxAlter (ostream& os)
  {
    os << "'show syntax alter' not implemented yet" << endl;
  }

  void TaQLShow::showSyntaxCount (ostream& os)
  {
    os << "'show syntax count' not implemented yet" << endl;
  }

  void TaQLShow::showOpers (ostream& os)
  {
    os << "Available TaQL operators in order of precedence:" << endl;
    os << "    **" << endl;
    os << "    !  ~  +  -       (unary operators)" << endl;
    os << "    *  /  // %" << endl;
    os << "    +  -" << endl;
    os << "    &" << endl;
    os << "    ^" << endl;
    os << "    |" << endl;
    os << "    == != >  >= <  <=  ~= !~= IN INCONE BETWEEN EXISTS LIKE  ~  !~" << endl;
    os << "    &&" << endl;
    os << "    ||" << endl;
    os << "Some operators have a synonym:" << endl;
    os << "    ==     =" << endl;
    os << "    !=     <>" << endl;
    os << "    &&     AND" << endl;
    os << "    ||     OR" << endl;
    os << "    !      NOT" << endl;
    os << "    ^      XOR" << endl;
    os << endl;
    os << "    **          power, right-associative, thus 2**1**2 = 2" << endl;
    os << "    ~           if unary, bit-wise complement" << endl;
    os << "    /           real division, thus 1/2 = 0.5" << endl;
    os << "    //          integer division, thus 1//2 = 0" << endl;
    os << "    %           modulo" << endl;
    os << "    +           also string concat; also for datetimes" << endl;
    os << "    -           also for datetimes" << endl;
    os << "    &           bit-wise and" << endl;
    os << "    ^           bit-wise xor" << endl;
    os << "    |           bit-wise or" << endl;
    os << "    ~= !~=      (not) about equal (relative to 1e-5)" << endl;
    os << "    ~ !~ LIKE   pattern match" << endl;
  }

  void TaQLShow::showFuncs (ostream& os, const String& type,
                            const Vector<String>& parts,
                            const TaQLStyle& style)
  {
    if (type.empty()  ||  type == "all") {
      showAllFuncs (os);
    } else if (type == "math") {
      showMathFuncs (os);
    } else {
      try {
        TableExprNodeSet operands;
        String ftype;
        if (parts.size() > 2) ftype = parts[2];
        operands.add (TableExprNodeSetElem(ftype));
        TableExprNode node = TableExprNode::newUDFNode (type+".help",
                                                        operands,
                                                        Table(), style);
        os << node.getString(0);
      } catch (const std::exception&) {
        os << type + " is an unknown type in 'show functions <type>'" << endl
           << "  (maybe an unknown UDF library)" << endl;
      }
    }
  }

  void TaQLShow::showAllFuncs (ostream& os)
  {
    os << "math functions:" << endl;
    os << "  pi          e           c           rand" << endl;
    os << "  sin         sinh        asin        cos         cosh        acos" << endl;
    os << "  tan         tanh        atan        atan2" << endl;
    os << "  exp         log         log10" << endl;
    os << "  pow         sqrt        sqr         cube" << endl;
    os << "  norm        abs         arg         fmod" << endl;
    os << "  sign        round       floor       ceil" << endl;
    os << "comparison functions:" << endl;
    os << "  near        nearabs     isnan       isinf       isfinite" << endl;
    os << "  isdef       isnull      iscol       iskey" << endl;
    os << "  min         max         iif" << endl;
    os << "conversion functions:" << endl;
    os << "  bool        int         real        imag        complex     conj" << endl;
    os << "  string      hms         dms         hdms" << endl;
    os << "datetime functions:" << endl;
    os << "  datetime    mjdtodate   mjd         date        time" << endl;
    os << "  year        month       day         weekday     week" << endl;
    os << "  cmonth      cweekday    cdatetime   cdate       ctime" << endl;
    os << "string functions:" << endl;
    os << "  strlength   upcase      downcase    capitalize" << endl;
    os << "  trim        ltrim       rtrim       substr      replace" << endl;
    os << "  regex       pattern     sqlpattern" << endl;
    os << "array functions:" << endl;
    os << "  array       ndim        nelem       shape" << endl;
    os << "  transpose   resize      diagonal" << endl;
    os << "  nullarray   marray      arraydata   mask" << endl;
    os << "  flatten     negatemask  replacemasked           replaceunmasked" << endl;
    os << "reduce functions:" << endl;
    os << "  sum         product     sumsqr      min         max" << endl;
    os << "  mean        variance    stddev      avdev       rms" << endl;
    os << "  median      fractile    any         all         ntrue       nfalse" << endl;
    os << "    plural, running and boxed forms of above reduce functions" << endl;

    os << "astro functions:" << endl;
    os << "  angdist     angdistx    cones       anycone     findcone" << endl;
    os << "    see also 'show functions meas' and 'show functions mscal'" << endl;
    os << "misc functions:" << endl;
    os << "  rownr       rowid" << endl;
    os << "aggregate functions:" << endl;
    os << "  gmin        gmax        gsum        gproduct    gsumsqr" << endl;
    os << "  gmean       gvariance   gstddev     grms" << endl;
    os << "  gany        gall        gntrue      gnfalse" << endl;
    os << "    plural forms of above aggregate functions (e.g., gmins)" << endl;
    os << "  gmedian     gfractile   ghist       gstack" << endl;
    os << "  countall    gcount      gfirst      glast " << endl;
  }

  void TaQLShow::showMathFuncs (ostream& os)
  {
    os << "TaQL Mathematical functions:" << endl;
    os << "  numeric cos (numeric)" << endl;
  }

  void TaQLShow::showUnitKind (ostream& os, const UnitVal& kind,
                               const map<String, UnitName>& units)
  {
    for (map<String,UnitName>::const_iterator iter = units.begin();
         iter != units.end(); ++iter) {
      if (Unit(iter->first).getValue() == kind) {
        os << "    " << iter->second << endl;
      }
    }
  }

  void TaQLShow::showUnits (ostream& os, const String& type)
  {
    if (type.empty()) {
      UnitMap::list (os);
    } else if (type == "prefix") {
      UnitMap::listPref (os);
    } else {
      UnitVal kind;
      if (type == "length") {
        kind = UnitVal::LENGTH;
      } else if (type == "mass") {
        kind = UnitVal::MASS;
      } else if (type == "time") {
        kind = UnitVal::TIME;
      } else if (type == "current") {
        kind = UnitVal::CURRENT;
      } else if (type == "temperature") {
        kind = UnitVal::TEMPERATURE;
      } else if (type == "intensity") {
        kind = UnitVal::INTENSITY;
      } else if (type == "molar") {
        kind = UnitVal::MOLAR;
      } else if (type == "angle") {
        kind = UnitVal::ANGLE;
      } else if (type == "solidangle") {
        kind = UnitVal::SOLIDANGLE;
      } else {
        try {
          Unit unit(type);
          kind = unit.getValue();
        } catch (const AipsError&) {
          throw AipsError ("Unknown kind or unit given in command "
                           "'show units " + type + "'\nUse 'show' to "
                           "show the valid kinds");
        }
      }
      showUnitKind (os, kind, UnitMap::giveDef());
      showUnitKind (os, kind, UnitMap::giveSI());
      showUnitKind (os, kind, UnitMap::giveCust());
      showUnitKind (os, kind, UnitMap::giveUser());
    }
  }

  void TaQLShow::showMeasTypes (ostream& os, const String& type)
  {
    // Because libtables cannot be dependent on libmeasures, the
    // no Measures functions can be used to show the types.
    Bool ok = False;
    if (type.empty()  ||  type == "epoch") {
      os << "Epoch types:" << endl;
      os << "    LAST           Local Apparent Sidereal Time" << endl;
      os << "    LMST           Local Mean Sidereal Time" << endl;
      os << "    GMST1, GMST    Greenwich Mean ST1" << endl;
      os << "    GAST           Greenwich Apparent ST" << endl;
      os << "    UT1, UT" << endl;
      os << "    UT2" << endl;
      os << "    UTC" << endl;
      os << "    TAI, IAT" << endl;
      os << "    TDT, TT, ET" << endl;
      os << "    TCG" << endl;
      os << "    TDB" << endl;
      os << "    TCB" << endl;
      ok = True;
    }
    if (type.empty()  ||  type == "position") {
      os << "Position types:" << endl;
      os << "    ITRF" << endl;
      os << "    WGS84" << endl;
      ok = True;
    }
    if (type.empty()  ||  type == "direction") {
      os << "Direction types:" << endl;
      os << "    J2000       mean equator and equinox at J2000.0 (FK5)" << endl;
      os << "    JNAT        geocentric natural frame" << endl;
      os << "    JMEAN       mean equator and equinox at frame epoch" << endl;
      os << "    JTRUE       true equator and equinox at frame epoch" << endl;
      os << "    APP         apparent geocentric position" << endl;
      os << "    B1950       mean epoch and ecliptic at B1950.0" << endl;
      os << "    B1950_VLA   mean epoch(1979.9)) and ecliptic at B1950.0" << endl;
      os << "    BMEAN       mean equator and equinox at frame epoch" << endl;
      os << "    BTRUE       true equator and equinox at frame epoch" << endl;
      os << "    HADEC       topocentric HA and declination" << endl;
      os << "    AZEL        topocentric Azimuth and Elevation (N through E)" << endl;
      os << "    AZELSW      topocentric Azimuth and Elevation (S through W)" << endl;
      os << "    AZELNE      topocentric Azimuth and Elevation (N through E)" << endl;
      os << "    AZELGEO     geodetic Azimuth and Elevation (N through E)" << endl;
      os << "    AZELNEGEO   geodetic Azimuth and Elevation (N through E)" << endl;
      os << "    AZELSWGEO   geodetic Azimuth and Elevation (S through W)" << endl;
      os << "    ECLIPTIC    ecliptic for J2000 equator and equinox" << endl;
      os << "    MECLIPTIC   ecliptic for mean equator of date" << endl;
      os << "    TECLIPTIC   ecliptic for true equator of date" << endl;
      os << "    GALACTIC    galactic coordinates" << endl;
      os << "    SUPERGAL    supergalactic coordinates" << endl;
      os << "    ITRF        coordinates wrt ITRF Earth frame" << endl;
      os << "    TOPO        apparent topocentric position" << endl;
      os << "    ICRS        International Celestial reference system" << endl;
      os << endl;
      os << "See also 'show functions meas pos/dir" << endl;
      ok = True;
    }
    if (!ok) {
      throw AipsError (type +
                       " is an unknown type for command "
                       "'show meastypes <type>'");
    }
  }

} //# NAMESPACE CASACORE - END
