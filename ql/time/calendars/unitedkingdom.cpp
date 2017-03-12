/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/time/calendars/unitedkingdom.hpp>
#include <ql/errors.hpp>
#include <ql/settings.hpp>

namespace QuantLib {

    namespace {

        // a few rules used by multiple calendars

        bool isNewYearsDay(Day d, Month m, Year y, Weekday w, Date ed) {
            // January 1st, possible moved to monday, introduced 1974-10-18, first 1975
            return ((d == 1 || ((d == 2 || d == 3) && w == Monday)) &&
                    m == January && y >= 1975 && ed >= Date(18, October, 1974));
        }

        bool isEarlyMayBankHoliday(Day d, Month m, Year y, Weekday w, Date ed) {
            // First monday in May, introduced 1978, first 1978
            return (d <= 7 && w == Monday && m == May && y >= 1978 && ed.year() >= 1978);
        }

        bool isSpringBankHoliday(Day d, Month m, Year y, Weekday w, Date ed) {
            // Last monday in May, introduced 1971-12-16, first 1972
            return (d >= 25 && w == Monday && m == May && y >= 1972 &&
                    y != 2002 && y != 2012 && ed >= Date(16, December, 1971));
        }

        bool isSummerBankHoliday(Day d, Month m, Year y, Weekday w, Date ed) {
            // Last monday in August, introduced 1971-12-16, first 1972
            return (d >= 25 && w == Monday && m == August && y >= 1972 &&
                    ed >= Date(16, December, 1971));
        }
    }

    UnitedKingdom::UnitedKingdom(UnitedKingdom::Market market) {
        // all calendar instances on the same market share the same
        // implementation instance
        static boost::shared_ptr<Calendar::Impl> settlementImpl(
                                           new UnitedKingdom::SettlementImpl);
        static boost::shared_ptr<Calendar::Impl> exchangeImpl(
                                           new UnitedKingdom::ExchangeImpl);
        static boost::shared_ptr<Calendar::Impl> metalsImpl(
                                           new UnitedKingdom::MetalsImpl);
        switch (market) {
          case Settlement:
            impl_ = settlementImpl;
            break;
          case Exchange:
            impl_ = exchangeImpl;
            break;
          case Metals:
            impl_ = metalsImpl;
            break;
          default:
            QL_FAIL("unknown market");
        }
    }

    std::string UnitedKingdom::holidayName(int holidayType) {
        switch (holidayType) {
        case Weekend:
            return "Weekend";
        case NewYearsDay:
            return "New Years Day";
        case GoodFriday:
            return "GoodFriday";
        case EasterMonday:
            return "Easter Monday";
        case WhitMonday:
            return "Whit Monday";
        case EarlyMayBankHoliday:
            return "Early May Bank Holiday";
        case SpringBankHoliday:
            return "Spring Bank Holiday";
        case SummerBankHoliday:
            return "Summer Bank Holiday";
        case ChristmasDay:
            return "Christmas Day";
        case BoxingDay:
            return "Boxing Day";
        case SpecialHoliday:
            return "Closed markets due to a special event";
        default:
            return "Business Day";
        }
    }

    int UnitedKingdom::SettlementImpl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Date ed = Settings::instance().evaluationDate();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if (isNewYearsDay(d, m, y, w, ed))
            return NewYearsDay;
        else if (dd == em - 3)
            return GoodFriday;
        else if (dd == em)
            return EasterMonday;
        else if (isEarlyMayBankHoliday(d, m, y, w, ed))
            return EarlyMayBankHoliday;
        else if (isSpringBankHoliday(d, m, y, w, ed))
            return SpringBankHoliday;
        else if (isSummerBankHoliday(d, m, y, w, ed))
            return SummerBankHoliday;
        else if ((d == 25 || (d == 27 && (w == Monday || w == Tuesday)))
                 && m == December)
            return ChristmasDay;
        else if ((d == 26 || (d == 28 && (w == Monday || w == Tuesday)))
                 && m == December)
            return BoxingDay;
        else if (
            // June 3rd, 2002 only (Golden Jubilee Bank Holiday)
            // June 4rd, 2002 only (special Spring Bank Holiday)
            ((d == 3 || d == 4) && m == June && y == 2002)
            // April 29th, 2011 only (Royal Wedding Bank Holiday)
            || (d == 29 && m == April && y == 2011)
            // June 4th, 2012 only (Diamond Jubilee Bank Holiday)
            // June 5th, 2012 only (Special Spring Bank Holiday)
            || ((d == 4 || d == 5) && m == June && y == 2012)
            // Millennium, December 31st, 1999
            || (d == 31 && m == December && y == 1999))
            return SpecialHoliday;
        else
            return BusinessDay;
    }

    bool UnitedKingdom::SettlementImpl::isBusinessDay(const Date& date) const {
        Weekday w = date.weekday();
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    std::string UnitedKingdom::SettlementImpl::holidayName(const Date& date) const {
        return UnitedKingdom::holidayName(holidayType(date));
    }

    int UnitedKingdom::ExchangeImpl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Date ed = Settings::instance().evaluationDate();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if (isNewYearsDay(d, m, y, w, ed))
            return NewYearsDay;
        else if (dd == em - 3)
            return GoodFriday;
        else if (dd == em)
            return EasterMonday;
        else if (isEarlyMayBankHoliday(d, m, y, w, ed))
            return EarlyMayBankHoliday;
        else if (isSpringBankHoliday(d, m, y, w, ed))
            return SpringBankHoliday;
        else if (isSummerBankHoliday(d, m, y, w, ed))
            return SummerBankHoliday;
        else if ((d == 25 || (d == 27 && (w == Monday || w == Tuesday)))
            && m == December)
            return ChristmasDay;
        else if ((d == 26 || (d == 28 && (w == Monday || w == Tuesday)))
            && m == December)
            return BoxingDay;
        else if (
            // June 3rd, 2002 only (Golden Jubilee Bank Holiday)
            // June 4rd, 2002 only (Special Spring Bank Holiday)
            ((d == 3 || d == 4) && m == June && y == 2002)
            // April 29th, 2011 only (Royal Wedding Bank Holiday)
            || (d == 29 && m == April && y == 2011)
            // June 4th, 2012 only (Diamond Jubilee Bank Holiday)
            // June 5th, 2012 only (Special Spring Bank Holiday)
            || ((d == 4 || d == 5) && m == June && y == 2012)
            // Millennium, December 31st, 1999
            || (d == 31 && m == December && y == 1999))
            return SpecialHoliday;
        else
            return BusinessDay;
    }

    bool UnitedKingdom::ExchangeImpl::isBusinessDay(const Date& date) const {
        Weekday w = date.weekday();
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    std::string UnitedKingdom::ExchangeImpl::holidayName(const Date& date) const {
        return UnitedKingdom::holidayName(holidayType(date));
    }

    int UnitedKingdom::MetalsImpl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Date ed = Settings::instance().evaluationDate();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if (isNewYearsDay(d, m, y, w, ed))
            return NewYearsDay;
        else if (dd == em - 3)
            return GoodFriday;
        else if (dd == em)
            return EasterMonday;
        else if (isEarlyMayBankHoliday(d, m, y, w, ed))
            return EarlyMayBankHoliday;
        else if (isSpringBankHoliday(d, m, y, w, ed))
            return SpringBankHoliday;
        else if (isSummerBankHoliday(d, m, y, w, ed))
            return SummerBankHoliday;
        else if ((d == 25 || (d == 27 && (w == Monday || w == Tuesday)))
            && m == December)
            return ChristmasDay;
        else if ((d == 26 || (d == 28 && (w == Monday || w == Tuesday)))
            && m == December)
            return BoxingDay;
        else if (
            // June 3rd, 2002 only (Golden Jubilee Bank Holiday)
            // June 4rd, 2002 only (Special Spring Bank Holiday)
            ((d == 3 || d == 4) && m == June && y == 2002)
            // April 29th, 2011 only (Royal Wedding Bank Holiday)
            || (d == 29 && m == April && y == 2011)
            // June 4th, 2012 only (Diamond Jubilee Bank Holiday)
            // June 5th, 2012 only (Special Spring Bank Holiday)
            || ((d == 4 || d == 5) && m == June && y == 2012)
            // Millennium, December 31st, 1999
            || (d == 31 && m == December && y == 1999))
            return SpecialHoliday;
        else
            return BusinessDay;
    }

    bool UnitedKingdom::MetalsImpl::isBusinessDay(const Date& date) const {
        Weekday w = date.weekday();
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    std::string UnitedKingdom::MetalsImpl::holidayName(const Date& date) const {
        return UnitedKingdom::holidayName(holidayType(date));
    }

}

