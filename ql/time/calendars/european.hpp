/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
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

/*! \file european.hpp
    \brief European Exchange Market calendar
*/

#ifndef quantlib_european_calendar_hpp
#define quantlib_european_calendar_hpp

#include <ql/time/calendar.hpp>

namespace QuantLib {

    //! Swedish calendar
    /*! Holidays:
        <ul>
        <li>Saturdays</li>
        <li>Sundays</li>
        <li>New Year's Day, January 1st</li>
        <li>Good Friday</li>
        <li>Easter Monday</li>
        <li>Labour Day, May 1st</li>
        <li>Christmas Day, December 25th</li>
        <li>Boxing Day, December 26th</li>
        </ul>

        \ingroup calendars
    */
    class European : public Calendar {
      private:
        class Impl : public Calendar::WesternImpl {
          public:
            enum Holidays { BusinessDay, Weekend, NewYearsDay, GoodFriday, 
                            EasterMonday, LabourDay, ChristmasDay, BoxingDay};
            
            std::string name() const { return "European Exchange Market"; }
            bool isBusinessDay(const Date&) const;
            int holidayType(const Date&) const;
            std::string holidayName(const Date&) const;
        };
      public:
        European();
    };

}


#endif
