/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2017, Johan Hagenbjörk

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

#include <ql/time/calendars/sweden.hpp>
#include <ql/settings.hpp>

namespace QuantLib {

    Sweden::Sweden() {
        // all calendar instances share the same implementation instance
        static boost::shared_ptr<Calendar::Impl> impl(new Sweden::Impl);
        impl_ = impl;
    }

    bool  Sweden::Impl::isBusinessDay(const Date& date) const {
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    int Sweden::Impl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Date ed = Settings::instance().evaluationDate(); // This is an expensive calculation, try to avoid 
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if (dd == em - 3)
            return GoodFriday;
        else if (dd == em)
            return EasterMonday;
        else if (dd == em + 38)
            return AscensionThursday;
        else if (dd == em + 49 && y < 2005) // fixme
            return WhitMonday;
        else if (d == 1 && m == January)
            return NewYearsDay;
        else if (d == 6 && m == January)
            return Epiphany;
        else if (d == 1 && m == May)
            return MayDay;
        else if (d == 6 && m == June && y >= 2010) // fixme
            return NationalDay;
        else if (w == Friday && (d >= 19 && d <= 25) && m == June)
            return MidsummerEve;
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
        
    std::string Sweden::Impl::holidayName(const Date& date) const {
        switch (holidayType(date)) {
            case Weekend:
                return "Weekend";
            case NewYearsDay:
                return "New Years Day";
            case Epiphany:
                return "Epiphany";
            case GoodFriday:
                return "Good Friday";
            case EasterMonday:
                return "Easter Monday";
            case AscensionThursday:
                return "Ascension Thursday";
            case WhitMonday:
                return "Whit Monday";
            case MayDay:
                return "May Day";
            case NationalDay:
                return "National Day";
            case MidsummerEve:
                return "Midsummer Eve";
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

}

