/*
 Copyright 2023 SINTEF Digital

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

#ifndef OPM_UDT_PARAMS_HPP
#define OPM_UDT_PARAMS_HPP


namespace Opm {

class Deck;

class UDTParams
{
public:
    UDTParams() = default;
    explicit UDTParams(const Deck& deck);

    static UDTParams serializationTestObject();

    bool operator==(const UDTParams&) const;

    int maxDimensions() const { return max_dimensions_; }

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(max_dimensions_);
    }

private:
    int max_dimensions_ = 1; //!< Maximum number of dimensions
};

}

#endif
