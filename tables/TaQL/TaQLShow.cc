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
      os << "  show constants                   shows how to specify constantscalars and arrays" << endl;
      os << "  show functions [type] [subtype]  shows available functions" << endl;
      os << "       possible types: math, logical, conv(ersion), datetime, string" << endl;
      os << "                       array, reduce, astro, misc, aggr(egate)" << endl;
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
      } else if (cmd == "constant"  ||  cmd == "constants") {
        showConstants (os);
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
    Table table(Table::openTable(parts[1]));
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
      os << "SELECT [[DISTINCT] expression_list] [INTO table [AS options]]" << endl;
      os << "  [FROM table_list] [WHERE expression]" << endl;
      os << "  [GROUPBY expression_list] [HAVING expression]" << endl;
      os << "  [ORDERBY [DISTINCT] sort_list] [LIMIT expression] [OFFSET expression]" << endl;
      os << "  [GIVING table [AS options] | set] [DMINFO datamanagers]" << endl;
      os << endl;
      os << "CALC expression [FROM table_list]" << endl;
      os << endl;
      os << "UPDATE table_list SET update_list [FROM table_list]" << endl;
      os << "  [WHERE ...] [ORDERBY ...] [LIMIT ...] [OFFSET ...]" << endl;
      os << endl;
      os << "INSERT INTO table_list SET column=expr, column=expr, ..." << endl;
      os << "INSERT INTO table_list [(column_list)] VALUES (expr_list)" << endl;
      os << "INSERT INTO table_list [(column_list)] SELECT_command" << endl;
      os << endl;
      os << "DELETE FROM table_list" << endl;
      os << "  [WHERE ...] [ORDERBY ...] [LIMIT ...] [OFFSET ...]" << endl;
      os << endl;
      os << "CREATE TABLE table [AS options]" << endl;
      os << "  [column_specs]" << endl;
      os << "  [LIMIT ...]" << endl;
      os << "  [DMINFO datamanagers]" << endl;
      os << endl;
      os << "ALTER TABLE table" << endl;
      os << "  [ADD COLUMN [column_specs] [DMINFO datamanagers]" << endl;
      os << "  [RENAME COLUMN old TO new, old TO new, ...]" << endl;
      os << "  [DROP COLUMN col, col, ...]" << endl;
      os << "  [SET KEYWORD key=value, key=value, ...]" << endl;
      os << "  [COPY KEYWORD key=other, key=other, ...]" << endl;
      os << "  [RENAME KEYWORD old TO new, old TO new, ...]" << endl;
      os << "  [DROP KEYWORD key, key, ...]" << endl;
      os << "  [ADD ROW nrow]" << endl;
      os << endl;
      os << "COUNT [column_list] FROM table_list [WHERE ...]" << endl;
      os << endl;
      os << "Use 'show syntax <command>' for more information about the command" << endl;
      os << "    'show syntax set'       for more information about sets/intervals" << endl;
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
    } else if (cmd == "set") {
      showSyntaxInterval (os);
    } else {
      os << cmd + " is an unknown command for 'show syntax <command>'" << endl;
      os << "   use select, calc, update, insert, delete, create, alter, count or set" << endl;
    }
  }

  void TaQLShow::showSyntaxSelect (ostream& os)
  {
    os << "SELECT" << endl;
    os << "  [[DISTINCT] expression_list]" << endl;
    os << "  [INTO table [AS options]]" << endl;
    os << "  [FROM table_list]" << endl;
    os << "  [WHERE expression]" << endl;
    os << "  [GROUPBY expression_list]" << endl;
    os << "  [HAVING expression]" << endl;
    os << "  [ORDERBY [DISTINCT] sort_list]" << endl;
    os << "  [LIMIT expression] [OFFSET expression]" << endl;
    os << "  [GIVING table [AS options] | set]" << endl;
    os << "  [DMINFO datamanagers]" << endl;
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

  void TaQLShow::showSyntaxInterval (ostream& os)
  {
    os << "A set is a series of values, ranges and/or intervals enclosed in brackets." << endl;
    os << "Often the IN operator is used on a set, but a set can also be used as an array." << endl;
    os << endl;
    os << "A value can be of type integer, double, complex, datetime or string." << endl;
    os << "Numeric data types can be mixed; the 'highest' type is used." << endl;
    os << endl;
    os << "A range is a series of values written as start:end:step" << endl;
    os << "  'start' can be left out making it unbounded (from -infinity)" << endl;
    os << "  'end'   can be left out making it unbounded (till +infinity)" << endl;
    os << "  ':step' can be left out and defaults to 1" << endl;
    os << "start and end can be integer, double, or datetime" << endl;
    os << "step must be integer or double" << endl;
    os << endl;
    os << "An interval is a continuous set of real values with optional bounds." << endl;
    os << "If a bound is given, it can be open or closed." << endl;
    os << "An interval can be given in two ways:" << endl;
    os << "  Using curly braces (closed bound) and arrows (open bound)" << endl;
    os << "    bounded:     {1,2}   <1,2>   {1,2>   <1,2}" << endl;
    os << "    unbounded:   {1,}    <1,>    {,2>    <,2}" << endl;
    os << "  Using  a=:=b (closed bounds)  and  a<:<b (open bounds)" << endl;
    os << "    bounded:     1=:=2   1<:<2   1=:<2   1<:=2" << endl;
    os << "    unbounded:   1=:     1<:     :<2     :=2" << endl;
    os << endl;
    os << "A set consisting of values and/or bounded ranges is a 1-dim array." << endl;
    os << "  For example:   [1,2,3,4,5]   [1:6]   [1,2:5,5]   are all the same" << endl;
    os << "Multi-dimensional arrays can be given using nested sets." << endl;
    os << "  For example    [[1,2,3],[4:7]]   defines an array with shape [2,3]" << endl;
    os << "" << endl;
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

  void TaQLShow::showConstants (ostream& os)
  {
    os << "Scalar constants of following data types:" << endl;
    os << "  Bool       TRUE or FALSE (case-insensitive), T or F" << endl;
    os << "  Integer    also hexadecimal like 0xffff" << endl;
    os << "  Double     12.  12e5  3.2e-5  etc." << endl;
    os << "             value followed by a unit like 10m or 10.5sec" << endl;
    os << "             position/time in HMS or DMS like 2h13m44.5 or 30d13m44.4" << endl;
    os << "               results in double with unit rad" << endl;
    os << "  Complex    add imaginary part like  1+2i  or  3 - 5j" << endl;
    os << "  String     enclose in single or double quotes; concatenation like 'ab'\"cd\"" << endl;
    os << "  Datetime   date/time like 3Mar16/12:14:23.3 or 2016-03-02/1:4:23" << endl;
    os << "               - or space can be used instead of /" << endl;
    os << "  Regex      p/globpattern/ or f/regex/ or m/regex/ (same as f/.*regex.*/)" << endl; 
    os << "               used with operator ~ or !~" << endl;
    os << endl;
    os << "N-dim array of those data types (except regex) like:" << endl;
    os << "             [1,2,3] (1-dim)  or  [[1,2,3],[4,5e3,6]] (2-dim)" << endl;
    os << "             using function ARRAY" << endl;
    os << endl;
    os << "Masked array (True value means bad (as in numpy)):" << endl;
    os << "             array[mask]  like  [1,2,3][[T,F,T]]" << endl;
    os << "             using function MARRAY" << endl;
  }

  void TaQLShow::showFuncs (ostream& os, const String& type,
                            const Vector<String>& parts,
                            const TaQLStyle& style)
  {
    if (type.empty()  ||  type == "all") {
      showAllFuncs (os);
    } else if (type == "math") {
      showMathFuncs (os);
    } else if (type == "conversion"  ||  type == "conv") {
      showConvFuncs (os);
    } else if (type == "logical") {
      showLogicalFuncs (os);
    } else if (type == "datetime") {
      showDateTimeFuncs (os);
    } else if (type == "string") {
      showStringFuncs (os);
    } else if (type == "array") {
      showArrayFuncs (os);
    } else if (type == "reduce") {
      showReduceFuncs (os);
    } else if (type == "astro") {
      showAstroFuncs (os);
    } else if (type == "misc") {
      showMiscFuncs (os);
    } else if (type == "aggr") {
      showAggrFuncs (os);
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
    os << "About all TaQL functions operate on scalars and arrays (and mixed)" << endl;
    os << "math functions:" << endl;
    os << "  pi          e           c           rand" << endl;
    os << "  sin         sinh        asin        cos         cosh        acos" << endl;
    os << "  tan         tanh        atan        atan2" << endl;
    os << "  exp         log         log10" << endl;
    os << "  pow         sqrt        sqr         cube" << endl;
    os << "  norm        abs         arg         fmod" << endl;
    os << "  sign        round       floor       ceil" << endl;
    os << "logical functions:" << endl;
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
    os << "    see also 'show func meas' and 'show func mscal'" << endl;
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
    os << "Mathematical functions" << endl;
    os << endl;
    os << "  double  PI    ()" << endl;
    os << "  double  E     ()" << endl;
    os << "  double  C     ()                      m/s" << endl;
    os << "  double  RAND  ()" << endl;
    os << "  numeric SIN   (numeric)" << endl;
    os << "  numeric SINH  (numeric)" << endl;
    os << "  double  ASIN  (real)                  rad" << endl;
    os << "  numeric COS   (numeric)" << endl;
    os << "  numeric COSH  (numeric)" << endl;
    os << "  double  ACOS  (real)                  rad" << endl;
    os << "  double  TAN   (real)" << endl;
    os << "  double  TANH  (real)" << endl;
    os << "  double  ATAN  (real)                  rad" << endl;
    os << "  double  ATAN2 (real y, real x)        rad" << endl;
    os << "  numeric EXP   (numeric)" << endl;
    os << "  numeric LOG   (numeric)" << endl;
    os << "  numeric LOG10 (numeric)" << endl;
    os << "  numeric POW   (numeric, numeric exp)" << endl;
    os << "  numeric SQRT  (numeric)" << endl;
    os << "  numeric SQR   (numeric)      aka SQUARE" << endl;
    os << "  numeric CUBE  (numeric)" << endl;
    os << "  real    NORM  (numeric)" << endl;
    os << "  real    ABS   (numeric)      aka AMPLITUDE" << endl;
    os << "  double  ARG   (numeric)      aka PHASE" << endl;
    os << "  real    FMOD  (real, real)" << endl;
    os << "  real    SIGN  (real)" << endl;
    os << "  real    ROUND (real)         round(-1.6) = -2" << endl;
    os << "  real    FLOOR (real)         floor(-2.2) = -3" << endl;
    os << "  real    CEIL  (real)         ceil (-2.2) = -2" << endl;
  }

  void TaQLShow::showConvFuncs (ostream& os)
  {
    os << "Conversion functions" << endl;
    os << endl;
    os << "  string HMS       (real RAD)    convert angles to e.g. 12h34m56.789" << endl;
    os << "  string DMS       (real RAD)    convert angles to e.g. 12d34m56.789" << endl;
    os << "  string HDMS      (realarray)   convert angles alternately to HMS and DMS" << endl;
    os << "  string STR       (string, int WIDTH)    make string WIDTH long" << endl;
    os << "  string STR       (numeric, )    make string WIDTH long" << endl;
    os << "  string STR       (string, int WIDTH)    make string WIDTH long" << endl;
  }

  void TaQLShow::showLogicalFuncs (ostream& os)
  {
    os << "Logical functions" << endl;
    os << endl;
    os << "  bool    NEAR      (numeric, numeric, double tol)    relative near" << endl;
    os << "  bool    NEARABS   (numeric, numeric, double tol)    absolute near" << endl;
    os << endl;
    os << "  bool    ISNAN     (numeric val)       is value Not-a-Number?" << endl;
    os << "  bool    ISINF     (numeric val)       is value infinite?" << endl;
    os << "  bool    ISFINITE  (numeric val)       is value finite?" << endl;
    os << "  bool    ISNULL    (anytype)           is array a null array?" << endl;
    os << "  bool    ISDEFINED (anytype)           contains row a value?" << endl;
    os << "  bool  t.ISCOLUMN  (string)            does keyword exist in table?" << endl;
    os << "  bool  t.ISKEYWORD (string)            does keyword exist in table?" << endl;
    os << endl;
    os << "  numeric MIN (numeric, numeric)" << endl;
    os << "  numeric MAX (numeric, numeric)" << endl;
    os << "  anytype IIF (bool cond, arg1, arg2)   arg1 if cond is True, else arg2" << endl;
  }

  void TaQLShow::showDateTimeFuncs (ostream& os)
  {
    os << "Date/time functions" << endl;
    os << "Functions taking a datetime, use current UTC date/time if not given" << endl;
    os << endl;
    os << "  datetime DATETIME  (string)       convert string to datetime" << endl;
    os << "  datetime MJDTODATE (real)         convert MJD to datetime" << endl;
    os << "  double   MJD       (datetime)     convert datetime to MJD" << endl;
    os << "  datetime DATE      (datetime)     get date, thus remove time part" << endl;
    os << "  double   TIME      (datetime)     get time (in rad), thus remove date part" << endl;
    os << "  int      YEAR      (datetime)     get year" << endl;
    os << "  int      MONTH     (datetime)     get month (1..12)" << endl;
    os << "  int      DAY       (datetime)     get day (1..31)" << endl;
    os << "  int      WEEK      (datetime)     get week number (0..53)" << endl;
    os << "  int      WEEKDAY   (datetime)   aka DOW     weekday (1=Mondaeey .. 7=Sunday)" << endl;
    os << "  string   CDATETIME (datetime)   aka CTOD    YYYY/MM/DD/HH:MM:SS.SSS" << endl;
    os << "  string   CDATE     (datetime)               DD-MMM-YYYY" << endl;
    os << "  string   CTIME     (datetime)               HH:MM:SS.SSS" << endl;
    os << "  string   CMONTH    (datetime)               Jan..Dec" << endl;
    os << "  string   CWEEKDAY  (datetime)   aka CDOW    Mon..Sun" << endl;
  }

  void TaQLShow::showStringFuncs (ostream& os)
  {
    os << "String functions" << endl;
    os << endl;
    os << "  int    LEN        (string)      aka STRLENGTH" << endl;
    os << "  string UPCASE     (string)      aka UPPER" << endl;
    os << "  string DOWNCASE   (string)      aka LOWER" << endl;
    os << "  string CAPITALIZE (string)" << endl;
    os << "  string LTRIM      (string)      remove leading whitespace" << endl;
    os << "  string RTRIM      (string)      remove trailing whitespace" << endl;
    os << "  string TRIM       (string)      remove leading/trailing whitespace" << endl;
    os << "  string SUBSTR     (string, int START, int N)    ST<0 means from the end" << endl;
    os << "  string REPLACE    (string SRC, string TOREPLACE[, string REPLACEMENT])" << endl;
    os << "  string REPLACE    (string SRC, regex  TOREPLACE[, string REPLACEMENT])" << endl;
    os << endl;
    os << "  regex  REGEX      (string)      make regex from regular expression string" << endl;
    os << "  regex  PATTERN    (string)      make regex from glob pattern string" << endl;
    os << "  regex  SQLPATTERN (string)      make regex from SQL-style pattern string" << endl;
    os << endl;
    os << "'show func conversion'  for functions converting values to string" << endl;
    os << "'show func datetime'    for functions converting date/time to string" << endl;
  }

  void TaQLShow::showArrayFuncs (ostream& os)
  {
    os << "Array creation/manipulation functions" << endl;
    os << endl;
    os << "  array ARRAY     (value, shape)          create array and fill with value" << endl;
    os << "  int   NELEMENTS (any)     aka COUNT     size of array (1 for scalar)" << endl;
    os << "  int   NDIM      (any)                   dimensionality of array (0 for scalar)" << endl;
    os << "  array SHAPE     (any)                   shape of array (empty for scalar)" << endl;
    os << "  array TRANSPOSE (array [,axes])         transpose array" << endl;
    os << "    if axes are given, only those axes are transposed" << endl;
    os << "  array RESIZE    (array, shape [,mode])  resize an array" << endl;
    os << "    no mode: copy corresponding elements" << endl;
    os << "    mode=0 : upsampling; copy values evenly if axis gets larger: 1,2 -> 1,1,2,2" << endl;
    os << "    mode=1 : repeat values if axis gets larger: 1,2 -> 1,2,1,2" << endl;
    os << "  array DIAGONAL  (array [,firstaxis [,diag]])" << endl;
    os << "    diagonal of each 2-dim subarray (at axis firstaxis)" << endl;
    os << "    diag=0 main diagonal; <0 below main diagonal; >0 above" << endl;
    os << endl;
    os << "  array NULLARRAY (value)                 create null array" << endl;
    os << "  array MARRAY    (array, boolarray)      create masked array" << endl;
    os << "    same as 'array[boolarray]'" << endl;
    os << "  array ARRAYDATA (array)                 array without possible mask" << endl;
    os << "  bool  ARRAYMASK (array)   aka MASK      mask of masked array" << endl;
    os << "  array FLATTEN   (array)                 remove masked elements" << endl;
    os << "  array NEGATEMASK(array)                 negate mask in masked array" << endl;
    os << "  array REPLACEMASKED   (arr1, arr2)" << endl;
    os << "    replace masked elements in arr1 by corresponding value in arr2" << endl;
    os << "  array REPLACEUNMASKED (arr1, arr2)" << endl;
    os << "    replace unmasked elements in arr1 by corresponding value in arr2" << endl;
  }

  void TaQLShow::showReduceFuncs (ostream& os)
  {
    os << "Array reduce functions (use unmasked elements only)" << endl;
    os << " XXX        (array)               reduces to a scalar" << endl;
    os << " XXXS       (array, reduceAxes)   reduces to a (N-M)-dim array" << endl;
    os << " RUNNINGXXX (array, windowSize)   calculates XXX in sliding window over pixel" << endl;
    os << " BOXEDXXX   (array, boxSize)      calculates XXX for each box" << endl;
    os << endl;
    os << "XXX can be one of the following functions:" << endl;
    os << "  bool    ANY      (bool)      is any element true?" << endl;
    os << "  bool    ALL      (bool)      are all elements true?" << endl;
    os << "  int     NTRUE    (bool)      number of true elements" << endl;
    os << "  int     NFALSE   (bool)      number of false elements" << endl;
    os << "  numeric SUM      (numeric)   sum of all elements" << endl;
    os << "  numeric SUMSQR   (numeric)   sum of all squared elements   aka SUMSQUARE" << endl;
    os << "  numeric PRODUCT  (numeric)   product of all elements" << endl;
    os << "  real    MIN      (real)      minimum of all elements" << endl;
    os << "  real    MAX      (real)      maximum of all elements" << endl;
    os << "  numeric MEAN     (numeric)   mean of all elements          aka AVG" << endl;
    os << "  double  VARIANCE (real)      variance" << endl;
    os << "  double  STDDEV   (real)      standard deviation" << endl;
    os << "  double  AVDEV    (real)      average deviation" << endl;
    os << "  double  RMS      (real)      root-mean-squares" << endl;
    os << "  double  MEDIAN   (real)      median (the middle element)" << endl;
    os << "  double  FRACTILE (real, fraction)   element at given fraction" << endl;
  }

  void TaQLShow::showAstroFuncs (ostream& os)
  {
    os << "Astronomical functions" << endl;
    os << endl;
    os << "  double ANGDIST  (arg1,arg2)     aka ANGULARDISTANCE" << endl;
    os << "    angular distance (in rad) between corrersponding positions in arg1 and arg2" << endl;
    os << "    arg1 and arg2 must be arrays containing ra/dec or lon/lat pairs" << endl;
    os << "  double ANGDISTX (arg1,arg2)     aka ANGULARDISTANCEX" << endl;
    os << "    same as ANGDIST, but between all positions in arg1 and arg2" << endl;
    os << "  bool   ANYCONE    (source, cones)" << endl;
    os << "    True if source in at least one of the cones" << endl;
    os << "    synonym for operator INCONE" << endl;
    os << "  bool   ANYCONE    (source, conepos, radii)" << endl;
    os << "    same as above, but cone centers and radii are given separately" << endl;
    os << "    each radius is applied to each cone" << endl;
    os << "  int    FINDCONE    (sources, cones)" << endl;
    os << "    index of the first cone containing a source" << endl;
    os << "    if a single source is given, the result is a scalar, otherwise an array" << endl;
    os << "  int    FINDCONE    (sources, conepos, radii)" << endl;
    os << "    same as above, but cone centers and radii are given separately" << endl;
    os << "    each radius is applied to each cone" << endl;
    os << "  bool   CONES      (sources, cones)" << endl;
    os << "    2-dim bool array telling for each source if in each cone" << endl;
    os << "  bool   CONES      (sources, conepos, radii)" << endl;
    os << "    3-dim bool array telling for each source if in each cone and radius" << endl;
    os << endl;
    os << "'show func meas'   for measures functions converting between reference frames" << endl;
    os << "'show func mscal'  for mscal functions handling measures in MeasurementSets" << endl;
  }

  void TaQLShow::showMiscFuncs (ostream& os)
  {
    os << "Miscellaneous functions" << endl;
    os << endl;
    os << "  int ROWNR()   aka ROWNUMBER    return row number in current table" << endl;
    os << "  int ROWID()                    return row number in input table" << endl;
  }

  void TaQLShow::showAggrFuncs (ostream& os)
  {
    os << "Aggregate functions operating per group (using GROUPBY)" << endl;
    os << endl;
    os << "The following functions results in a scalar value" << endl;
    os << "  int     GCOUNT()              number of rows                aka GCOUNT(*)" << endl;
    os << "  int     GCOUNT (columnname)   number of rows for which the column has a value" << endl;
    os << "  anytype GFIRST    (anytype)   first value in the group" << endl;
    os << "  anytype GLAST     (anytype)   last value of the group" << endl;
    os << "  bool    GANY      (bool)      is any element true?" << endl;
    os << "  bool    GALL      (bool)      are all elements true?" << endl;
    os << "  bool    GNTRUE    (bool)      number of true elements" << endl;
    os << "  int     GNFALSE   (bool)      number of false elements" << endl;
    os << "  numeric GSUM      (numeric)   sum of all elements" << endl;
    os << "  numeric GSUMSQR   (numeric)   sum of all squared elements   aka GSUMSQUARE" << endl;
    os << "  numeric GPRODUCT  (numeric)   product of all elements" << endl;
    os << "  real    GMIN      (real)      minimum of all elements" << endl;
    os << "  real    GMAX      (real)      maximum of all elements" << endl;
    os << "  numeric GMEAN     (numeric)   mean of all elements          aka GAVG" << endl;
    os << "  double  GVARIANCE (real)      variance" << endl;
    os << "  double  GSTDDEV   (real)      standard deviation" << endl;
    os << "  double  GAVDEV    (real)      average deviation" << endl;
    os << "  double  GRMS      (real)      root-mean-squares" << endl;
    os << "  double  GMEDIAN   (real)      median (the middle element)" << endl;
    os << "  double  GFRACTILE (real, fraction)   element at given fraction" << endl;
    os << endl;
    os << "The following functions results in an array and operate element by element" << endl;
    os << "  GANYS       GALLS       GNTRUES     GNFALSES" << endl;
    os << "  GSUMS       GSUMSQRS    GPRODUCTS   GMINS       GMAXS" << endl;
    os << "  GMEANS      GVARIANCES  GSTDDEVS    GAVDEVS     GRMSS" << endl;
    os << endl;
    os << "The following functions results in a scalar value" << endl;
    os << "  double  GHIST  (data, nbin, start, end)   histogram of the data" << endl;
    os << "  anytype GSTACK (anytype)    stack the data to an array      aka GAGGR" << endl;
    os << endl;
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
      os << "    UT1, UT        Universal Time" << endl;
      os << "    UT2            Universal Time" << endl;
      os << "    UTC            Coordinated Universal Time" << endl;
      os << "    TAI, IAT       International Atomic Time" << endl;
      os << "    TDT, TT, ET    Terrestrial Dynamical Time" << endl;
      os << "    TCG            Geocentric Coordinate Time" << endl;
      os << "    TDB            Barycentric Dynamical Time" << endl;
      os << "    TCB            Barycentric Coordinate Time" << endl;
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
