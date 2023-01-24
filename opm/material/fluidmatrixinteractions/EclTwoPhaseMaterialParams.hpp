// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.

  Consult the COPYING file in the top-level source directory of this
  module for the precise wording of the license and the list of
  copyright holders.
*/
/*!
 * \file
 * \copydoc Opm::EclTwoPhaseMaterialParams
 */
#ifndef OPM_ECL_TWO_PHASE_MATERIAL_PARAMS_HPP
#define OPM_ECL_TWO_PHASE_MATERIAL_PARAMS_HPP

#include <memory>

#include <opm/material/common/EnsureFinalized.hpp>

namespace Opm {

enum class EclTwoPhaseApproach {
    GasOil,
    OilWater,
    GasWater
};

/*!
 * \brief Implementation for the parameters required by the material law for two-phase
 *        simulations.
 *
 * Essentially, this class just stores the two parameter objects for
 * the twophase capillary pressure laws.
 */
template<class Traits, class GasOilParamsT, class OilWaterParamsT, class GasWaterParamsT>
class EclTwoPhaseMaterialParams : public EnsureFinalized
{
    using Scalar = typename Traits::Scalar;
    enum { numPhases = 3 };
public:
    using EnsureFinalized :: finalize;

    using GasOilParams = GasOilParamsT;
    using OilWaterParams = OilWaterParamsT;
    using GasWaterParams = GasWaterParamsT;

    static EclTwoPhaseMaterialParams serializationTestObject()
    {
        EclTwoPhaseMaterialParams result;
        result.gasOilParams_ = std::make_shared<GasOilParams>(GasOilParams::serializationTestObject());
        result.oilWaterParams_ = std::make_shared<OilWaterParams>(OilWaterParams::serializationTestObject());
        result.gasWaterParams_ = std::make_shared<GasWaterParams>(GasWaterParams::serializationTestObject());
        result.approach = EclTwoPhaseApproach::OilWater;

        return result;
    }

    void setApproach(EclTwoPhaseApproach newApproach)
    { approach_ = newApproach; }

    EclTwoPhaseApproach approach() const
    { return approach_; }

    /*!
     * \brief The parameter object for the gas-oil twophase law.
     */
    const GasOilParams& gasOilParams() const
    { EnsureFinalized::check(); return *gasOilParams_; }

    /*!
     * \brief The parameter object for the gas-oil twophase law.
     */
    GasOilParams& gasOilParams()
    { EnsureFinalized::check(); return *gasOilParams_; }

    /*!
     * \brief Set the parameter object for the gas-oil twophase law.
     */
    void setGasOilParams(std::shared_ptr<GasOilParams> val)
    { gasOilParams_ = val; }

    /*!
     * \brief The parameter object for the oil-water twophase law.
     */
    const OilWaterParams& oilWaterParams() const
    { EnsureFinalized::check(); return *oilWaterParams_; }

    /*!
     * \brief The parameter object for the oil-water twophase law.
     */
    OilWaterParams& oilWaterParams()
    { EnsureFinalized::check(); return *oilWaterParams_; }

    /*!
     * \brief Set the parameter object for the oil-water twophase law.
     */
    void setOilWaterParams(std::shared_ptr<OilWaterParams> val)
    { oilWaterParams_ = val; }

  /*!
     * \brief The parameter object for the gas-water twophase law.
     */
    const GasWaterParams& gasWaterParams() const
    { EnsureFinalized::check(); return *gasWaterParams_; }

    /*!
     * \brief The parameter object for the gas-water twophase law.
     */
    GasWaterParams& gasWaterParams()
    { EnsureFinalized::check(); return *gasWaterParams_; }

    /*!
     * \brief Set the parameter object for the gas-water twophase law.
     */
    void setGasWaterParams(std::shared_ptr<GasWaterParams> val)
    { gasWaterParams_ = val; }

    bool operator==(const EclTwoPhaseMaterialParams& rhs) const
    {
        auto cmp_ptr = [](const auto& a, const auto& b)
        {
            if (!a && !b) {
                return true;
            }

            if (a && b) {
                return *a == *b;
            }

            return false;
        };

        return this->approach_ == rhs.approach_ &&
               cmp_ptr(this->gasOilParams_, rhs.gasOilParams_) &&
               cmp_ptr(this->oilWaterParams_, rhs.oilWaterParams_) &&
               cmp_ptr(this->gasWaterParams_, rhs.gasOilParams_);
    }

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(approach_);
        serializer(gasOilParams_);
        serializer(oilWaterParams_);
        serializer(gasWaterParams_);
    }
    
private:
    EclTwoPhaseApproach approach_;

    std::shared_ptr<GasOilParams> gasOilParams_;
    std::shared_ptr<OilWaterParams> oilWaterParams_;
    std::shared_ptr<GasWaterParams> gasWaterParams_;
};

} // namespace Opm

#endif
