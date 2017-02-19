/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2017 Johan Hagenbjörk
 Copyright (C) 2004, 2005 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006 StatPro Italia srl

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

#include <ql/time/calendars/unitedstates.hpp>
#include <ql/errors.hpp>
#include <ql/settings.hpp>

namespace QuantLib {

    namespace {

        // a few rules used by multiple calendars

        bool isMartinLutherKingDay(Day d, Month m, Year y, Weekday w, Date ed) {
            // third Monday in January, introduced 1983-08-02, first 1986
            return m == January && w == Monday && (d >= 15 && d <= 21) 
                   && y >= 1986 && ed >= Date(2, August, 1983);
        }

        bool isWashingtonsBirthday(Day d, Month m, Year y, Weekday w, Date ed) {
            // third Monday in February, introduced 1971-01-01, first 1971
            return m == February && w == Monday && d >= 15 && d <= 21
                   && y >= 1971 && ed >= Date(1, January, 1971);
        }

        bool isPresidentsDay(Day d, Month m, Year y, Weekday w, Date ed) {
            // February 22nd, possily adjusted, removed 1971-01-01, last 1970
            return m == February && (d == 22 || (d == 23 && w == Monday)
                                     || (d == 21 && w == Friday));
        }

        bool isMemorialDay(Day d, Month m, Year y, Weekday w, Date ed) {
            // last Monday in May, introduced 1971-01-01, first 1971
            return (m == May && w == Monday && d >= 25)
                   && y >= 1971 && ed >= Date(1, January, 1971);
        }

        bool isDecorationDay(Day d, Month m, Year y, Weekday w, Date ed) {
            // May 30th, possibly adjusted, removed 1971-01-01, last 1970
            return m == May && (d == 30 || (d == 31 && w == Monday)
                                || (d == 29 && w == Friday))
                   && (y <= 1970 && ed >= Date(1, January, 1971)
                       || ed < Date(1, January, 1971));
        }

        bool isLaborDay(Day d, Month m, Year y, Weekday w, Date ed) {
            // first Monday in September
            return d <= 7 && w == Monday && m == September;
        }

        bool isColumbusDay(Day d, Month m, Year y, Weekday w, Date ed) {
            // second Monday in October, introduced 1971-01-01, first 1971
            return y >= 1971 && ed >= Date(1, January, 1971)
                   && m == October && w == Monday && (d >= 8 && d <= 14);
        }

        bool isVeteransDay(Day d, Month m, Year y, Weekday w, Date ed) {
            if (y >= 1975 && ed >= Date(1, January, 1975)) {
                // November 11th, adjusted, introduced 1975, first 1975
                return m == November && (d == 11 || (d == 12 && w == Monday)
                                         || (d == 10 && w == Friday));
            } else if (y >= 1971 && ed >= Date(1, January, 1971)) {
                // fourth Monday in October, introduced 1971-01-01, first 1971
                return m == October && w == Monday && (d >= 22 && d <= 28);
            } else
                return false;
        }

        
        bool isPresidentElectionDay(Day d, Month m, Year y, Weekday w, Date ed) {
            return ((y <= 1968 || (y <= 1980 && y % 4 == 0)) && m == November
                    && d <= 7 && w == Tuesday);
        }

    }
    
    UnitedStates::UnitedStates(UnitedStates::Market market) {
        // all calendar instances on the same market share the same
        // implementation instance
        static boost::shared_ptr<Calendar::Impl> settlementImpl(
                                        new UnitedStates::SettlementImpl);
        static boost::shared_ptr<Calendar::Impl> nyseImpl(
                                        new UnitedStates::NyseImpl);
        static boost::shared_ptr<Calendar::Impl> governmentImpl(
                                        new UnitedStates::GovernmentBondImpl);
        static boost::shared_ptr<Calendar::Impl> nercImpl(
                                        new UnitedStates::NercImpl);
        switch (market) {
          case Settlement:
            impl_ = settlementImpl;
            break;
          case NYSE:
            impl_ = nyseImpl;
            break;
          case GovernmentBond:
            impl_ = governmentImpl;
            break;
          case NERC:
            impl_ = nercImpl;
            break;
          default:
            QL_FAIL("unknown market");
        }
    }

    bool UnitedStates::SettlementImpl::isBusinessDay(const Date& date) const {
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    int UnitedStates::SettlementImpl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Date ed = Settings::instance().evaluationDate();
        Day d = date.dayOfMonth();
        Month m = date.month();
        Year y = date.year();

        if ((d == 1 || (d == 2 && w == Monday)) && m == January)
            return NewYearsDay; // possibly moved to Monday if on Sunday
        else if (d == 31 && w == Friday && m == December)
            return NewYearsDay; // possibly moved to Friday if on Saturday
        else if (isMartinLutherKingDay(d, m, y, w, ed))
            return MartinLutherKingDay;
        else if (isWashingtonsBirthday(d, m, y, w, ed))
            return WashingtonsBirthday;
        else if (isPresidentsDay(d, m, y, w, ed))
            return PresidentsDay;
        else if (isMemorialDay(d, m, y, w, ed))
            return MemorialDay;
        else if ((d == 4 || (d == 5 && w == Monday) ||
                 (d == 3 && w == Friday)) && m == July)
            return IndependenceDay; // Monday if Sunday or Friday if Saturday
        else if (isLaborDay(d, m, y, w, ed))
            return LabourDay;
        else if (isColumbusDay(d, m, y, w, ed))
            return ColumbusDay;
        else if (isVeteransDay(d, m, y, w, ed))
            return VeteransDay;
        else if ((d >= 22 && d <= 28) && w == Thursday && m == November)
            return ThanksgivingDay; // fourth Thursday in November
        else if ((d == 25 || (d == 26 && w == Monday) ||
                 (d == 24 && w == Friday)) && m == December)
            return ChristmasDay;// Monday if Sunday or Friday if Saturday
        else if (isWeekend(w))
            return Weekend;
        else
            return BusinessDay;
    }


    bool UnitedStates::NyseImpl::isBusinessDay(const Date& date) const {
        Weekday w = date.weekday();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        if (isWeekend(w))
            return false;
        // Special closings
        if (// Hurricane Sandy
            (y == 2012 && m == October && (d == 29 || d == 30))
            // President Ford's funeral
            || (y == 2007 && m == January && d == 2)
            // President Reagan's funeral
            || (y == 2004 && m == June && d == 11)
            // September 11-14, 2001
            || (y == 2001 && m == September && (11 <= d && d <= 14))
            // President Nixon's funeral
            || (y == 1994 && m == April && d == 27)
            // Hurricane Gloria
            || (y == 1985 && m == September && d == 27)
            // 1977 Blackout
            || (y == 1977 && m == July && d == 14)
            // Funeral of former President Lyndon B. Johnson.
            || (y == 1973 && m == January && d == 25)
            // Funeral of former President Harry S. Truman
            || (y == 1972 && m == December && d == 28)
            // National Day of Participation for the lunar exploration.
            || (y == 1969 && m == July && d == 21)
            // Funeral of former President Eisenhower.
            || (y == 1969 && m == March && d == 31)
            // Closed all day - heavy snow.
            || (y == 1969 && m == February && d == 10)
            // Day after Independence Day.
            || (y == 1968 && m == July && d == 5)
            // June 12-Dec. 31, 1968
            // Four day week (closed on Wednesdays) - Paperwork Crisis
            || (y == 1968 && dd >= 163 && w == Wednesday)
            // Day of mourning for Martin Luther King Jr.
            || (y == 1968 && m == April && d == 9)
            // Funeral of President Kennedy
            || (y == 1963 && m == November && d == 25)
            // Day before Decoration Day
            || (y == 1961 && m == May && d == 29)
            // Day after Christmas
            || (y == 1958 && m == December && d == 26)
            // Christmas Eve
            || ((y == 1954 || y == 1956 || y == 1965)
            && m == December && d == 24)
            ) return false;
        else
            return true;
    }

    bool UnitedStates::NyseImpl::isBusinessDay(const Date& date) const {
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    int UnitedStates::NyseImpl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Date ed = Settings::instance().evaluationDate();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if ((d == 1 || (d == 2 && w == Monday)) && m == January)
            return NewYearsDay; // possibly moved to Monday if on Sunday
        else if (d == 31 && w == Friday && m == December)
            return NewYearsDay; // possibly moved to Friday if on Saturday
        else if (y >= 1998 && isMartinLutherKingDay(d, m, y, w, ed))
            return MartinLutherKingDay;
        else if (isWashingtonsBirthday(d, m, y, w, ed))
            return WashingtonsBirthday;
        else if (isPresidentsDay(d, m, y, w, ed))
            return PresidentsDay;
        else if (dd == em - 3)
            return GoodFriday;
        else if (isMemorialDay(d, m, y, w, ed))
            return MemorialDay;
        else if ((d == 4 || (d == 5 && w == Monday) ||
            (d == 3 && w == Friday)) && m == July)
            return IndependenceDay; // Monday if Sunday or Friday if Saturday
        else if (isLaborDay(d, m, y, w, ed))
            return LabourDay;
        else if ((d >= 22 && d <= 28) && w == Thursday && m == November)
            return ThanksgivingDay; // fourth Thursday in November
        else if ((d == 25 || (d == 26 && w == Monday) ||
            (d == 24 && w == Friday)) && m == December)
            return ChristmasDay;// Monday if Sunday or Friday if Saturday
        else if (isPresidentElectionDay(d, m, y, w, ed))
            return PresidentElectionDay;
        else if (isWeekend(w))
            return Weekend;
        else
            return BusinessDay;
    }

    bool UnitedStates::GovernmentBondImpl::isBusinessDay(const Date& date) const {
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    int UnitedStates::GovernmentBondImpl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Date ed = Settings::instance().evaluationDate();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if ((d == 1 || (d == 2 && w == Monday)) && m == January)
            return NewYearsDay; // possibly moved to Monday if on Sunday
        else if (d == 31 && w == Friday && m == December)
            return NewYearsDay; // possibly moved to Friday if on Saturday
        else if (isMartinLutherKingDay(d, m, y, w, ed))
            return MartinLutherKingDay;
        else if (isWashingtonsBirthday(d, m, y, w, ed))
            return WashingtonsBirthday;
        else if (isPresidentsDay(d, m, y, w, ed))
            return PresidentsDay;
        else if (dd == em - 3)
            return GoodFriday;
        else if (isMemorialDay(d, m, y, w, ed))
            return MemorialDay;
        else if ((d == 4 || (d == 5 && w == Monday) ||
            (d == 3 && w == Friday)) && m == July)
            return IndependenceDay; // Monday if Sunday or Friday if Saturday
        else if (isLaborDay(d, m, y, w, ed))
            return LabourDay;
        else if (isColumbusDay(d, m, y, w, ed))
            return ColumbusDay;
        else if ((d >= 22 && d <= 28) && w == Thursday && m == November)
            return ThanksgivingDay; // fourth Thursday in November
        else if ((d == 25 || (d == 26 && w == Monday) ||
            (d == 24 && w == Friday)) && m == December)
            return ChristmasDay;// Monday if Sunday or Friday if Saturday
        else if (isWeekend(w))
            return Weekend;
        else
            return BusinessDay;
    }

    bool UnitedStates::NercImpl::isBusinessDay(const Date& date) const {
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    int UnitedStates::NercImpl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Day d = date.dayOfMonth();
        Date ed = Settings::instance().evaluationDate();
        Month m = date.month();
        Year y = date.year();

        if ((d == 1 || (d == 2 && w == Monday)) && m == January)
            return NewYearsDay; // possibly moved to Monday if on Sunday
        else if (d == 31 && w == Friday && m == December)
            return NewYearsDay; // possibly moved to Friday if on Saturday
        else if (isMemorialDay(d, m, y, w, ed))
            return MemorialDay;
        else if ((d == 4 || (d == 5 && w == Monday) ||
            (d == 3 && w == Friday)) && m == July)
            return IndependenceDay; // Monday if Sunday or Friday if Saturday
        else if (isLaborDay(d, m, y, w, ed))
            return LabourDay;
        else if ((d >= 22 && d <= 28) && w == Thursday && m == November)
            return ThanksgivingDay; // fourth Thursday in November
        else if ((d == 25 || (d == 26 && w == Monday) ||
            (d == 24 && w == Friday)) && m == December)
            return ChristmasDay;// Monday if Sunday or Friday if Saturday
        else if (isWeekend(w))
            return Weekend;
        else
            return BusinessDay;
    }


}
