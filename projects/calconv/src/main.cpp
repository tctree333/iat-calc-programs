/*
 *--------------------------------------
 * Program Name: CALCONV
 * Author: Tomi Chen
 * License: MIT
 * Description: Convert between different calendar systems.
 *
 * Calendar conversion functions adapted from Fourmilab's calendar converter
 *--------------------------------------
 */

#include <ti/getcsc.h>
#include <ti/getkey.h>
#include <ti/screen.h>
#include <ti/vars.h>
#include <ti/real>

using namespace ti::literals;

#define MATRIX OS_VAR_MAT_J

// Gregorian

#define GREGORIAN_EPOCH 1721425.5_r

bool leap_gregorian(ti::real year)
{
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

ti::real gregorian_to_jd(ti::real year, ti::real month, ti::real day)
{
    return (GREGORIAN_EPOCH - 1) +
           ((year - 1) * 365) +
           ((year - 1) / 4).floor() +
           (-((year - 1) / 100).floor()) +
           ((year - 1) / 400).floor() +
           ((((month * 367) - 362) / 12) +
            ((month <= 2) ? 0 : (leap_gregorian(year) ? -1 : -2)) +
            day)
               .floor();
}

void jd_to_gregorian(ti::real jd, real_t result[3])
{
    ti::real wjd, depoch, quadricent, dqc, cent, dcent, quad, dquad,
        yindex, year, yearday, leapadj, month, day;

    wjd = (jd - 0.5).floor() + 0.5;
    depoch = wjd - GREGORIAN_EPOCH;
    quadricent = (depoch / 146097).floor();
    dqc = depoch.fmod(146097);
    cent = (dqc / 36524).floor();
    dcent = dqc.fmod(36524);
    quad = (dcent / 1461).floor();
    dquad = dcent.fmod(1461);
    yindex = (dquad / 365).floor();
    year = (quadricent * 400) + (cent * 100) + (quad * 4) + yindex;
    if (!((cent == 4) || (yindex == 4)))
    {
        year++;
    }
    yearday = wjd - gregorian_to_jd(year, 1, 1);
    leapadj = ((wjd < gregorian_to_jd(year, 3, 1)) ? 0
                                                   : (leap_gregorian(year) ? 1 : 2));
    month = ((((yearday + leapadj) * 12) + 373) / 367).floor();
    day = (wjd - gregorian_to_jd(year, month, 1)) + 1;

    result[0] = year;
    result[1] = month;
    result[2] = day;
}

// Julian

#define JULIAN_EPOCH 1721423.5

bool leap_julian(ti::real year)
{
    return year.fmod(4) == ((year > 0) ? 0 : 3);
}

ti::real julian_to_jd(ti::real year, ti::real month, ti::real day)
{

    /* Adjust negative common era years to the zero-based notation we use.  */

    if (year < 1)
    {
        year++;
    }

    /* Algorithm as given in Meeus, Astronomical Algorithms, Chapter 7, page 61 */

    if (month <= 2)
    {
        year--;
        month += 12;
    }

    return (((((year + 4716) * 365.25)).floor() +
             (((month + 1) * 30.6001_r)).floor() +
             day) -
            1524.5);
}

void jd_to_julian(ti::real jd, real_t result[3])
{
    ti::real z, a, b, c, d, e, year, month, day;

    jd += 0.5;
    z = jd.floor();

    a = z;
    b = a + 1524;
    c = ((b - 122.1_r) / 365.25).floor();
    d = (c * 365.25).floor();
    e = ((b - d) / 30.6001_r).floor();

    month = ((e < 14) ? (e - 1) : (e - 13)).floor();
    year = ((month > 2) ? (c - 4716) : (c - 4715)).floor();
    day = b - d - (e * 30.6001_r).floor();

    /*  If year is less than 1, subtract one to convert from
        a zero based date system to the common era system in
        which the year -1 (1 B.C.E) is followed by year 1 (1 C.E.).  */

    if (year < 1)
    {
        year--;
    }

    result[0] = year;
    result[1] = month;
    result[2] = day;
}

// Hebrew

// #define HEBREW_EPOCH 347995.5_r

// //  Is a given Hebrew year a leap year ?

// bool hebrew_leap(ti::real year)
// {
//     return ((year * 7) + 1).fmod(19) < 7;
// }

// //  How many months are there in a Hebrew year (12 = normal, 13 = leap)

// int hebrew_year_months(ti::real year)
// {
//     return hebrew_leap(year) ? 13 : 12;
// }

// //  Test for delay of start of new year and to avoid
// //  Sunday, Wednesday, and Friday as start of the new year.

// ti::real hebrew_delay_1(ti::real year)
// {
//     ti::real months, day, parts;

//     months = (((year * 235) - 234) / 19).floor();
//     parts = (months * 13753) + 12084;
//     day = (months * 29) + (parts / 25920).floor();

//     if (((day + 1) * 3).fmod(7) < 3)
//     {
//         day++;
//     }
//     return day;
// }

// //  Check for delay in start of new year due to length of adjacent years

// ti::real hebrew_delay_2(ti::real year)
// {
//     ti::real last, present, next;

//     last = hebrew_delay_1(year - 1);
//     present = hebrew_delay_1(year);
//     next = hebrew_delay_1(year + 1);

//     return ((next - present) == 356) ? 2 : (((present - last) == 382) ? 1 : 0);
// }

// //  How many days are in a Hebrew year ?

// ti::real hebrew_year_days(ti::real year)
// {
//     return hebrew_to_jd(year + 1, 7, 1) - hebrew_to_jd(year, 7, 1);
// }

// //  How many days are in a given month of a given year

// ti::real hebrew_month_days(ti::real year, ti::real month)
// {
//     //  First of all, dispose of fixed-length 29 day months

//     if (month == 2 || month == 4 || month == 6 ||
//         month == 10 || month == 13)
//     {
//         return 29;
//     }

//     //  If it's not a leap year, Adar has 29 days

//     if (month == 12 && !hebrew_leap(year))
//     {
//         return 29;
//     }

//     //  If it's Heshvan, days depend on length of year

//     if (month == 8 && !(hebrew_year_days(year).fmod(10) == 5))
//     {
//         return 29;
//     }

//     //  Similarly, Kislev varies with the length of year

//     if (month == 9 && (hebrew_year_days(year).fmod(10) == 3))
//     {
//         return 29;
//     }

//     //  Nope, it's a 30 day month

//     return 30;
// }

//  Finally, wrap it all up into...

// ti::real hebrew_to_jd(ti::real year, ti::real month, ti::real day)
// {
//     ti::real jd, mon, months;

//     months = hebrew_year_months(year);
//     jd = HEBREW_EPOCH + hebrew_delay_1(year) +
//          hebrew_delay_2(year) + day + 1;

//     if (month < 7)
//     {
//         for (mon = 7; mon <= months; mon++)
//         {
//             jd += hebrew_month_days(year, mon);
//         }
//         for (mon = 1; mon < month; mon++)
//         {
//             jd += hebrew_month_days(year, mon);
//         }
//     }
//     else
//     {
//         for (mon = 7; mon < month; mon++)
//         {
//             jd += hebrew_month_days(year, mon);
//         }
//     }

//     return jd;
// }

ti::real d3(ti::real a1)
{
    ti::real m2, w4, u2;

    m2 = (((a1 * 235) - 234) / 19).floor();
    w4 = (m2 / 25920).floor();
    u2 = m2.fmod(25920);
    return (m2 * 29) + (w4 * 13753) + (((u2 * 13753) + 12084) / 25920).floor();
}
ti::real d5(ti::real a1)
{
    return d3(a1) + (d3(a1).fmod(7) * 6 / 7).floor().fmod(2);
}
ti::real l2(ti::real a1)
{
    return d5(a1 + 1) - d5(a1);
}
ti::real d1(ti::real a1)
{
    return d5(a1) + ((l2(a1) + 19) / 15).floor().fmod(2) + ((l2(a1 - 1) + 7) / 15).floor().fmod(2);
}
ti::real d2(ti::real a1, ti::real m1)
{
    ti::real l = d1(a1 + 1) - d1(a1);
    return d1(a1) + ((m1 * 384 + 10) / 13).floor() + (((l + 1) / 2).floor().fmod(3) * ((m1 + 10) / 12).floor()) - (((l - 385) / 2).floor().fmod(3) * ((m1 + 9) / 12).floor());
}

ti::real hebrew_to_jd(ti::real year, ti::real month, ti::real day)
{
    ti::real m1, a1;

    m1 = month - 7;
    a1 = year - (m1 / 10).floor();
    return d2(a1, m1) +
           day + 347996.5;
}

void jd_to_hebrew(ti::real jd, real_t result[3])
{
    ti::real s, w1, e1, u3, a3, e3, u4, d14, u5, a4, e4, u6, d15, u7, a1, e5, m1, d0, year, month, day;

    s = jd.floor() - 347997;
    w1 = (s / 33783).floor();
    e1 = s.fmod(33783);
    u3 = (w1 * 1144) + (((w1 * 8) + (e1 * 25920) + 13835) / 765433).floor() + 1;
    a3 = (((u3 * 19) + 366) / 235).floor();
    e3 = ((u3 * 19) + 366).fmod(235);
    u4 = (e3 / 19).floor() - 6;
    d14 = s - d2(a3, u4);
    u5 = u3 + (d14 / 64).floor();
    a4 = (((u5 * 19) + 366) / 235).floor();
    e4 = ((u5 * 19) + 366).fmod(235);
    u6 = (e4 / 19).floor() - 6;
    d15 = s - d2(a4, u6);
    u7 = u5 + (d15 / 64).floor();
    a1 = (((u7 * 19) + 366) / 235).floor();
    e5 = ((u7 * 19) + 366).fmod(235);
    m1 = (e5 / 19).floor() - 6;
    d0 = s - d2(a1, m1);
    year = a1 + (m1 / 10).floor();
    month = m1 + 7;
    day = d0 + 1;

    result[0] = year;
    result[1] = month;
    result[2] = day;
}

// /*  JD_TO_HEBREW  --  Convert Julian date to Hebrew date
//                       This works by making multiple calls to
//                       the inverse function, and is this very
//                       slow.  */

// void jd_to_hebrew(ti::real jd, real_t result[3])
// {
//     ti::real year, month, day, i, count, first;

//     jd = jd.floor() + 0.5;
//     count = (((jd - HEBREW_EPOCH) * 98496) / 35975351).floor();
//     year = count - 1;
//     for (i = count; jd >= hebrew_to_jd(i, 7, 1); i++)
//     {
//         year++;
//     }
//     first = (jd < hebrew_to_jd(year, 1, 1)) ? 7 : 1;
//     month = first;
//     for (i = first; jd > hebrew_to_jd(year, i, hebrew_month_days(year, i)); i++)
//     {
//         month++;
//     }
//     day = (jd - hebrew_to_jd(year, month, 1)) + 1;

//     result[0] = year;
//     result[1] = month;
//     result[2] = day;
// }

// Islamic

bool leap_islamic(ti::real year)
{
    return (((year * 11) + 14) % 30) < 11;
}

#define ISLAMIC_EPOCH 1948439.5_r

ti::real islamic_to_jd(ti::real year, ti::real month, ti::real day)
{
    return day +
           ((month - 1) * 29.5).ceil() +
           (year - 1) * 354 +
           (((year * 11) + 3) / 30).floor() + ISLAMIC_EPOCH -
           1;
}

void jd_to_islamic(ti::real jd, real_t result[3])
{
    ti::real year, month, day;

    jd = jd.floor() + 0.5;
    year = ((((jd - ISLAMIC_EPOCH) * 30) + 10646) / 10631).floor();
    month = (((jd - (islamic_to_jd(year, 1, 1) + 29)) / 29.5).ceil() + 1).fmin(12);
    day = (jd - islamic_to_jd(year, month, 1)) + 1;

    result[0] = year;
    result[1] = month;
    result[2] = day;
}

//  Persian

#define PERSIAN_EPOCH 1948320.5_r

bool leap_persian(ti::real year)
{
    return ((((((year - ((year > 0) ? 474 : 473)) % 2820) + 474) + 38) * 682) % 2816) < 682;
}

ti::real persian_to_jd(ti::real year, ti::real month, ti::real day)
{
    ti::real epbase, epyear;

    epbase = year - ((year >= 0) ? 474 : 473);
    epyear = epbase.fmod(2820) + 474;

    return day +
           ((month <= 7) ? ((month - 1) * 31) : (((month - 1) * 30) + 6)) +
           (((epyear * 682) - 110) / 2816).floor() +
           (epyear - 1) * 365 +
           (epbase / 2820).floor() * 1029983 +
           (PERSIAN_EPOCH - 1);
}

void jd_to_persian(ti::real jd, real_t result[3])
{
    ti::real year, month, day, depoch, cycle, cyear, ycycle,
        aux1, aux2, yday;

    jd = jd.floor() + 0.5;

    depoch = jd - persian_to_jd(475, 1, 1);
    cycle = (depoch / 1029983).floor();
    cyear = depoch.fmod(1029983);
    if (cyear == 1029982)
    {
        ycycle = 2820;
    }
    else
    {
        aux1 = (cyear / 366).floor();
        aux2 = cyear.fmod(366);
        ycycle = (((aux1 * 2134) + (aux2 * 2816) + 2815) / 1028522).floor() +
                 aux1 + 1;
    }
    year = ycycle + (cycle * 2820) + 474;
    if (year <= 0)
    {
        year--;
    }
    yday = (jd - persian_to_jd(year, 1, 1)) + 1;
    month = (yday <= 186) ? (yday / 31).ceil() : ((yday - 6) / 30).ceil();
    day = (jd - persian_to_jd(year, month, 1)) + 1;

    result[0] = year;
    result[1] = month;
    result[2] = day;
}

// Mayan

#define MAYAN_COUNT_EPOCH 584282.5_r

ti::real mayan_count_to_jd(ti::real baktun, ti::real katun, ti::real tun, ti::real uinal, ti::real kin)
{
    return MAYAN_COUNT_EPOCH +
           (baktun * 144000) +
           (katun * 7200) +
           (tun * 360) +
           (uinal * 20) +
           kin;
}

void jd_to_mayan_count(ti::real jd, real_t result[5])
{
    ti::real d, baktun, katun, tun, uinal, kin;

    jd = jd.floor() + 0.5;
    d = jd - MAYAN_COUNT_EPOCH;
    baktun = (d / 144000).floor();
    d = d.fmod(144000);
    katun = (d / 7200).floor();
    d = d.fmod(7200);
    tun = (d / 360).floor();
    d = d.fmod(360);
    uinal = (d / 20).floor();
    kin = d.fmod(20);

    result[0] = baktun;
    result[1] = katun;
    result[2] = tun;
    result[3] = uinal;
    result[4] = kin;
}

void jd_to_mayan_haab(ti::real jd, real_t result[2])
{
    ti::real lcount, day;

    jd = jd.floor() + 0.5;
    lcount = jd - MAYAN_COUNT_EPOCH;
    day = (lcount + 8 + ((18 - 1) * 20)).fmod(365);

    result[0] = (day / 20).floor() + 1;
    result[1] = day.fmod(20);
}

void jd_to_mayan_tzolkin(ti::real jd, real_t result[2])
{
    ti::real lcount;

    jd = jd.floor() + 0.5;
    lcount = jd - MAYAN_COUNT_EPOCH;
    result[0] = (lcount + 20 - 1).fmod(20) + 1;
    result[1] = (lcount + 4 - 1).fmod(13) + 1;
}

// Indian Civil Calendar

ti::real indian_civil_to_jd(ti::real year, ti::real month, ti::real day)
{
    ti::real Caitra, gyear, leap, start, jd, m;

    gyear = year + 78;
    leap = leap_gregorian(gyear);
    start = gregorian_to_jd(gyear, 3, leap ? 21 : 22);
    Caitra = leap ? 31 : 30;

    if (month == 1)
    {
        jd = start + (day - 1);
    }
    else
    {
        jd = start + Caitra;
        m = month - 2;
        m = m.fmin(5);
        jd += m * 31;
        if (month >= 8)
        {
            m = month - 7;
            jd += m * 30;
        }
        jd += day - 1;
    }

    return jd;
}

//  JD_TO_INDIAN_CIVIL  --  Calculate Indian Civil date from Julian day

void jd_to_indian_civil(ti::real jd, real_t result[3])
{
    ti::real Caitra, Saka, greg0, leap, start, year, yday, mday, month, day;

    real_t greg[3];

    Saka = 79 - 1; // Offset in years from Saka era to Gregorian epoch
    start = 80;    // Day offset between Saka

    jd = jd.floor() + 0.5;
    jd_to_gregorian(jd, greg);              // Gregorian date for Julian day
    leap = leap_gregorian(greg[0]);         // Is this a leap year?
    year = ti::real(greg[0]) - Saka;        // Tentative year in Saka era
    greg0 = gregorian_to_jd(greg[0], 1, 1); // JD at start of Gregorian year
    yday = jd - greg0;                      // Day number (0 based) in Gregorian year
    Caitra = leap ? 31 : 30;                // Days in Caitra this year

    if (yday < start)
    {
        //  Day is at the end of the preceding Saka year
        year--;
        yday += Caitra + (31 * 5) + (30 * 3) + 10 + start;
    }

    yday -= start;
    if (yday < Caitra)
    {
        month = 1;
        day = yday + 1;
    }
    else
    {
        mday = yday - Caitra;
        if (mday < (31 * 5))
        {
            month = (mday / 31).floor() + 2;
            day = (mday % 31) + 1;
        }
        else
        {
            mday -= 31 * 5;
            month = (mday / 30).floor() + 7;
            day = (mday % 30) + 1;
        }
    }

    result[0] = year;
    result[1] = month;
    result[2] = day;
}

int main(void)
{
    real_t real_selection;
    os_GetMatrixElement(MATRIX, 1, 1, &real_selection);
    int24_t selection = os_RealToInt24(&real_selection);
    if (selection == 0)
    {
        real_t year, month, day;
        os_GetMatrixElement(MATRIX, 1, 2, &year);
        os_GetMatrixElement(MATRIX, 1, 3, &month);
        os_GetMatrixElement(MATRIX, 1, 4, &day);
        real_t result = gregorian_to_jd(ti::real(year), ti::real(month), ti::real(day));
        os_SetMatrixElement(MATRIX, 2, 1, &result);
    }
    else if (selection == 1)
    {
        real_t jd;
        os_GetMatrixElement(MATRIX, 1, 2, &jd);
        real_t result[3];
        jd_to_gregorian(ti::real(jd), result);
        os_SetMatrixElement(MATRIX, 2, 1, &result[0]);
        os_SetMatrixElement(MATRIX, 2, 2, &result[1]);
        os_SetMatrixElement(MATRIX, 2, 3, &result[2]);
    }
    else if (selection == 2)
    {
        real_t year, month, day;
        os_GetMatrixElement(MATRIX, 1, 2, &year);
        os_GetMatrixElement(MATRIX, 1, 3, &month);
        os_GetMatrixElement(MATRIX, 1, 4, &day);
        real_t result = julian_to_jd(ti::real(year), ti::real(month), ti::real(day));
        os_SetMatrixElement(MATRIX, 2, 1, &result);
    }
    else if (selection == 3)
    {
        real_t jd;
        os_GetMatrixElement(MATRIX, 1, 2, &jd);
        real_t result[3];
        jd_to_julian(ti::real(jd), result);
        os_SetMatrixElement(MATRIX, 2, 1, &result[0]);
        os_SetMatrixElement(MATRIX, 2, 2, &result[1]);
        os_SetMatrixElement(MATRIX, 2, 3, &result[2]);
    }
    else if (selection == 4)
    {
        real_t year, month, day;
        os_GetMatrixElement(MATRIX, 1, 2, &year);
        os_GetMatrixElement(MATRIX, 1, 3, &month);
        os_GetMatrixElement(MATRIX, 1, 4, &day);
        real_t result = hebrew_to_jd(ti::real(year), ti::real(month), ti::real(day));
        os_SetMatrixElement(MATRIX, 2, 1, &result);
    }
    else if (selection == 5)
    {
        real_t jd;
        os_GetMatrixElement(MATRIX, 1, 2, &jd);
        real_t result[3];
        jd_to_hebrew(ti::real(jd), result);
        os_SetMatrixElement(MATRIX, 2, 1, &result[0]);
        os_SetMatrixElement(MATRIX, 2, 2, &result[1]);
        os_SetMatrixElement(MATRIX, 2, 3, &result[2]);
    }
    else if (selection == 6)
    {
        real_t year, month, day;
        os_GetMatrixElement(MATRIX, 1, 2, &year);
        os_GetMatrixElement(MATRIX, 1, 3, &month);
        os_GetMatrixElement(MATRIX, 1, 4, &day);
        real_t result = islamic_to_jd(ti::real(year), ti::real(month), ti::real(day));
        os_SetMatrixElement(MATRIX, 2, 1, &result);
    }
    else if (selection == 7)
    {
        real_t jd;
        os_GetMatrixElement(MATRIX, 1, 2, &jd);
        real_t result[3];
        jd_to_islamic(ti::real(jd), result);
        os_SetMatrixElement(MATRIX, 2, 1, &result[0]);
        os_SetMatrixElement(MATRIX, 2, 2, &result[1]);
        os_SetMatrixElement(MATRIX, 2, 3, &result[2]);
    }
    else if (selection == 8)
    {
        real_t year, month, day;
        os_GetMatrixElement(MATRIX, 1, 2, &year);
        os_GetMatrixElement(MATRIX, 1, 3, &month);
        os_GetMatrixElement(MATRIX, 1, 4, &day);
        real_t result = persian_to_jd(ti::real(year), ti::real(month), ti::real(day));
        os_SetMatrixElement(MATRIX, 2, 1, &result);
    }
    else if (selection == 9)
    {
        real_t jd;
        os_GetMatrixElement(MATRIX, 1, 2, &jd);
        real_t result[3];
        jd_to_persian(ti::real(jd), result);
        os_SetMatrixElement(MATRIX, 2, 1, &result[0]);
        os_SetMatrixElement(MATRIX, 2, 2, &result[1]);
        os_SetMatrixElement(MATRIX, 2, 3, &result[2]);
    }
    else if (selection == 10)
    {
        real_t baktun, katun, tun, uinal, kin;
        os_GetMatrixElement(MATRIX, 1, 2, &baktun);
        os_GetMatrixElement(MATRIX, 1, 3, &katun);
        os_GetMatrixElement(MATRIX, 1, 4, &tun);
        os_GetMatrixElement(MATRIX, 1, 5, &uinal);
        os_GetMatrixElement(MATRIX, 1, 6, &kin);
        real_t result = mayan_count_to_jd(ti::real(baktun), ti::real(katun), ti::real(tun), ti::real(uinal), ti::real(kin));
        os_SetMatrixElement(MATRIX, 2, 1, &result);
    }
    else if (selection == 11)
    {
        real_t jd;
        os_GetMatrixElement(MATRIX, 1, 2, &jd);
        real_t count_result[5];
        jd_to_mayan_count(ti::real(jd), count_result);
        real_t haab_result[2];
        jd_to_mayan_haab(ti::real(jd), haab_result);
        real_t tzolkin_result[2];
        jd_to_mayan_tzolkin(ti::real(jd), tzolkin_result);
        os_SetMatrixElement(MATRIX, 2, 1, &count_result[0]);
        os_SetMatrixElement(MATRIX, 2, 2, &count_result[1]);
        os_SetMatrixElement(MATRIX, 2, 3, &count_result[2]);
        os_SetMatrixElement(MATRIX, 2, 4, &count_result[3]);
        os_SetMatrixElement(MATRIX, 2, 5, &count_result[4]);
        os_SetMatrixElement(MATRIX, 2, 6, &haab_result[0]);
        os_SetMatrixElement(MATRIX, 2, 7, &haab_result[1]);
        os_SetMatrixElement(MATRIX, 2, 8, &tzolkin_result[0]);
        os_SetMatrixElement(MATRIX, 2, 9, &tzolkin_result[1]);
    }
    else if (selection == 12)
    {
        real_t year, month, day;
        os_GetMatrixElement(MATRIX, 1, 2, &year);
        os_GetMatrixElement(MATRIX, 1, 3, &month);
        os_GetMatrixElement(MATRIX, 1, 4, &day);
        real_t result = indian_civil_to_jd(ti::real(year), ti::real(month), ti::real(day));
        os_SetMatrixElement(MATRIX, 2, 1, &result);
    }
    else if (selection == 13)
    {
        real_t jd;
        os_GetMatrixElement(MATRIX, 1, 2, &jd);
        real_t result[3];
        jd_to_indian_civil(ti::real(jd), result);
        os_SetMatrixElement(MATRIX, 2, 1, &result[0]);
        os_SetMatrixElement(MATRIX, 2, 2, &result[1]);
        os_SetMatrixElement(MATRIX, 2, 3, &result[2]);
    }

    return 0;
}
