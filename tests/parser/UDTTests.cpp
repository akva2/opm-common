/*
  Copyright 2018 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#define BOOST_TEST_MODULE UDTTests

#include <boost/test/unit_test.hpp>

#include <opm/input/eclipse/Deck/Deck.hpp>
#include <opm/input/eclipse/EclipseState/EclipseState.hpp>
#include <opm/input/eclipse/EclipseState/Runspec.hpp>
#include <opm/input/eclipse/Parser/Parser.hpp>
#include <opm/input/eclipse/Python/Python.hpp>
#include <opm/input/eclipse/Schedule/Schedule.hpp>
#include <opm/input/eclipse/Schedule/SummaryState.hpp>
#include <opm/input/eclipse/Schedule/UDQ/UDQConfig.hpp>
#include <opm/input/eclipse/Schedule/UDQ/UDQState.hpp>
#include <opm/input/eclipse/Schedule/UDQ/UDTParams.hpp>
#include <opm/input/eclipse/Schedule/UDQ/UDT.hpp>
#include <opm/input/eclipse/Schedule/Well/WellMatcher.hpp>

using namespace Opm;


BOOST_AUTO_TEST_CASE(Dimensions)
{
    const std::string input = R"(
RUNSPEC
UDTDIMS
  1 10 10 1 /
)";
    Parser parser;

    auto deck = parser.parseString(input);
    auto runspec = Runspec(deck);
    const auto& udt_params = runspec.udtParams();

    BOOST_CHECK_EQUAL(udt_params.maxDimensions(), 1);
}


BOOST_AUTO_TEST_CASE(DimensionsFail)
{
    const std::string input = R"(
RUNSPEC
UDTDIMS
  1 10 10 2 /
)";
    Parser parser;

    auto deck = parser.parseString(input);
    BOOST_CHECK_THROW(auto runspec = Runspec(deck), std::invalid_argument);
}


BOOST_AUTO_TEST_CASE(ParseUDT)
{
    const std::string input = R"(
RUNSPEC
UDTDIMS
  1 10 10 1 /
SCHEDULE
UDT
  'TEST1' 1/
  'LC' 100.0 500.0 /
  100.0 180.0 /
/
/
)";
    Parser parser;
    auto deck = parser.parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table(deck);
    FieldPropsManager fp(deck, Phases{true, true, true}, grid, table);
    Runspec runspec(deck);
    BOOST_CHECK_NO_THROW(Schedule schedule(deck, grid, fp, runspec, std::make_shared<Python>()));
}


BOOST_AUTO_TEST_CASE(UDT_NV)
{
    UDT udt({1.0, 4.0, 5.0}, {5.0, 10.0, 11.0}, UDT::InterpolationType::NearestNeighbour);

    BOOST_CHECK_EQUAL(udt(0.0), 5.0);
    BOOST_CHECK_EQUAL(udt(1.5), 5.0);
    BOOST_CHECK_EQUAL(udt(4.0), 10.0);
    BOOST_CHECK_EQUAL(udt(4.7), 11.0);
    BOOST_CHECK_EQUAL(udt(5.2), 11.0);
}


BOOST_AUTO_TEST_CASE(UDT_LC)
{
    UDT udt({1.0, 4.0, 5.0}, {5.0, 10.0, 11.0}, UDT::InterpolationType::LinearClamp);

    BOOST_CHECK_EQUAL(udt(0.0), 5.0);
    BOOST_CHECK_EQUAL(udt(1.5), 5.0 + (10.0-5.0) * (1.5-1.0) / (4.0-1.0));
    BOOST_CHECK_EQUAL(udt(4.0), 10.0);
    BOOST_CHECK_EQUAL(udt(4.7), 10.0 + (11.0-10.0) * (4.7-4.0) / (5.0-4.0));
    BOOST_CHECK_EQUAL(udt(5.2), 11.0);
}


BOOST_AUTO_TEST_CASE(UDT_LL)
{
    UDT udt({1.0, 4.0, 5.0}, {5.0, 10.0, 11.0}, UDT::InterpolationType::LinearExtrapolate);

    BOOST_CHECK_EQUAL(udt(0.0), 5.0 + (10.0-5.0)* (0.0-1.0) / (4.0-1.0));
    BOOST_CHECK_EQUAL(udt(1.5), 5.0 + (10.0-5.0) * (1.5-1.0) / (4.0-1.0));
    BOOST_CHECK_EQUAL(udt(4.0), 10.0);
    BOOST_CHECK_EQUAL(udt(4.7), 10.0 + (11.0-10.0) * (4.7-4.0) / (5.0-4.0));
    BOOST_CHECK_EQUAL(udt(5.2), 10.0 + (11.0-10.0) * (5.2-4.0) / (5.0-4.0));
}


BOOST_AUTO_TEST_CASE(EvalUDT)
{
    const std::string input = R"(
RUNSPEC
UDTDIMS
  1 10 10 1 /
SCHEDULE
WELSPECS
   'PROD1'   'TEST'   5   1  1*       'OIL'  2*      'STOP'  4* /
   'PROD2'   'TEST'   1   1  1*       'OIL'  2*      'STOP'  4* /
/
COMPDAT
   'PROD1'   5  1   1    2    'OPEN'   1*     1*    0.216       /
   'PROD2'   1  1   1    1    'OPEN'   1*     1*    0.216       /
/
WCONPROD
  'PROD1'  'OPEN'  ORAT   500.0   4*   180.0 /
  'PROD2'  'OPEN'  ORAT   500.0   4*   180.0 /
/
UDT
  'TEST1' 1/
  'LC' 100.0 500.0 /
  100.0 180.0 /
/
/
UDQ
ASSIGN WU_WBHP 0 /
DEFINE WU_WBHP0 WU_WBHP /
DEFINE WU_WBHP (TU_FBHP[WOPR] UMIN WBHP) UMIN WU_WBHP0 /
/
)";
    Parser parser;
    auto deck = parser.parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table(deck);
    FieldPropsManager fp(deck, Phases{true, true, true}, grid, table);
    Runspec runspec(deck);
    Schedule schedule(deck, grid, fp, runspec, std::make_shared<Python>());
    SummaryState summaryState;
    UDQState udq_state;
    schedule[0].udq().eval_assign(0,
                    schedule,
                    schedule.wellMatcher(0),
                    schedule.segmentMatcherFactory(0),
                    summaryState,
                    udq_state);
}
