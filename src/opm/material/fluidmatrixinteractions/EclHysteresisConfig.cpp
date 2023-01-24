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

#include <config.h>
#include <opm/material/fluidmatrixinteractions/EclHysteresisConfig.hpp>

#include <opm/input/eclipse/EclipseState/Runspec.hpp>

namespace Opm {

EclHysteresisConfig EclHysteresisConfig::serializationTestObject()
{
    EclHysteresisConfig result;
    result.enableHysteresis_ = true;
    result.pcHysteresisModel_ = 1;
    result.krHysteresisModel_ = 2;
    result.modParamTrapped_ = 3.0;
    result.curvatureCapPrs_ = 4.0;

    return result;
}

void EclHysteresisConfig::initFromState(const Runspec& runspec)
{
    enableHysteresis_ = false;

    enableHysteresis_ = runspec.hysterPar().active();

    if (!enableHysteresis_)
        return;

    krHysteresisModel_ = runspec.hysterPar().krHysteresisModel();
    pcHysteresisModel_ = runspec.hysterPar().pcHysteresisModel();
    modParamTrapped_ = runspec.hysterPar().modParamTrapped();
    curvatureCapPrs_ = runspec.hysterPar().curvatureCapPrs();
}

bool EclHysteresisConfig::operator==(const EclHysteresisConfig& rhs) const
{
    return this->enableHysteresis_ == rhs.enableHysteresis_ &&
           this->pcHysteresisModel_ == rhs.pcHysteresisModel_ &&
           this->krHysteresisModel_ == rhs.krHysteresisModel_ &&
           this->modParamTrapped_== rhs.modParamTrapped_ &&
           this->curvatureCapPrs_ == rhs.curvatureCapPrs_;
}

} // namespace Opm
