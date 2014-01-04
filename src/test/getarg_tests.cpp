#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-RCK");
    BOOST_CHECK(GetBoolArg("-RCK"));
    BOOST_CHECK(GetBoolArg("-RCK", false));
    BOOST_CHECK(GetBoolArg("-RCK", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-RCKo"));
    BOOST_CHECK(!GetBoolArg("-RCKo", false));
    BOOST_CHECK(GetBoolArg("-RCKo", true));

    ResetArgs("-RCK=0");
    BOOST_CHECK(!GetBoolArg("-RCK"));
    BOOST_CHECK(!GetBoolArg("-RCK", false));
    BOOST_CHECK(!GetBoolArg("-RCK", true));

    ResetArgs("-RCK=1");
    BOOST_CHECK(GetBoolArg("-RCK"));
    BOOST_CHECK(GetBoolArg("-RCK", false));
    BOOST_CHECK(GetBoolArg("-RCK", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noRCK");
    BOOST_CHECK(!GetBoolArg("-RCK"));
    BOOST_CHECK(!GetBoolArg("-RCK", false));
    BOOST_CHECK(!GetBoolArg("-RCK", true));

    ResetArgs("-noRCK=1");
    BOOST_CHECK(!GetBoolArg("-RCK"));
    BOOST_CHECK(!GetBoolArg("-RCK", false));
    BOOST_CHECK(!GetBoolArg("-RCK", true));

    ResetArgs("-RCK -noRCK");  // -RCK should win
    BOOST_CHECK(GetBoolArg("-RCK"));
    BOOST_CHECK(GetBoolArg("-RCK", false));
    BOOST_CHECK(GetBoolArg("-RCK", true));

    ResetArgs("-RCK=1 -noRCK=1");  // -RCK should win
    BOOST_CHECK(GetBoolArg("-RCK"));
    BOOST_CHECK(GetBoolArg("-RCK", false));
    BOOST_CHECK(GetBoolArg("-RCK", true));

    ResetArgs("-RCK=0 -noRCK=0");  // -RCK should win
    BOOST_CHECK(!GetBoolArg("-RCK"));
    BOOST_CHECK(!GetBoolArg("-RCK", false));
    BOOST_CHECK(!GetBoolArg("-RCK", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--RCK=1");
    BOOST_CHECK(GetBoolArg("-RCK"));
    BOOST_CHECK(GetBoolArg("-RCK", false));
    BOOST_CHECK(GetBoolArg("-RCK", true));

    ResetArgs("--noRCK=1");
    BOOST_CHECK(!GetBoolArg("-RCK"));
    BOOST_CHECK(!GetBoolArg("-RCK", false));
    BOOST_CHECK(!GetBoolArg("-RCK", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-RCK", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-RCK", "eleven"), "eleven");

    ResetArgs("-RCK -RCK");
    BOOST_CHECK_EQUAL(GetArg("-RCK", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-RCK", "eleven"), "");

    ResetArgs("-RCK=");
    BOOST_CHECK_EQUAL(GetArg("-RCK", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-RCK", "eleven"), "");

    ResetArgs("-RCK=11");
    BOOST_CHECK_EQUAL(GetArg("-RCK", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-RCK", "eleven"), "11");

    ResetArgs("-RCK=eleven");
    BOOST_CHECK_EQUAL(GetArg("-RCK", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-RCK", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-RCK", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-RCK", 0), 0);

    ResetArgs("-RCK -RCK");
    BOOST_CHECK_EQUAL(GetArg("-RCK", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-RCK", 11), 0);

    ResetArgs("-RCK=11 -RCK=12");
    BOOST_CHECK_EQUAL(GetArg("-RCK", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-RCK", 11), 12);

    ResetArgs("-RCK=NaN -RCK=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-RCK", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-RCK", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--RCK");
    BOOST_CHECK_EQUAL(GetBoolArg("-RCK"), true);

    ResetArgs("--RCK=verbose --RCK=1");
    BOOST_CHECK_EQUAL(GetArg("-RCK", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-RCK", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noRCK");
    BOOST_CHECK(!GetBoolArg("-RCK"));
    BOOST_CHECK(!GetBoolArg("-RCK", true));
    BOOST_CHECK(!GetBoolArg("-RCK", false));

    ResetArgs("-noRCK=1");
    BOOST_CHECK(!GetBoolArg("-RCK"));
    BOOST_CHECK(!GetBoolArg("-RCK", true));
    BOOST_CHECK(!GetBoolArg("-RCK", false));

    ResetArgs("-noRCK=0");
    BOOST_CHECK(GetBoolArg("-RCK"));
    BOOST_CHECK(GetBoolArg("-RCK", true));
    BOOST_CHECK(GetBoolArg("-RCK", false));

    ResetArgs("-RCK --noRCK");
    BOOST_CHECK(GetBoolArg("-RCK"));

    ResetArgs("-noRCK -RCK"); // RCK always wins:
    BOOST_CHECK(GetBoolArg("-RCK"));
}

BOOST_AUTO_TEST_SUITE_END()
