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

#include <ql/time/calendars/European.hpp>
#include <ql/settings.hpp>

namespace QuantLib {

    European::European() {
        // all calendar instances share the same implementation instance
        static boost::shared_ptr<Calendar::Impl> impl(new European::Impl);
        impl_ = impl;
    }

    bool  European::Impl::isBusinessDay(const Date& date) const {
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    int European::Impl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
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
        else if (d == 1 && m == May)
            return LabourDay;
        else if (d == 25 && m == December)
            return ChristmasDay;
        else if (d == 26 && m == December)
            return BoxingDay;
        else if (isWeekend(w))
            return Weekend;
        else
            return BusinessDay;
    }
        
    std::string European::Impl::holidayName(const Date& date) const {
        switch (holidayType(date)) {
            case Weekend:
                return "Weekend";
            case NewYearsDay:
                return "New Years Day";
            case GoodFriday:
                return "Good Friday";
            case EasterMonday:
                return "Christmas Eve";
            case ChristmasDay:
                return "Christmas Day";
            case BoxingDay:
                return "Boxing Day";
            default:
                return "Business Day";
        }
    }

}

