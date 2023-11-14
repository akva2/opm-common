/*
  Copyright 2013 Statoil ASA.

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

#ifndef COMPLETION_HPP_
#define COMPLETION_HPP_

#include <opm/input/eclipse/Schedule/Well/FilterCake.hpp>
#include <opm/input/eclipse/Schedule/Well/WINJMULT.hpp>

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

namespace Opm {
    class DeckKeyword;
    class DeckRecord;
    class ScheduleGrid;
    class FieldPropsManager;
} // namespace Opm

namespace Opm { namespace RestartIO {
    struct RstConnection;
}} // namespace Opm::RestartIO

namespace Opm {

    class Connection
    {
    public:
        enum class State {
            OPEN = 1,
            SHUT = 2,
            AUTO = 3, // Seems like the AUTO state can not be serialized to restart files.
        };

        static std::string State2String(State enumValue);
        static State StateFromString(std::string_view stringValue);


        enum class Direction {
            X = 1,
            Y = 2,
            Z = 3,
        };

        static std::string Direction2String(const Direction enumValue);
        static Direction   DirectionFromString(std::string_view stringValue);


        enum class Order {
            DEPTH,
            INPUT,
            TRACK,
        };

        static std::string Order2String(Order enumValue);
        static Order OrderFromString(std::string_view comporderStringValue);


        enum class CTFKind {
            DeckValue,
            Defaulted,
        };

        Connection() = default;
        Connection(int i, int j, int k,
                   std::size_t          global_index,
                   int                  complnum,
                   double               depth,
                   State                state,
                   double               CF,
                   double               Kh,
                   double               rw,
                   double               r0,
                   double               re,
                   double               connection_length,
                   double               skin_factor,
                   double               d_factor,
                   double               Ke,
                   const int            satTableId,
                   const Direction      direction,
                   const CTFKind        ctf_kind,
                   const std::size_t    sort_value,
                   const bool           defaultSatTabId);

        Connection(const RestartIO::RstConnection& rst_connection,
                   const ScheduleGrid&             grid,
                   const FieldPropsManager&        fp);

        static Connection serializationTestObject();

        bool attachedToSegment() const;
        bool sameCoordinate(const int i, const int j, const int k) const;
        int getI() const;
        int getJ() const;
        int getK() const;
        std::size_t global_index() const;
        State state() const;
        Direction dir() const;
        double depth() const;
        int satTableId() const;
        int complnum() const;
        int segment() const;
        double CF() const;
        double wpimult() const;
        double Kh() const;
        double rw() const;
        double r0() const;
        double re() const;
        double connectionLength() const;
        double skinFactor() const;
        double dFactor() const;
        double Ke() const;
        CTFKind kind() const;
        const InjMult& injmult() const;
        bool activeInjMult() const;
        void setInjMult(const InjMult& inj_mult);
        void setFilterCake(const FilterCake& filter_cake);
        const FilterCake& getFilterCake() const;
        bool filterCakeActive() const;
        double getFilterCakeRadius() const;
        double getFilterCakeArea() const;

        void setState(State state);
        void setComplnum(int compnum);
        void setSkinFactor(double skin_factor);
        void setDFactor(double d_factor);
        void setKe(double Ke);
        void setCF(double CF);
        void scaleWellPi(double wellPi);
        bool prepareWellPIScaling();
        bool applyWellPIScaling(const double scaleFactor);
        void updateSegmentRST(const int segment_number_arg,
                              const double center_depth_arg);
        void updateSegment(const int segment_number_arg,
                           const double center_depth_arg,
                           const std::size_t compseg_insert_index,
                           const std::pair<double,double>& perf_range);
        std::size_t sort_value() const;
        bool getDefaultSatTabId() const;
        void setDefaultSatTabId(bool id);
        const std::optional<std::pair<double, double>>& perf_range() const;
        std::string str() const;
        bool ctfAssignedFromInput() const
        {
            return this->m_ctfkind == CTFKind::DeckValue;
        }

        bool operator==( const Connection& ) const;
        bool operator!=( const Connection& ) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(this->direction);
            serializer(this->center_depth);
            serializer(this->open_state);
            serializer(this->sat_tableId);
            serializer(this->m_complnum);
            serializer(this->m_CF);
            serializer(this->m_Kh);
            serializer(this->m_rw);
            serializer(this->m_r0);
            serializer(this->m_re);
            serializer(this->m_connection_length);
            serializer(this->m_skin_factor);
            serializer(this->m_d_factor);
            serializer(this->m_Ke);
            serializer(this->ijk);
            serializer(this->m_global_index);
            serializer(this->m_ctfkind);
            serializer(this->m_injmult);
            serializer(this->m_sort_value);
            serializer(this->m_perf_range);
            serializer(this->m_defaultSatTabId);
            serializer(this->segment_number);
            serializer(this->m_subject_to_welpi);
            serializer(this->m_filter_cake);
        }

    private:
        // Note to maintainer: If you add new members to this list, then
        // please also update the operator==(), serializeOp(), and
        // serializationTestObject() member functions.
        Direction direction { Direction::Z };
        double center_depth { 0.0 };
        State open_state { State::SHUT };
        int sat_tableId { -1 };
        int m_complnum { -1 };
        double m_CF;
        double m_Kh;
        double m_rw;
        double m_r0;
        double m_re;
        double m_connection_length;
        double m_skin_factor;
        double m_d_factor;
        double m_Ke;

        std::array<int,3> ijk{};
        CTFKind m_ctfkind { CTFKind::DeckValue };
        std::optional<InjMult> m_injmult{};
        std::size_t m_global_index{};

        /*
          The sort_value member is a peculiar quantity. The connections are
          assembled in the WellConnections class. During the lifetime of the
          connections there are three different sort orders which are all
          relevant:

             input: This is the ordering implied be the order of the
                connections in the input deck.

             simulation: This is the ordering the connections have in
                WellConnections container during the simulation and RFT output.

             restart: This is the ordering the connections have when they are
                written out to a restart file.

          Exactly what consitutes input, simulation and restart ordering, and
          how the connections transition between the three during application
          lifetime is different from MSW and normal wells.

          normal wells: For normal wells the simulation order is given by the
          COMPORD keyword, and then when the connections are serialized to the
          restart file they are written in input order; i.e. we have:

               input == restart and simulation given COMPORD

          To recover the input order when creating the restart files the
          sort_value member corresponds to the insert index for normal wells.

          MSW wells: For MSW wells the wells simulator order[*] is given by
          COMPSEGS keyword, the COMPORD keyword is ignored. The connections are
          sorted in WellConnections::order() and then retain that order for all
          eternity i.e.

               input and simulation == restart

          Now the important point is that the COMPSEGS detail used to perform
          this sorting is not available when loading from a restart file, but
          then the connections are already sorted correctly. I.e. *after* a
          restart we will have:

               input(from restart) == simulation == restart

          The sort_value member is used to sort the connections into restart
          ordering. In the case of normal wells this corresponds to recovering
          the input order, whereas for MSW wells this is equivalent to the
          simulation order.

          [*]: For MSW wells the topology is given by the segments and entered
               explicitly, so the truth is probably that the storage order
               during simulation makes no difference?
        */
        std::size_t m_sort_value{};

        std::optional<std::pair<double,double>> m_perf_range{};
        bool m_defaultSatTabId{true};

        // Associate segment number
        //
        // 0 means the connection is not associated to a segment.
        int segment_number { 0 };

        // Whether or not this Connection is subject to WELPI scaling.
        bool m_subject_to_welpi { false };

        // For applying last known WPIMULT to when calculating connection transmissibilty factor in CSKIN
        double m_wpimult = 1.0;

        std::optional<FilterCake> m_filter_cake{};

        static std::string CTFKindToString(const CTFKind);
    };

} // namespace Opm

#endif // COMPLETION_HPP_
