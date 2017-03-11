/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 StatPro Italia srl

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

#include <ql/time/calendars/denmark.hpp>
#include <ql/settings.hpp>

namespace QuantLib {

    Denmark::Denmark() {
        // all calendar instances share the same implementation instance
        static boost::shared_ptr<Calendar::Impl> impl(new Denmark::Impl);
        impl_ = impl;
    }

    bool  Denmark::Impl::isBusinessDay(const Date& date) const {
        if (isWeekend(date.weekday()))
            return false;
        else
            return holidayType(date) == BusinessDay;
    }

    int Denmark::Impl::holidayType(const Date& date) const {
        Weekday w = date.weekday();
        Date ed = Settings::instance().evaluationDate();
        Day d = date.dayOfMonth(), dd = date.dayOfYear();
        Month m = date.month();
        Year y = date.year();
        Day em = easterMonday(y);

        if (d == 1 && m == January)
            return NewYearsDay;
        else if (dd == em - 4)
            return MaundayThrusday;
        else if (dd == em - 3)
            return GoodFriday;
        else if (dd == em)
            return EasterMonday;
        else if (dd == em + 25)
            return GreatPrayerDay;
        else if (dd == em + 38)
            return AscensionDay;
        else if (dd == em + 39 && y >= 2008 && ed.year() >= 2008) // Introduced 2008
            return BankHoliday;
        else if (dd == em + 49 && ed < Date(12, October, 2004)) // Removed 2004-10-12, Last 2004
            return WhitMonday;
        else if (d == 5 && m == June)
            return ConstitutionDay;
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

    std::string Denmark::Impl::holidayName(const Date& date) const {
        switch (holidayType(date)) {
        case Weekend:
            return "Weekend";
        case NewYearsDay:
            return "New Years Day";
        case MaundayThrusday:
            return "Maunday Thursday";
        case GoodFriday:
            return "Good Friday";
        case EasterMonday:
            return "Easter Monday";
        case GreatPrayerDay:
            return "Great Prayer Day";
        case AscensionDay:
            return "Ascension Day";
        case BankHoliday:
            return "Bank Holiday";
        case WhitMonday:
            return "Whit Monday";
        case ConstitutionDay:
            return "Constitution Day";
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

