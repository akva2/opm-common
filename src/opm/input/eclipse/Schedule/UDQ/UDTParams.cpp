/*
  Copyright 2023 SINTEF Digital

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  OPM is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <opm/input/eclipse/Schedule/UDQ/UDTParams.hpp>

#include <opm/input/eclipse/Deck/Deck.hpp>
#include <opm/input/eclipse/Parser/ParserKeywords/U.hpp>

namespace Opm {

/*
  The UDTDIMS keyword contain a list of MAX_ items which probably
  originate in an ECLIPSE implementation detail. Our implementation does not
  require these max values, we therefor ignore them completely.
*/

UDTParams::UDTParams(const Deck& deck)
    : UDTParams()
{
    if (deck.hasKeyword<ParserKeywords::UDTDIMS>()) {
        const auto& record = deck["UDTDIMS"].back().getRecord(0);
        max_dimensions_  = record.getItem(ParserKeywords::UDTDIMS::MAX_DIMENSIONS::itemName).get<int>(0);
        if (max_dimensions_ != 1) {
            throw std::invalid_argument("Only 1D UDTs are supported");
        }
    }
}

UDTParams UDTParams::serializationTestObject()
{
    UDTParams result;
    result.max_dimensions_ = 2;

    return result;
}

bool UDTParams::operator==(const UDTParams& data) const
{
    return this->max_dimensions_ == data.max_dimensions_;
}

}
