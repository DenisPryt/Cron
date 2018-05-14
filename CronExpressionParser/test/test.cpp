#include <Cron.h>

#include <date/date.h>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>


void CheckNext(const char* pattern, const char* initial, const char* expected)
{
	using namespace Cron;
	using namespace std::string_literals;
	using namespace std::chrono;
	using namespace boost::posix_time;


	std::stringstream ssInitial{initial};
	system_clock::time_point initialTime;
	::date::from_stream(ssInitial, "%F %T", initialTime);

	std::istringstream ssExpected{expected};
	system_clock::time_point expectedTime;
	::date::from_stream(ssExpected, "%F %T", expectedTime);

	try
	{
		Expression expr = Expression::ParseCronExpression(pattern);
		auto nextTime = expr.NextTime(initialTime);
		std::cout << pattern << " Next " << (expectedTime == nextTime ? " OK" : " FAIL") << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cout << pattern << " ERROR " << ex.what() << std::endl;
	}
}


void CheckPrev(const char* pattern, const char* initial, const char* expected)
{
	using namespace Cron;
	using namespace std::string_literals;
	using namespace std::chrono;
	using namespace boost::posix_time;


	std::stringstream ssInitial{initial};
	system_clock::time_point initialTime;
	::date::from_stream(ssInitial, "%F %T", initialTime);

	std::istringstream ssExpected{expected};
	system_clock::time_point expectedTime;
	::date::from_stream(ssExpected, "%F %T", expectedTime);

	try
	{
		Expression expr = Expression::ParseCronExpression(pattern);
		
		auto prevTime = expr.PrevTime(initialTime);
		std::cout << pattern << " Prev " << (expectedTime == prevTime ? " OK" : " FAIL") << std::endl;

		std::cout << to_simple_string(time_from_string(initial)) << std::endl;
		std::cout << to_simple_string(time_from_string(expected)) << std::endl;
		std::cout << to_simple_string(from_time_t(system_clock::to_time_t(prevTime))) << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cout << pattern << " ERROR " << ex.what() << std::endl;
	}
}


void CheckNextAll()
{
	return;
	CheckNext("*/15 * 1-4 * * *", "2012-07-01 09:53:50", "2012-07-02 01:00:00");
	CheckNext("*/15 * 1-4 * * *", "2012-07-01 09:53:00", "2012-07-02 01:00:00");
	CheckNext("0 */2 1-4 * * *", "2012-07-01 09:00:00", "2012-07-02 01:00:00");
	CheckNext("* * * * * *", "2012-07-01 09:00:00", "2012-07-01 09:00:01");
	CheckNext("* * * * * *", "2012-12-01 09:00:58", "2012-12-01 09:00:59");
	CheckNext("10 * * * * *", "2012-12-01 09:42:09", "2012-12-01 09:42:10");
	CheckNext("11 * * * * *", "2012-12-01 09:42:10", "2012-12-01 09:42:11");
	CheckNext("10 * * * * *", "2012-12-01 09:42:10", "2012-12-01 09:43:10");
	CheckNext("10-15 * * * * *", "2012-12-01 09:42:09", "2012-12-01 09:42:10");
	CheckNext("10-15 * * * * *", "2012-12-01 21:42:14", "2012-12-01 21:42:15");
	CheckNext("0 * * * * *", "2012-12-01 21:10:42", "2012-12-01 21:11:00");
	CheckNext("0 * * * * *", "2012-12-01 21:11:00", "2012-12-01 21:12:00");
	CheckNext("0 11 * * * *", "2012-12-01 21:10:42", "2012-12-01 21:11:00");
	CheckNext("0 10 * * * *", "2012-12-01 21:11:00", "2012-12-01 22:10:00");
	CheckNext("0 0 * * * *", "2012-09-30 11:01:00", "2012-09-30 12:00:00");
	CheckNext("0 0 * * * *", "2012-09-30 12:00:00", "2012-09-30 13:00:00");
	CheckNext("0 0 * * * *", "2012-09-10 23:01:00", "2012-09-11 00:00:00");
	CheckNext("0 0 * * * *", "2012-09-11 00:00:00", "2012-09-11 01:00:00");
	CheckNext("0 0 0 * * *", "2012-09-01 14:42:43", "2012-09-02 00:00:00");
	CheckNext("0 0 0 * * *", "2012-09-02 00:00:00", "2012-09-03 00:00:00");
	CheckNext("* * * 10 * *", "2012-10-09 15:12:42", "2012-10-10 00:00:00");
	CheckNext("* * * 10 * *", "2012-10-11 15:12:42", "2012-11-10 00:00:00");
	CheckNext("0 0 0 * * *", "2012-09-30 15:12:42", "2012-10-01 00:00:00");
	CheckNext("0 0 0 * * *", "2012-10-01 00:00:00", "2012-10-02 00:00:00");
	CheckNext("0 0 0 * * *", "2012-08-30 15:12:42", "2012-08-31 00:00:00");
	CheckNext("0 0 0 * * *", "2012-08-31 00:00:00", "2012-09-01 00:00:00");
	CheckNext("0 0 0 * * *", "2012-10-30 15:12:42", "2012-10-31 00:00:00");
	CheckNext("0 0 0 * * *", "2012-10-31 00:00:00", "2012-11-01 00:00:00");
	CheckNext("0 0 0 1 * *", "2012-10-30 15:12:42", "2012-11-01 00:00:00");
	CheckNext("0 0 0 1 * *", "2012-11-01 00:00:00", "2012-12-01 00:00:00");
	CheckNext("0 0 0 1 * *", "2010-12-31 15:12:42", "2011-01-01 00:00:00");
	CheckNext("0 0 0 1 * *", "2011-01-01 00:00:00", "2011-02-01 00:00:00");
	CheckNext("0 0 0 31 * *", "2011-10-30 15:12:42", "2011-10-31 00:00:00");
	CheckNext("0 0 0 1 * *", "2011-10-30 15:12:42", "2011-11-01 00:00:00");
	CheckNext("* * * * * 2", "2010-10-25 15:12:42", "2010-10-26 00:00:00");
	CheckNext("* * * * * 2", "2010-10-20 15:12:42", "2010-10-26 00:00:00");
	CheckNext("* * * * * 2", "2010-10-27 15:12:42", "2010-11-02 00:00:00");
	CheckNext("55 5 * * * *", "2010-10-27 15:04:54", "2010-10-27 15:05:55");
	CheckNext("55 5 * * * *", "2010-10-27 15:05:55", "2010-10-27 16:05:55");
	CheckNext("55 * 10 * * *", "2010-10-27 09:04:54", "2010-10-27 10:00:55");
	CheckNext("55 * 10 * * *", "2010-10-27 10:00:55", "2010-10-27 10:01:55");
	CheckNext("* 5 10 * * *", "2010-10-27 09:04:55", "2010-10-27 10:05:00");
	CheckNext("* 5 10 * * *", "2010-10-27 10:05:00", "2010-10-27 10:05:01");
	CheckNext("55 * * 3 * *", "2010-10-02 10:05:54", "2010-10-03 00:00:55");
	CheckNext("55 * * 3 * *", "2010-10-03 00:00:55", "2010-10-03 00:01:55");
	CheckNext("* * * 3 11 *", "2010-10-02 14:42:55", "2010-11-03 00:00:00");
	CheckNext("* * * 3 11 *", "2010-11-03 00:00:00", "2010-11-03 00:00:01");
	CheckNext("0 0 0 29 2 *", "2007-02-10 14:42:55", "2008-02-29 00:00:00");
	CheckNext("0 0 0 29 2 *", "2008-02-29 00:00:00", "2012-02-29 00:00:00");

	CheckNext("0 0 7 ? * mon-FRI", "2009-09-26 00:42:55", "2009-09-28 07:00:00");
	CheckNext("0 0 7 ? * MON-FrI", "2009-09-28 07:00:00", "2009-09-29 07:00:00");

	CheckNext("0 30 23 30 1/3 ?", "2010-12-30 00:00:00", "2011-01-30 23:30:00");
	CheckNext("0 30 23 30 1/3 ?", "2011-01-30 23:30:00", "2011-04-30 23:30:00");
	CheckNext("0 30 23 30 1/3 ?", "2011-04-30 23:30:00", "2011-07-30 23:30:00");

	CheckNext("0 0 7 * * 5L", "2018-04-09 07:00:00", "2018-04-27 07:00:00");
	CheckNext("0 0 7 * * 1L", "2018-04-09 07:00:00", "2018-04-30 07:00:00");
	CheckNext("0 0 7 L * 5L", "2018-04-09 07:00:00", "2018-08-31 07:00:00");

	CheckNext("3/15 3-40/8 5-9 * * 4L", "2018-04-26 06:13:35", "2018-04-26 06:19:03");
	CheckNext("3/15 3-40/8 5-9 * * 4#3", "2018-04-10 06:13:35", "2018-04-19 05:03:03");
}


void CheckPrevAll()
{
	CheckPrev("*/15 * 1-4 * * *", "2018-07-01 09:53:50", "2018-07-01 04:59:45");
	CheckPrev("*/15 * 3-7 * * *", "2018-07-01 02:53:40", "2018-06-30 07:59:45");
	CheckPrev("0 */2 1-4 * * *", "2018-07-01 09:00:00", "2018-07-01 04:58:00");
	CheckPrev("* * * * * *", "2012-07-01 09:00:00", "2012-07-01 08:59:59");
	CheckPrev("* * * * * *", "2020-03-01 00:00:00", "2020-02-29 23:59:59");
	
	CheckPrev("0 0 7 ? * mon-FRI", "2009-09-26 00:42:55", "2009-09-25 07:00:00");
	CheckPrev("0 0 7 ? * MON-FrI", "2009-09-28 07:00:00", "2009-09-25 07:00:00");
	
	CheckPrev("0 30 23 30 1/3 ?", "2010-12-30 00:00:00", "2010-10-30 23:30:00");
	CheckPrev("0 30 23 * 2/3 ?" , "2011-01-30 23:30:00", "2010-11-30 23:30:00");
	CheckPrev("0 30 23 31 3-6 ?", "2011-02-28 23:30:00", "2010-05-31 23:30:00");
	
	CheckPrev("0 0 7 * * 5L", "2018-04-30 06:59:59", "2018-04-27 07:00:00");
	CheckPrev("0 0 7 * * 5L", "2018-04-27 07:01:01", "2018-04-27 07:00:00");
	CheckPrev("0 0 7 * * 5L", "2018-04-27 06:59:59", "2018-03-30 07:00:00");
	CheckPrev("0 0 7 * * 1L", "2018-04-09 07:01:01", "2018-03-26 07:00:00");
	CheckPrev("0 0 7 L * 5L", "2018-04-09 07:00:00", "2017-06-30 07:00:00");
	
	CheckPrev("3/15 3-40/8 5-9 * * 4#1", "2018-04-26 06:13:35", "2018-04-05 09:35:48");
	CheckPrev("3/15 3-40/8 5-9 * * 7#1", "2018-04-10 06:13:35", "2018-04-01 09:35:48");

	CheckPrev("* * * * * 2#2", "2018-05-07 06:13:35", "2018-04-10 23:59:59");
	CheckPrev("* * * * * 4#5", "2018-05-30 06:13:35", "2018-03-29 23:59:59");
}


int main()
{
	CheckNextAll();
	CheckPrevAll();

	return 0;
}
