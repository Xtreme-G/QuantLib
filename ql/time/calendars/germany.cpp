/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004 Ferdinando Ametrano
 Copyright (C) 2017 Johan Hagenbjörk

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

#include <ql/time/calendars/germany.hpp>
#include <ql/errors.hpp>
#include <ql/settings.hpp>

namespace QuantLib {

    namespace {

        // a few rules used by multiple calendars

        bool isLabourDay(Day d, Month m, Year y, Weekday w, Date ed) {
            // 1st of May, introduced 1919-04-17, first 1919
            return d == 1 && m == May &&
                   y >= 1919 && ed >= Date(17, April, 1919);
        }

        bool isReunionDay(Day d, Month m, Year y, Weekday w, Date ed) {
            // 3rd of October, introduced 1990-08-31, first 1990
            return d == 3 && m == October &&
                   y >= 1990 && ed >= Date(31, August, 1990);
        }

        bool isNationalDay(Day d, Month m, Year y, Weekday w, Date ed) {
            // 17th of July, first 1953, removed 1990-08-31, last 1990
            return d == 17 && m == July &&
                   y >= 1953 && ed.year() >= 1953 && ed <= Date(31, August, 1990);
        }
    }

    Germany::Germany(Germany::Market market) {
        // all calendar instances on the same market share the same
        // implementation instance
        static boost::shared_ptr<Calendar::Impl> settlementImpl(
            new Germany::SettlementImpl);
        static boost::shared_ptr<Calendar::Impl> frankfurtStockExchangeImpl(
            new Germany::FrankfurtStockExchangeImpl);
        static boost::shared_ptr<Calendar::Impl> xetraImpl(
            new Germany::XetraImpl);
        static boost::shared_ptr<Calendar::Impl> eurexImpl(
            new Germany::EurexImpl);
        static boost::shared_ptr<Calendar::Impl> euwaxImpl(
            new Germany::EuwaxImpl);

        switch (market) {
          case Settlement:
            impl_ = settlementImpl;
            break;
          case FrankfurtStockExchange:
            impl_ = frankfurtStockExchangeImpl;
            break;
          case Xetra:
            impl_ = xetraImpl;
            break;
          case Eurex:
            impl_ = eurexImpl;
            break;
          case Euwax:
            impl_ = euwaxImpl;
            break;
          default:
            QL_FAIL("unknown market");
        }
    }

    std::string Germany::holidayName(int holidayType) {
        switch (holidayType) {
        case Weekend:
            return "Weekend";
        case NewYearsDay:
            return "New Years Day";
        case GoodFriday:
            return "GoodFriday";
        case EasterMonday:
            return "Easter Monday";
        case AscensionThursday:
            return "Ascension Thursday";
        case WhitMonday:
            return "Whit Monday";
        case CorpusChristi:
            return "Corpus Christi";
        case ReunionDay:
            return "Day of German unity";
        case NationalDay:
            return "Old National Day";
        case ChristmasEve:
            return "Christmas Eve";
        case ChristmasDay:
            return "Christmas Day";
        case BoxingDay:
            return "Boxing Day";
        case NewYearsEve:
            return "New Year's Eve";
        default:
            return "Business Day";
        }
    }

    int Germany::SettlementImpl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Date ed = Settings::instance().evaluationDate();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if (d == 1 && m == January)
            return NewYearsDay;
        else if (dd == em - 3)
            return GoodFriday;
        else if (dd == em)
            return EasterMonday;
        else if (dd == em + 38)
            return AscensionThursday;
        else if (dd == em + 49)
            return WhitMonday;
        else if (dd == em + 59)
            return CorpusChristi;
        else if (isLabourDay(d, m, y, w, ed))
            return LabourDay;
        else if (isReunionDay(d, m, y, w, ed))
            return ReunionDay;
        else if (isNationalDay(d, m, y, w, ed))
            return NationalDay;
        else if (d == 24 && m == December)
            return ChristmasEve;
        else if (d == 25 && m == December)
            return ChristmasDay;
        else if (d == 26 && m == December)
            return BoxingDay;
        else if (d == 31 && m == December)
            return NewYearsEve;
        else if (isWeekend(w))
            return Weekend;
        else
            return BusinessDay;
    }

    bool Germany::SettlementImpl::isBusinessDay(const Date& date) const {
        Weekday w = date.weekday();
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    std::string Germany::SettlementImpl::holidayName(const Date& date) const {
        return Germany::holidayName(holidayType(date));
    }

    int Germany::FrankfurtStockExchangeImpl::holidayType(
      const Date& date) const {
        Weekday w = date.weekday();
        Date ed = Settings::instance().evaluationDate();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if (d == 1 && m == January)
            return NewYearsDay;
        else if (dd == em - 3)
            return GoodFriday;
        else if (dd == em)
            return EasterMonday;
        else if (isLabourDay(d, m, y, w, ed))
            return LabourDay;
        else if (d == 24 && m == December)
            return ChristmasEve;
        else if (d == 25 && m == December)
            return ChristmasDay;
        else if (d == 26 && m == December)
            return BoxingDay;
        else if (d == 31 && m == December)
            return NewYearsEve;
        else if (isWeekend(w))
            return Weekend;
        else
            return BusinessDay;
    }

    bool Germany::FrankfurtStockExchangeImpl::isBusinessDay(
      const Date& date) const {
        Weekday w = date.weekday();
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    std::string Germany::FrankfurtStockExchangeImpl::holidayName(
      const Date& date) const {
        return Germany::holidayName(holidayType(date));
    }

    int Germany::XetraImpl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Date ed = Settings::instance().evaluationDate();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if (d == 1 && m == January)
            return NewYearsDay;
        else if (dd == em - 3)
            return GoodFriday;
        else if (dd == em)
            return EasterMonday;
        else if (isLabourDay(d, m, y, w, ed))
            return LabourDay;
        else if (d == 24 && m == December)
            return ChristmasEve;
        else if (d == 25 && m == December)
            return ChristmasDay;
        else if (d == 26 && m == December)
            return BoxingDay;
        else if (d == 31 && m == December)
            return NewYearsEve;
        else if (isWeekend(w))
            return Weekend;
        else
            return BusinessDay;
    }

    bool Germany::XetraImpl::isBusinessDay(const Date& date) const {
        Weekday w = date.weekday();
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    std::string Germany::XetraImpl::holidayName(const Date& date) const {
        return Germany::holidayName(holidayType(date));
    }

    int Germany::EurexImpl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Date ed = Settings::instance().evaluationDate();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if (d == 1 && m == January)
            return NewYearsDay;
        else if (dd == em - 3)
            return GoodFriday;
        else if (dd == em)
            return EasterMonday;
        else if (isLabourDay(d, m, y, w, ed))
            return LabourDay;
        else if (d == 24 && m == December)
            return ChristmasEve;
        else if (d == 25 && m == December)
            return ChristmasDay;
        else if (d == 26 && m == December)
            return BoxingDay;
        else if (d == 31 && m == December)
            return NewYearsEve;
        else if (isWeekend(w))
            return Weekend;
        else
            return BusinessDay;
    }

    bool Germany::EurexImpl::isBusinessDay(const Date& date) const {
        Weekday w = date.weekday();
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    std::string Germany::EurexImpl::holidayName(const Date& date) const {
        return Germany::holidayName(holidayType(date));
    }

    int Germany::EuwaxImpl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Date ed = Settings::instance().evaluationDate();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if (d == 1 && m == January)
            return NewYearsDay;
        else if (dd == em - 3)
            return GoodFriday;
        else if (dd == em)
            return EasterMonday;
        else if (dd == em + 49)
            return WhitMonday;
        else if (isLabourDay(d, m, y, w, ed))
            return LabourDay;
        else if (d == 24 && m == December)
            return ChristmasEve;
        else if (d == 25 && m == December)
            return ChristmasDay;
        else if (d == 26 && m == December)
            return BoxingDay;
        else if (d == 31 && m == December)
            return NewYearsEve;
        else if (isWeekend(w))
            return Weekend;
        else
            return BusinessDay;
    }
    
    bool Germany::EuwaxImpl::isBusinessDay(const Date& date) const {
        Weekday w = date.weekday();
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    std::string Germany::EuwaxImpl::holidayName(const Date& date) const {
        return Germany::holidayName(holidayType(date));
    }

}

