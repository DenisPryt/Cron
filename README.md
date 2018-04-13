Cron expression parsing in modern C++ usind chrono and date libraries
===================================================================

Given a cron expression and a date, you can get the next date which satisfies the cron expression.

Supports cron expressions with seconds.

Supports non-standard characters '/', 'L', '#'. (https://en.wikipedia.org/wiki/Cron#Non-Standard_Characters)

**Usage example**
-------------

    #include <Cron.h>

    using namespace Cron;
    using namespace std::chrono;
    
    Expression expr = Expression::ParseCronExpression(3/15 3-40/8 5-9 * * 4L);
    system_clock::time_point nextTime = expr.NextTime(system_clock::now());

**Support for the following cron expressions:**
-------------

```
Field name   | Allowed values  | Allowed special characters
------------------------------------------------------------
Seconds      | 0-59            | * , - /
Minutes      | 0-59            | * , - /
Hours        | 0-23            | * , - /
Day of month | 1-31            | * , - / ? L
Month        | 1-12 or JAN-DEC | * , - /
Day of week  | 0-6 or SUN-SAT  | * , - / ? L #
```

**Examples expressions**
--------------------

Expression, input date, next date:

    "0 0 7 ? * mon-FRI"      , "2009-09-26 00:42:55", "2009-09-28 07:00:00"
    "0 30 23 30 1/3 ?"       , "2011-04-30_23:30:00", "2011-07-30_23:30:00"
    "0 0 7 L * 5L"           , "2018-04-09 07:00:00", "2018-08-31 07:00:00"
    "3/15 3-40/8 5-9 * * 4L" , "2018-04-26 06:13:35", "2018-04-26 06:19:03"
    "3/15 3-40/8 5-9 * * 4#3", "2018-04-10 06:13:35", "2018-04-19 05:03:03"
    
