/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 StatPro Italia srl
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

/*! \file denmark.hpp
    \brief Danish calendar
*/

#ifndef quantlib_danish_calendar_hpp
#define quantlib_danish_calendar_hpp

#include <ql/time/calendar.hpp>

namespace QuantLib {

    //! Danish calendar
    /*! Holidays:
        <ul>
        <li>Saturdays</li>
        <li>Sundays</li>
        <li>New Year's Day, January 1st</li>
        <li>Maunday Thursday</li>
        <li>Good Friday</li>
        <li>Easter Monday</li>
        <li>Great Prayer Day, 25 days after Easter Monday</li>
        <li>Ascension, 38 days after Easter Monday</li>
        <li>Bank Holiday, 39 days after Easter Monday</li>
        <li>Whit (Pentecost) Monday, 49 days after Easter Monday</li>
        <li>Constitution Day, June 5th</li>
        <li>Christmas Eve, December 24th</li>
        <li>Christmas Day, December 25th</li>
        <li>Boxing Day, December 26th</li>
        <li>New Year's Eve, December 31th</li>
        </ul>

        \ingroup calendars
    */
    class Denmark : public Calendar {
      private:
        class Impl : public Calendar::WesternImpl {
          public:
           enum Holidays { BusinessDay, Weekend,
                           NewYearsDay, MaundayThrusday, GoodFriday, EasterMonday, 
                           GreatPrayerDay, AscensionDay, BankHoliday, WhitMonday, 
                           ConstitutionDay, ChristmasEve, ChristmasDay, BoxingDay, 
                           NewYearsEve };
            std::string name() const { return "Denmark"; }
            bool isBusinessDay(const Date&) const;
            int holidayType(const Date&) const;
            std::string holidayName(const Date&) const;
        };
      public:
        Denmark();
    };

}


#endif
