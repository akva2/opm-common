/*
  Copyright 2024 SINTEF Digital

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
#ifndef OPM_CHECKSUM_HPP
#define OPM_CHECKSUM_HPP

#include <numeric>
#include <opm/common/utility/TimeService.hpp>

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

namespace Opm::Serialization {
namespace detail {

template<class Container>
std::vector<std::size_t> getSortedIndex(const Container& C)
{
    std::vector<std::size_t> index(C.size());
    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(),
              [&C](std::size_t i, std::size_t j)
              {
        auto it1 = C.begin();
        std::advance(it1, i);
        auto it2 = C.begin();
        std::advance(it2, j);
        return it1->first < it2->first;
    });

    return index;
}


//! \brief Abstract struct for check-summing which is (partially) specialized for specific types.
template <bool pod, class T>
struct CheckSum
{
    template<class CheckSummer>
    static void checksum(const T&, CheckSummer& checkSum);
};

//! \brief Check-summing for pod data.
template<class T>
struct CheckSum<true,T>
{
    //! \brief Calculates the check-sum for a POD.
    //! \param data The data to check-sum
    //! \param checksum Checksummer to use
    template<class CheckSummer>
    static void checkSum(const T& data, CheckSummer& checksum)
    {
        checkSum(&data, 1, checksum);
    }

    //! \brief Calculates the check-sum for an array of POD.
    //! \param data The array to check-sum
    //! \param n Length of array
    //! \param checksum Checksummer to use
    template<class CheckSummer>
    static void checkSum(const T* data, std::size_t n, CheckSummer& checksum)
    {
        const char* Tbuf = reinterpret_cast<const char*>(data);
        std::for_each(Tbuf, Tbuf + n*sizeof(T), std::ref(checksum));
    }
};

//! \brief Default handling for unsupported types.
template<class T>
struct CheckSum<false,T>
{
    template<class CheckSummer>
    static void checksum(const T&, CheckSummer&)
    {
        static_assert(!std::is_same_v<T,T>, "Check-summing not supported for type");
    }
};

//! \brief Specialization for std::bitset
template <std::size_t Size>
struct CheckSum<false,std::bitset<Size>>
{
    template<class CheckSummer>
    static void checkSum(const std::bitset<Size>& data, CheckSummer& checksum)
    {
        CheckSum<true,unsigned long long>::checkSum(data.to_ullong(), checksum);
    }
};

template<>
struct CheckSum<false,std::string>
{
    template<class CheckSummer>
    static void checkSum(const std::string& data, CheckSummer& checksum)
    {
        CheckSum<true,char>::checkSum(data.c_str(), data.size(), checksum);
    }
};

template<>
struct CheckSum<false,time_point>
{
    template<class CheckSummer>
    static void checkSum(const time_point& data, CheckSummer& checksum)
    {
        CheckSum<true,std::time_t>::checkSum(TimeService::to_time_t(data), checksum);
    }
};

}

//! \brief Struct handling check-summing of data.
template<class CheckSummer>
struct CheckSum
{
    //! \brief Constructor
    //! \param csum Check-summer to use. Must implement operator()(char)
    CheckSum(CheckSummer& csum)
        : m_checksum(csum)
    {
    }

    //! \brief Calculates the checksum for a variable.
    //! \tparam T The type of the data to be checksummed
    //! \param data The data to checksum
    template<class T>
    void checkSum(const T& data) const
    {
        detail::CheckSum<std::is_pod_v<T>,T>::checkSum(data, m_checksum);
    }

    //! \brief Calculates the checksum for an array.
    //! \tparam T The type of the data to be checksummed
    //! \param data The array to checksum
    //! \param n Length of array
    template<class T>
    void checkSum(const T* data, std::size_t n) const
    {
        static_assert(std::is_pod_v<T>, "Checksumming an array not supported for non-pod data");
        detail::CheckSum<true,T>::checkSum(data, n, m_checksum);
    }

    CheckSummer& m_checksum; //!< Actual check-summing implementation
};

} // end namespace Opm::Serialization

#endif // OPM_CHECKSUM_HPP
