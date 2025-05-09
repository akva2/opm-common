/*
  Copyright 2014 Statoil ASA.

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

#include "opm/input/eclipse/Deck/Deck.hpp"
#include "opm/input/eclipse/EclipseState/EclipseState.hpp"
#include "opm/input/eclipse/Parser/Parser.hpp"
#include <stdexcept>
#include <ostream>

#define BOOST_TEST_MODULE FaultTests

#include <boost/test/unit_test.hpp>

#include <opm/input/eclipse/EclipseState/Grid/FaultCollection.hpp>
#include <opm/input/eclipse/EclipseState/Grid/Fault.hpp>
#include <opm/input/eclipse/EclipseState/Grid/FaultFace.hpp>
#include <opm/input/eclipse/EclipseState/Grid/FaceDir.hpp>



BOOST_AUTO_TEST_CASE(CreateInvalidFace) {
    // I out of range
    BOOST_CHECK_THROW( Opm::FaultFace(10,10,10,10 , 10 , 1 , 1 , 5 , 5 , Opm::FaceDir::XPlus) , std::invalid_argument );

    // I1 != I2 when face == X
    BOOST_CHECK_THROW( Opm::FaultFace( 10,10,10, 1 , 3  , 1 , 1 , 5 , 5 , Opm::FaceDir::XPlus) , std::invalid_argument );

    // J1 < J2
    BOOST_CHECK_THROW( Opm::FaultFace(  10,10,10,3 , 3  , 3 , 1 , 5 , 5 , Opm::FaceDir::XPlus) , std::invalid_argument );

}


BOOST_AUTO_TEST_CASE(CreateFace) {
    Opm::FaultFace face1(10,10,10,0, 2  , 0 , 0 , 0 , 0 , Opm::FaceDir::YPlus);
    Opm::FaultFace face2(10,10,10,0, 2  , 1 , 1 , 0 , 0 , Opm::FaceDir::YPlus);
    Opm::FaultFace face3(10,10,10,0, 2  , 0 , 0 , 1 , 1 , Opm::FaceDir::YPlus);

    {
        const std::vector<size_t> trueValues1{0,1,2};
        const std::vector<size_t> trueValues2{10,11,12};
        const std::vector<size_t> trueValues3{100,101,102};
        auto iter3 = face3.begin();
        auto iter2 = face2.begin();
        size_t i = 0;
        for (auto iter1 = face1.begin(); iter1 != face1.end(); ++iter1) {
            size_t index1 = *iter1;
            size_t index2 = *iter2;
            size_t index3 = *iter3;

            BOOST_CHECK_EQUAL( index1 , trueValues1[i] );
            BOOST_CHECK_EQUAL( index2 , trueValues2[i] );
            BOOST_CHECK_EQUAL( index3 , trueValues3[i] );

            ++iter2;
            ++iter3;
            ++i;
        }
    }
    BOOST_CHECK_EQUAL( face1.getDir() , Opm::FaceDir::YPlus);
}


BOOST_AUTO_TEST_CASE(CreateFault) {
    Opm::Fault fault("FAULT1");
    BOOST_CHECK_EQUAL( "FAULT1" , fault.getName());
    BOOST_CHECK_EQUAL( 1.0 , fault.getTransMult());
}

namespace Opm {

inline std::ostream& operator<<( std::ostream& stream, const FaultFace& face ) {
    stream << face.getDir() << ": [ ";
    for( auto index : face ) stream << index << " ";
    return stream << "]";
}

}

BOOST_AUTO_TEST_CASE(AddFaceToFaults) {
    Opm::Fault fault("FAULT1");
    Opm::FaultFace face1( 10,10,10,0, 2  , 0 , 0 , 0 , 0 , Opm::FaceDir::YPlus );
    Opm::FaultFace face2( 10,10,10,0, 2  , 1 , 1 , 0 , 0 , Opm::FaceDir::YPlus );
    Opm::FaultFace face3( 10,10,10,0, 2  , 0 , 0 , 1 , 1 , Opm::FaceDir::YPlus );
    fault.addFace( face1 );
    fault.addFace( face2 );
    fault.addFace( face3 );

    {
        auto iter = fault.begin();
        BOOST_CHECK_EQUAL( *iter , face1 ); ++iter;
        BOOST_CHECK_EQUAL( *iter , face2 ); ++iter;
        BOOST_CHECK_EQUAL( *iter , face3 );
    }

}



BOOST_AUTO_TEST_CASE(CreateFaultCollection) {
    Opm::FaultCollection faults;
    BOOST_CHECK_EQUAL( faults.size() , 0U );
    BOOST_CHECK(! faults.hasFault("NO-NotThisOne"));
    BOOST_CHECK_THROW( faults.getFault("NO") , std::invalid_argument );
}


BOOST_AUTO_TEST_CASE(AddFaultsToCollection) {
    Opm::FaultCollection faults;

    faults.addFault("FAULT");
    BOOST_CHECK_EQUAL( faults.size() , 1U );
    BOOST_CHECK(faults.hasFault("FAULT"));

    const auto& fault1 = faults.getFault("FAULT");
    const auto& fault2 = faults.getFault(0);
    BOOST_CHECK_EQUAL(fault1.getName(), fault2.getName());

    faults.addFault("FAULTX");
    const auto& faultx = faults.getFault("FAULTX");
    BOOST_CHECK_EQUAL( faults.size() , 2U );
    BOOST_CHECK(faults.hasFault("FAULTX"));
    BOOST_CHECK_EQUAL( faultx.getName() , faults.getFault(1).getName());
}

BOOST_AUTO_TEST_CASE(GridOnly) {
    const std::string deck_string = R"(
RUNSPEC
DIMENS
  10 10 10 /
GRID
DX
1000*0.25 /
DY
1000*0.25 /
DZ
1000*0.25 /
TOPS
100*0.25 /
FAULTS
  'FLT1' 3 3 1 4 1 7 'X' /
  'FLT2' 1 8 4 4 1 7 'Y' /
/
MULTFLT
  'FLT1' 0.0001 /
  'FLT2' 0.0005 /
/
MULTFLT
  'FLT1' 0.001 /
/
)";

    Opm::Parser parser;
    Opm::Deck deck = parser.parseString(deck_string);
    Opm::EclipseState state(deck);
    const auto& flt1 = state.getFaults().getFault("FLT1");
    BOOST_CHECK_EQUAL(flt1.getTransMult(), 0.001);
    const auto& flt2 = state.getFaults().getFault("FLT2");
    BOOST_CHECK_EQUAL(flt2.getTransMult(), 0.0005);
}

BOOST_AUTO_TEST_CASE(Pattern) {
    const std::string deck_string = R"(
RUNSPEC
DIMENS
  10 10 10 /
GRID
DX
1000*0.25 /
DY
1000*0.25 /
DZ
1000*0.25 /
TOPS
100*0.25 /
FAULTS
  'FLT1' 3 3 1 4 1 7 'X' /
  'FLT2' 1 8 4 4 1 7 'Y' /
/
MULTFLT
  'FLT*' 0.0001 /
/
)";

    Opm::Parser parser;
    Opm::Deck deck = parser.parseString(deck_string);
    Opm::EclipseState state(deck);
    const auto& flt1 = state.getFaults().getFault("FLT1");
    BOOST_CHECK_EQUAL(flt1.getTransMult(), 0.0001);
    const auto& flt2 = state.getFaults().getFault("FLT2");
    BOOST_CHECK_EQUAL(flt2.getTransMult(), 0.0001);
}

BOOST_AUTO_TEST_CASE(PatternTrunc) {
    const std::string deck_string = R"(
RUNSPEC
DIMENS
  10 10 10 /
GRID
DX
1000*0.25 /
DY
1000*0.25 /
DZ
1000*0.25 /
TOPS
100*0.25 /
FAULTS
  'FLT11' 3 3 1 4 1 7 'X' /
  'FLT12' 3 3 1 4 1 7 'X' /
  'FLT22' 1 8 4 4 1 7 'Y' /
/
MULTFLT
  'FLT*1' 0.0001 /
  'FLT2*' 0.0005 /
/
)";

    Opm::Parser parser;
    Opm::Deck deck = parser.parseString(deck_string);
    Opm::EclipseState state(deck);
    const auto& flt1 = state.getFaults().getFault("FLT11");
    BOOST_CHECK_EQUAL(flt1.getTransMult(), 0.0001);
    const auto& flt12 = state.getFaults().getFault("FLT12");
    BOOST_CHECK_EQUAL(flt12.getTransMult(), 0.0001);
    const auto& flt2 = state.getFaults().getFault("FLT22");
    BOOST_CHECK_EQUAL(flt2.getTransMult(), 0.0005);
}

BOOST_AUTO_TEST_CASE(EditOnly) {
    const std::string deck_string = R"(
RUNSPEC
DIMENS
  10 10 10 /
GRID
DX
1000*0.25 /
DY
1000*0.25 /
DZ
1000*0.25 /
TOPS
100*0.25 /
FAULTS
  'FLT1' 3 3 1 4 1 7 'X' /
  'FLT2' 1 8 4 4 1 7 'Y' /
/
EDIT
MULTFLT
  'FLT1' 0.0001 /
  'FLT2' 0.0005 /
/
MULTFLT
  'FLT1' 0.001 /
/
)";

    Opm::Parser parser;
    Opm::Deck deck = parser.parseString(deck_string);
    Opm::EclipseState state(deck);
    const auto& flt1 = state.getFaults().getFault("FLT1");
    BOOST_CHECK_EQUAL(flt1.getTransMult(), 0.001);
    const auto& flt2 = state.getFaults().getFault("FLT2");
    BOOST_CHECK_EQUAL(flt2.getTransMult(), 0.0005);
}

BOOST_AUTO_TEST_CASE(GridAndEdit) {
    const std::string deck_string = R"(
RUNSPEC
DIMENS
  10 10 10 /
GRID
DX
1000*0.25 /
DY
1000*0.25 /
DZ
1000*0.25 /
TOPS
100*0.25 /
FAULTS
  'FLT1' 3 3 1 4 1 7 'X' /
  'FLT2' 1 8 4 4 1 7 'Y' /
/
MULTFLT
  'FLT1' 0.0001 /
/
EDIT
MULTFLT
  'FLT1' 20 /
  'FLT2' 0.0005 /
/
)";

    Opm::Parser parser;
    Opm::Deck deck = parser.parseString(deck_string);
    Opm::EclipseState state(deck);
    const auto& flt1 = state.getFaults().getFault("FLT1");
    BOOST_CHECK_EQUAL(flt1.getTransMult(), 0.002);
    const auto& flt2 = state.getFaults().getFault("FLT2");
    BOOST_CHECK_EQUAL(flt2.getTransMult(), 0.0005);
}

BOOST_AUTO_TEST_CASE(GridAndEdit2) {
    const std::string deck_string = R"(
RUNSPEC
DIMENS
  10 10 10 /
GRID
DX
1000*0.25 /
DY
1000*0.25 /
DZ
1000*0.25 /
TOPS
100*0.25 /
FAULTS
  'FLT1' 3 3 1 4 1 7 'X' /
/
MULTFLT
  'FLT1' 5.0 /
  'FLT1' 0.0001 /
/
EDIT
MULTFLT
  'FLT1' 0.0005 /
  'FLT1' 20 /
/
)";

    Opm::Parser parser;
    Opm::Deck deck = parser.parseString(deck_string);
    Opm::EclipseState state(deck);
    const auto& flt1 = state.getFaults().getFault("FLT1");
    BOOST_CHECK_EQUAL(flt1.getTransMult(), 0.002);
}

BOOST_AUTO_TEST_CASE(GridAndEditPattern) {
    const std::string deck_string = R"(
RUNSPEC
DIMENS
  10 10 10 /
GRID
DX
1000*0.25 /
DY
1000*0.25 /
DZ
1000*0.25 /
TOPS
100*0.25 /
FAULTS
  'FLT11' 3 3 1 4 1 7 'X' /
  'FLT12' 3 3 1 4 1 7 'X' /
  'FLT22' 1 8 4 4 1 7 'Y' /
/
MULTFLT
  'FLT*1' 0.0001 /
/
EDIT
MULTFLT
  'FLT1*' 20 /
  'FLT2*' 0.0005 /
/
)";

    Opm::Parser parser;
    Opm::Deck deck = parser.parseString(deck_string);
    Opm::EclipseState state(deck);
    const auto& flt1 = state.getFaults().getFault("FLT11");
    BOOST_CHECK_EQUAL(flt1.getTransMult(), 0.0001 * 20);
    const auto& flt12 = state.getFaults().getFault("FLT12");
    BOOST_CHECK_EQUAL(flt12.getTransMult(), 0.0001 * 20);
    const auto& flt2 = state.getFaults().getFault("FLT22");
    BOOST_CHECK_EQUAL(flt2.getTransMult(), 0.0001 * 0.0005);
}
