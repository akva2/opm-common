/*
  Copyright 2020 Statoil ASA.

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

#include <opm/common/OpmLog/OpmLog.hpp>
#include <opm/common/utility/OpmInputError.hpp>

#include <opm/input/eclipse/Deck/DeckItem.hpp>
#include <opm/input/eclipse/Deck/DeckRecord.hpp>

#include <opm/input/eclipse/EclipseState/Aquifer/AquiferFlux.hpp>

#include <opm/input/eclipse/Parser/ParserKeywords/D.hpp>
#include <opm/input/eclipse/Parser/ParserKeywords/E.hpp>
#include <opm/input/eclipse/Parser/ParserKeywords/L.hpp>
#include <opm/input/eclipse/Parser/ParserKeywords/N.hpp>


#include <opm/input/eclipse/Schedule/Action/SimulatorUpdate.hpp>
#include <opm/input/eclipse/Schedule/Events.hpp>
#include <opm/input/eclipse/Schedule/GasLiftOpt.hpp>
#include <opm/input/eclipse/Schedule/HandlerContext.hpp>
#include <opm/input/eclipse/Schedule/OilVaporizationProperties.hpp>
#include <opm/input/eclipse/Schedule/RPTConfig.hpp>
#include <opm/input/eclipse/Schedule/Schedule.hpp>
#include <opm/input/eclipse/Schedule/ScheduleState.hpp>
#include <opm/input/eclipse/Schedule/Tuning.hpp>

#include <algorithm>
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <fmt/format.h>

namespace Opm {

bool handleGroupKeyword(HandlerContext& handlerContext);
bool handleMSWKeyword(HandlerContext& handlerContext);
bool handleNetworkKeyword(HandlerContext& handlerContext);
bool handleUDQKeyword(HandlerContext& handlerContext);
bool handleWellKeyword(HandlerContext& handlerContext);

namespace {

void handleAQUCT(HandlerContext& handlerContext)
{
    throw OpmInputError("AQUCT is not supported as SCHEDULE keyword",
                        handlerContext.keyword.location());
}

void handleAQUFETP(HandlerContext& handlerContext)
{
    throw OpmInputError("AQUFETP is not supported as SCHEDULE keyword",
                        handlerContext.keyword.location());
}

void handleAQUFLUX(HandlerContext& handlerContext)
{
    auto& aqufluxs = handlerContext.state().aqufluxs;
    for (const auto& record : handlerContext.keyword) {
        const auto aquifer = SingleAquiferFlux { record };
        aqufluxs.insert_or_assign(aquifer.id, aquifer);
    }
}

void handleBCProp(HandlerContext& handlerContext)
{
    auto& bcprop = handlerContext.state().bcprop;
    for (const auto& record : handlerContext.keyword) {
        bcprop.updateBCProp(record);
    }
}

void handleDRSDT(HandlerContext& handlerContext)
{
    std::size_t numPvtRegions = handlerContext.runspec().tabdims().getNumPVTTables();
    std::vector<double> maximums(numPvtRegions);
    std::vector<std::string> options(numPvtRegions);
    for (const auto& record : handlerContext.keyword) {
        const auto& max = record.getItem<ParserKeywords::DRSDT::DRSDT_MAX>().getSIDouble(0);
        const auto& option = record.getItem<ParserKeywords::DRSDT::OPTION>().get< std::string >(0);
        std::fill(maximums.begin(), maximums.end(), max);
        std::fill(options.begin(), options.end(), option);
        auto& ovp = handlerContext.state().oilvap();
        OilVaporizationProperties::updateDRSDT(ovp, maximums, options);
    }
}

void handleDRSDTR(HandlerContext& handlerContext)
{
    std::size_t numPvtRegions = handlerContext.runspec().tabdims().getNumPVTTables();
    std::vector<double> maximums(numPvtRegions);
    std::vector<std::string> options(numPvtRegions);
    std::size_t pvtRegionIdx = 0;
    for (const auto& record : handlerContext.keyword) {
        const auto& max = record.getItem<ParserKeywords::DRSDTR::DRSDT_MAX>().getSIDouble(0);
        const auto& option = record.getItem<ParserKeywords::DRSDTR::OPTION>().get< std::string >(0);
        maximums[pvtRegionIdx] = max;
        options[pvtRegionIdx] = option;
        pvtRegionIdx++;
    }
    auto& ovp = handlerContext.state().oilvap();
    OilVaporizationProperties::updateDRSDT(ovp, maximums, options);
}

void handleDRSDTCON(HandlerContext& handlerContext)
{
    std::size_t numPvtRegions = handlerContext.runspec().tabdims().getNumPVTTables();
    std::vector<double> maximums(numPvtRegions);
    std::vector<std::string> options(numPvtRegions);
    for (const auto& record : handlerContext.keyword) {
        const auto& max = record.getItem<ParserKeywords::DRSDTCON::DRSDT_MAX>().getSIDouble(0);
        const auto& option = record.getItem<ParserKeywords::DRSDTCON::OPTION>().get< std::string >(0);
        std::fill(maximums.begin(), maximums.end(), max);
        std::fill(options.begin(), options.end(), option);
        auto& ovp = handlerContext.state().oilvap();
        OilVaporizationProperties::updateDRSDTCON(ovp, maximums, options);
    }
}

void handleDRVDT(HandlerContext& handlerContext)
{
    std::size_t numPvtRegions = handlerContext.runspec().tabdims().getNumPVTTables();
    std::vector<double> maximums(numPvtRegions);
    for (const auto& record : handlerContext.keyword) {
        const auto& max = record.getItem<ParserKeywords::DRVDTR::DRVDT_MAX>().getSIDouble(0);
        std::fill(maximums.begin(), maximums.end(), max);
        auto& ovp = handlerContext.state().oilvap();
        OilVaporizationProperties::updateDRVDT(ovp, maximums);
    }
}

void handleDRVDTR(HandlerContext& handlerContext)
{
    std::size_t numPvtRegions = handlerContext.runspec().tabdims().getNumPVTTables();
    std::size_t pvtRegionIdx = 0;
    std::vector<double> maximums(numPvtRegions);
    for (const auto& record : handlerContext.keyword) {
        const auto& max = record.getItem<ParserKeywords::DRVDTR::DRVDT_MAX>().getSIDouble(0);
        maximums[pvtRegionIdx] = max;
        pvtRegionIdx++;
    }
    auto& ovp = handlerContext.state().oilvap();
    OilVaporizationProperties::updateDRVDT(ovp, maximums);
}

void handleEXIT(HandlerContext& handlerContext)
{
    if (handlerContext.actionx_mode) {
        using ES = ParserKeywords::EXIT;
        int status = handlerContext.keyword.getRecord(0).getItem<ES::STATUS_CODE>().get<int>(0);
        OpmLog::info("Simulation exit with status: " +
                     std::to_string(status) +
                     " requested as part of ACTIONX at report_step: " +
                     std::to_string(handlerContext.currentStep));
        handlerContext.setExitCode(status);
    }
}

void handleGEOKeyword(HandlerContext& handlerContext)
{
    handlerContext.state().geo_keywords().push_back(handlerContext.keyword);
    handlerContext.state().events().addEvent( ScheduleEvents::GEO_MODIFIER );
    if (handlerContext.sim_update) {
        handlerContext.sim_update->tran_update = true;
    }
}

void handleLIFTOPT(HandlerContext& handlerContext)
{
    auto glo = handlerContext.state().glo();

    const auto& record = handlerContext.keyword.getRecord(0);

    const double gaslift_increment = record.getItem<ParserKeywords::LIFTOPT::INCREMENT_SIZE>().getSIDouble(0);
    const double min_eco_gradient = record.getItem<ParserKeywords::LIFTOPT::MIN_ECONOMIC_GRADIENT>().getSIDouble(0);
    const double min_wait = record.getItem<ParserKeywords::LIFTOPT::MIN_INTERVAL_BETWEEN_GAS_LIFT_OPTIMIZATIONS>().getSIDouble(0);
    const bool all_newton = DeckItem::to_bool( record.getItem<ParserKeywords::LIFTOPT::OPTIMISE_ALL_ITERATIONS>().get<std::string>(0) );

    glo.gaslift_increment(gaslift_increment);
    glo.min_eco_gradient(min_eco_gradient);
    glo.min_wait(min_wait);
    glo.all_newton(all_newton);

    handlerContext.state().glo.update( std::move(glo) );
}

void handleMESSAGES(HandlerContext& handlerContext)
{
    handlerContext.state().message_limits().update( handlerContext.keyword );
}

void handleMXUNSUPP(HandlerContext& handlerContext)
{
    std::string msg_fmt = fmt::format("Problem with keyword {{keyword}} at report step {}\n"
                                      "In {{file}} line {{line}}\n"
                                      "OPM does not support grid property modifier {} "
                                      "in the Schedule section",
                                      handlerContext.currentStep,
                                      handlerContext.keyword.name());
    OpmLog::warning(OpmInputError::format(msg_fmt, handlerContext.keyword.location()));
}

void handleNEXTSTEP(HandlerContext& handlerContext)
{
    const auto& record = handlerContext.keyword[0];
    auto next_tstep = record.getItem<ParserKeywords::NEXTSTEP::MAX_STEP>().getSIDouble(0);
    auto apply_to_all = DeckItem::to_bool( record.getItem<ParserKeywords::NEXTSTEP::APPLY_TO_ALL>().get<std::string>(0) );

    handlerContext.state().next_tstep = NextStep{next_tstep, apply_to_all};
    handlerContext.state().events().addEvent(ScheduleEvents::TUNING_CHANGE);
}

void handleNUPCOL(HandlerContext& handlerContext)
{
    const int nupcol = handlerContext.keyword.getRecord(0).getItem("NUM_ITER").get<int>(0);

    if (handlerContext.keyword.getRecord(0).getItem("NUM_ITER").defaultApplied(0)) {
        std::string msg = "OPM Flow uses 12 as default NUPCOL value";
        OpmLog::note(msg);
    }

    handlerContext.state().update_nupcol(nupcol);
}

void handleRPTONLY(HandlerContext& handlerContext)
{
    handlerContext.state().rptonly(true);
}

void handleRPTONLYO(HandlerContext& handlerContext)
{
    handlerContext.state().rptonly(false);
}

void handleRPTRST(HandlerContext& handlerContext)
{
    auto rst_config = handlerContext.state().rst_config();
    rst_config.update(handlerContext.keyword, handlerContext.parseContext, handlerContext.errors);
    handlerContext.state().rst_config.update(std::move(rst_config));
}

void handleRPTSCHED(HandlerContext& handlerContext)
{
    handlerContext.state().rpt_config.update( RPTConfig(handlerContext.keyword ));
    auto rst_config = handlerContext.state().rst_config();
    rst_config.update(handlerContext.keyword, handlerContext.parseContext, handlerContext.errors);
    handlerContext.state().rst_config.update(std::move(rst_config));
}

/*
  We do not really handle the SAVE keyword, we just interpret it as: Write a
  normal restart file at this report step.
*/
void handleSAVE(HandlerContext& handlerContext)
{
    handlerContext.state(handlerContext.currentStep).updateSAVE(true);
}

void handleSUMTHIN(HandlerContext& handlerContext)
{
    auto value = handlerContext.keyword.getRecord(0).getItem(0).getSIDouble(0);
    handlerContext.state().update_sumthin( value );
}

void handleTUNING(HandlerContext& handlerContext)
{
    const auto numrecords = handlerContext.keyword.size();
    auto tuning = handlerContext.state().tuning();

    // local helpers
    auto nondefault_or_previous_double = [](const Opm::DeckRecord& rec, const std::string& item_name, double previous_value) {
        const auto& deck_item = rec.getItem(item_name);
        return deck_item.defaultApplied(0) ? previous_value : rec.getItem(item_name).get< double >(0);
    };

    auto nondefault_or_previous_int = [](const Opm::DeckRecord& rec, const std::string& item_name, int previous_value) {
        const auto& deck_item = rec.getItem(item_name);
        return deck_item.defaultApplied(0) ? previous_value : rec.getItem(item_name).get< int >(0);
    };

    auto nondefault_or_previous_sidouble = [](const Opm::DeckRecord& rec, const std::string& item_name, double previous_value) {
        const auto& deck_item = rec.getItem(item_name);
        return deck_item.defaultApplied(0) ? previous_value : rec.getItem(item_name).getSIDouble(0);
    };

    // \Note No TSTINIT value should not be used unless explicitly non-defaulted, hence removing value by default
    // \Note (exception is the first time step, which is handled by the Tuning constructor)
    tuning.TSINIT = std::nullopt;

    if (numrecords > 0) {
        const auto& record1 = handlerContext.keyword.getRecord(0);

        // \Note A value indicates TSINIT was set in this record
        if (const auto& deck_item = record1.getItem("TSINIT"); !deck_item.defaultApplied(0))
            tuning.TSINIT = std::optional<double>{ record1.getItem("TSINIT").getSIDouble(0) };

        tuning.TSMAXZ = nondefault_or_previous_sidouble(record1, "TSMAXZ", tuning.TSMAXZ);
        tuning.TSMINZ = nondefault_or_previous_sidouble(record1, "TSMINZ", tuning.TSMINZ);
        tuning.TSMCHP = nondefault_or_previous_sidouble(record1, "TSMCHP", tuning.TSMCHP);
        tuning.TSFMAX = nondefault_or_previous_double(record1, "TSFMAX", tuning.TSFMAX);
        tuning.TSFMIN = nondefault_or_previous_double(record1, "TSFMIN", tuning.TSFMIN);
        tuning.TSFCNV = nondefault_or_previous_double(record1, "TSFCNV", tuning.TSFCNV);
        tuning.TFDIFF = nondefault_or_previous_double(record1, "TFDIFF", tuning.TFDIFF);
        tuning.THRUPT = nondefault_or_previous_double(record1, "THRUPT", tuning.THRUPT);

        const auto& TMAXWCdeckItem = record1.getItem("TMAXWC");
        if (TMAXWCdeckItem.hasValue(0)) {
            tuning.TMAXWC_has_value = true;
            tuning.TMAXWC = nondefault_or_previous_sidouble(record1, "TMAXWC", tuning.TMAXWC);
        }
    }

    if (numrecords > 1) {
        const auto& record2 = handlerContext.keyword.getRecord(1);

        tuning.TRGTTE = nondefault_or_previous_double(record2, "TRGTTE", tuning.TRGTTE);
        tuning.TRGCNV = nondefault_or_previous_double(record2, "TRGCNV", tuning.TRGCNV);
        tuning.TRGMBE = nondefault_or_previous_double(record2, "TRGMBE", tuning.TRGMBE);
        tuning.TRGLCV = nondefault_or_previous_double(record2, "TRGLCV", tuning.TRGLCV);
        tuning.XXXTTE = nondefault_or_previous_double(record2, "XXXTTE", tuning.XXXTTE);
        tuning.XXXCNV = nondefault_or_previous_double(record2, "XXXCNV", tuning.XXXCNV);

        tuning.XXXMBE = nondefault_or_previous_double(record2, "XXXMBE", tuning.XXXMBE);

        tuning.XXXLCV = nondefault_or_previous_double(record2, "XXXLCV", tuning.XXXLCV);
        tuning.XXXWFL = nondefault_or_previous_double(record2, "XXXWFL", tuning.XXXWFL);
        tuning.TRGFIP = nondefault_or_previous_double(record2, "TRGFIP", tuning.TRGFIP);

        const auto& TRGSFTdeckItem = record2.getItem("TRGSFT");
        if (TRGSFTdeckItem.hasValue(0)) {
            tuning.TRGSFT_has_value = true;
            tuning.TRGSFT = nondefault_or_previous_double(record2, "TRGSFT", tuning.TRGSFT);
        }

        tuning.THIONX = nondefault_or_previous_double(record2, "THIONX", tuning.THIONX);
        tuning.TRWGHT = nondefault_or_previous_int(record2, "TRWGHT", tuning.TRWGHT);
    }

    if (numrecords > 2) {
        const auto& record3 = handlerContext.keyword.getRecord(2);

        tuning.NEWTMX = nondefault_or_previous_int(record3, "NEWTMX", tuning.NEWTMX);
        tuning.NEWTMN = nondefault_or_previous_int(record3, "NEWTMN", tuning.NEWTMN);
        tuning.LITMAX = nondefault_or_previous_int(record3, "LITMAX", tuning.LITMAX);
        tuning.LITMIN = nondefault_or_previous_int(record3, "LITMIN", tuning.LITMIN);
        tuning.MXWSIT = nondefault_or_previous_int(record3, "MXWSIT", tuning.MXWSIT);
        tuning.MXWPIT = nondefault_or_previous_int(record3, "MXWPIT", tuning.MXWPIT);
        tuning.DDPLIM = nondefault_or_previous_sidouble(record3, "DDPLIM", tuning.DDPLIM);
        tuning.DDSLIM = nondefault_or_previous_double(record3, "DDSLIM", tuning.DDSLIM);
        tuning.TRGDPR = nondefault_or_previous_sidouble(record3, "TRGDPR", tuning.TRGDPR);

        const auto& XXXDPRdeckItem = record3.getItem("XXXDPR");
        if (XXXDPRdeckItem.hasValue(0)) {
            tuning.XXXDPR_has_value = true;
            tuning.XXXDPR = nondefault_or_previous_sidouble(record3, "XXXDPR", tuning.XXXDPR);
        }
    }

    handlerContext.state().update_tuning( std::move( tuning ));
    handlerContext.state().events().addEvent(ScheduleEvents::TUNING_CHANGE);
}

void handleVAPPARS(HandlerContext& handlerContext)
{
    for (const auto& record : handlerContext.keyword) {
        double vap1 = record.getItem("OIL_VAP_PROPENSITY").get< double >(0);
        double vap2 = record.getItem("OIL_DENSITY_PROPENSITY").get< double >(0);
        auto& ovp = handlerContext.state().oilvap();
        OilVaporizationProperties::updateVAPPARS(ovp, vap1, vap2);
    }
}

void handleVFPINJ(HandlerContext& handlerContext)
{
    auto table = VFPInjTable(handlerContext.keyword, handlerContext.unitSystem());
    handlerContext.state().events().addEvent( ScheduleEvents::VFPINJ_UPDATE );
    handlerContext.state().vfpinj.update( std::move(table) );
}

void handleVFPPROD(HandlerContext& handlerContext)
{
    auto table = VFPProdTable(handlerContext.keyword,
                              handlerContext.gasLiftOptActive(),
                              handlerContext.unitSystem());
    handlerContext.state().events().addEvent( ScheduleEvents::VFPPROD_UPDATE );
    handlerContext.state().vfpprod.update( std::move(table) );
}

}

bool Schedule::handleNormalKeyword(HandlerContext& handlerContext)
{
    if (handleGroupKeyword(handlerContext)) {
        return true;
    }
    if (handleMSWKeyword(handlerContext)) {
        return true;
    }
    if (handleNetworkKeyword(handlerContext)) {
        return true;
    }
    if (handleUDQKeyword(handlerContext)) {
        return true;
    }
    if (handleWellKeyword(handlerContext)) {
        return true;
    }

    using handler_function = std::function<void(HandlerContext&)>;
    static const std::unordered_map<std::string, handler_function> handler_functions = {
        { "AQUCT"   , &handleAQUCT     },
        { "AQUFETP" , &handleAQUFETP   },
        { "AQUFLUX" , &handleAQUFLUX   },
        { "BCPROP"  , &handleBCProp    },
        { "BOX"     , &handleGEOKeyword},
        { "DRSDT"   , &handleDRSDT     },
        { "DRSDTCON", &handleDRSDTCON  },
        { "DRSDTR"  , &handleDRSDTR    },
        { "DRVDT"   , &handleDRVDT     },
        { "DRVDTR"  , &handleDRVDTR    },
        { "ENDBOX"  , &handleGEOKeyword},
        { "EXIT",     &handleEXIT      },
        { "LIFTOPT" , &handleLIFTOPT   },
        { "MESSAGES", &handleMESSAGES  },
        { "MULTFLT" , &handleGEOKeyword},
        { "MULTPV"  , &handleMXUNSUPP  },
        { "MULTR"   , &handleMXUNSUPP  },
        { "MULTR-"  , &handleMXUNSUPP  },
        { "MULTREGT", &handleMXUNSUPP  },
        { "MULTSIG" , &handleMXUNSUPP  },
        { "MULTSIGV", &handleMXUNSUPP  },
        { "MULTTHT" , &handleMXUNSUPP  },
        { "MULTTHT-", &handleMXUNSUPP  },
        { "MULTX"   , &handleGEOKeyword},
        { "MULTX-"  , &handleGEOKeyword},
        { "MULTY"   , &handleGEOKeyword},
        { "MULTY-"  , &handleGEOKeyword},
        { "MULTZ"   , &handleGEOKeyword},
        { "MULTZ-"  , &handleGEOKeyword},
        { "NEXT"    , &handleNEXTSTEP  },
        { "NEXTSTEP", &handleNEXTSTEP  },
        { "NUPCOL"  , &handleNUPCOL    },
        { "RPTONLY" , &handleRPTONLY   },
        { "RPTONLYO", &handleRPTONLYO  },
        { "RPTRST"  , &handleRPTRST    },
        { "RPTSCHED", &handleRPTSCHED  },
        { "SAVE"    , &handleSAVE      },
        { "SUMTHIN" , &handleSUMTHIN   },
        { "TUNING"  , &handleTUNING    },
        { "VAPPARS" , &handleVAPPARS   },
        { "VFPINJ"  , &handleVFPINJ    },
        { "VFPPROD" , &handleVFPPROD   },
    };

    auto function_iterator = handler_functions.find(handlerContext.keyword.name());
    if (function_iterator == handler_functions.end()) {
        return false;
    }

    try {
        function_iterator->second(handlerContext);
    } catch (const OpmInputError&) {
        throw;
    } catch (const std::logic_error& e) {
        // Rethrow as OpmInputError to provide more context,
        // but add "Internal error: " to the string, as that
        // is what logic_error signifies.
        const OpmInputError opm_error { std::string("Internal error: ") + e.what(), handlerContext.keyword.location() } ;
        OpmLog::error(opm_error.what());
        std::throw_with_nested(opm_error);
    } catch (const std::exception& e) {
        // Rethrow as OpmInputError to provide more context.
        const OpmInputError opm_error { e, handlerContext.keyword.location() } ;
        OpmLog::error(opm_error.what());
        std::throw_with_nested(opm_error);
    }

    return true;
}

}
