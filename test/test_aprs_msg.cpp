#include <APRS_MSG.h>
#include <unity.h>


APRS_MSG amsg;

void test_mph2kmh(void) {
    TEST_ASSERT_EQUAL(129, APRS_MSG::mph2kmh(80));
    TEST_ASSERT_EQUAL(0, APRS_MSG::mph2kmh(0));
    TEST_ASSERT_EQUAL(409, APRS_MSG::mph2kmh(254));
    TEST_ASSERT_EQUAL(483, APRS_MSG::mph2kmh(300));
}

void test_kmh2mph(void) {
    TEST_ASSERT_EQUAL(80, APRS_MSG::kmh2mph(129));
    TEST_ASSERT_EQUAL(0, APRS_MSG::kmh2mph(0));
    TEST_ASSERT_EQUAL(7, APRS_MSG::kmh2mph(12));
    TEST_ASSERT_EQUAL(186, APRS_MSG::kmh2mph(300));
}

void test_hinch2mm(void) {
    TEST_ASSERT_EQUAL(0, APRS_MSG::hinch2mm(0));
    TEST_ASSERT_EQUAL(25, APRS_MSG::hinch2mm(100));
    TEST_ASSERT_EQUAL(89, APRS_MSG::hinch2mm(350));
}

void test_mm2hinch(void) {
    TEST_ASSERT_EQUAL(0, APRS_MSG::mm2hinch(0));
    TEST_ASSERT_EQUAL(106, APRS_MSG::mm2hinch(27));
    TEST_ASSERT_EQUAL(1575, APRS_MSG::mm2hinch(400));
}


void test_f2c(void) {
    TEST_ASSERT_EQUAL(0, APRS_MSG::f2c(32));
    TEST_ASSERT_EQUAL(-17, APRS_MSG::f2c(0));
    TEST_ASSERT_EQUAL(-23, APRS_MSG::f2c(-10));
    TEST_ASSERT_EQUAL(50, APRS_MSG::f2c(123));
    TEST_ASSERT_EQUAL(125, APRS_MSG::f2c(257));
}

void test_c2f(void) {
    TEST_ASSERT_EQUAL(32, APRS_MSG::c2f(0));
    TEST_ASSERT_EQUAL(5, APRS_MSG::c2f(-15));
    TEST_ASSERT_EQUAL(-36, APRS_MSG::c2f(-38));
    TEST_ASSERT_EQUAL(212, APRS_MSG::c2f(100));
    TEST_ASSERT_EQUAL(572, APRS_MSG::c2f(300));
}


void test_feed2meter(void){
    TEST_ASSERT_EQUAL(0, APRS_MSG::feed2meter(0));
    TEST_ASSERT_EQUAL(0, APRS_MSG::feed2meter(1));
    TEST_ASSERT_EQUAL(1, APRS_MSG::feed2meter(2));
    TEST_ASSERT_EQUAL(37, APRS_MSG::feed2meter(121));
};

void test_meter2feed(void){
    TEST_ASSERT_EQUAL(0, APRS_MSG::meter2feed(0));
    TEST_ASSERT_EQUAL(3, APRS_MSG::meter2feed(1));
    TEST_ASSERT_EQUAL(397, APRS_MSG::meter2feed(121));
};

void test_getWXField(void) {
    amsg.reset();
    TEST_ASSERT_EQUAL_STRING(".../...g...t...r...p...P...h..b....", amsg.getWXField());
    amsg.setHumidity(100);
    TEST_ASSERT_EQUAL_STRING(".../...g...t...r...p...P...h00b....", amsg.getWXField());
    amsg.setHumidity(10);
    TEST_ASSERT_EQUAL_STRING(".../...g...t...r...p...P...h10b....", amsg.getWXField());
    amsg.setTemperatureCelsius(0);
    TEST_ASSERT_EQUAL_STRING(".../...g...t032r...p...P...h10b....", amsg.getWXField());
    amsg.setTemperatureCelsius(-38);
    TEST_ASSERT_EQUAL_STRING(".../...g...t-36r...p...P...h10b....", amsg.getWXField());
    amsg.setTemperatureFahrenheit(0);
    TEST_ASSERT_EQUAL_STRING(".../...g...t000r...p...P...h10b....", amsg.getWXField());
    amsg.reset();
    amsg.setGustKmh(13);
    TEST_ASSERT_EQUAL_STRING(".../...g008t...r...p...P...h..b....", amsg.getWXField());
    amsg.reset();
    amsg.setWindDirection(321);
    TEST_ASSERT_EQUAL_STRING("321/...g...t...r...p...P...h..b....", amsg.getWXField());
    amsg.reset();
    amsg.setWindSpeedKmh(100);
    TEST_ASSERT_EQUAL_STRING(".../062g...t...r...p...P...h..b....", amsg.getWXField());
    amsg.reset();
    amsg.setRainFallLastHourMM(25);
    TEST_ASSERT_EQUAL_STRING(".../...g...t...r098p...P...h..b....", amsg.getWXField());
    amsg.reset();
    amsg.setRainFallLast24MM(15);
    TEST_ASSERT_EQUAL_STRING(".../...g...t...r...p059P...h..b....", amsg.getWXField());
    amsg.reset();
    amsg.setRainFallSinceMidNightMM(42);
    TEST_ASSERT_EQUAL_STRING(".../...g...t...r...p...P165h..b....", amsg.getWXField());
    amsg.reset();
    amsg.setPressure(1024);
    TEST_ASSERT_EQUAL_STRING(".../...g...t...r...p...P...h..b1024", amsg.getWXField());
    amsg.reset();
    amsg.setWXDevice("BMC280");
    TEST_ASSERT_EQUAL_STRING(".../...g...t...r...p...P...h..b....BMC280", amsg.getWXField());
}



void test_setLng(void) {
    amsg.reset();
    amsg.setLat(52.484638214111);
    amsg.setLng(13.575653076172);
    TEST_ASSERT_EQUAL_STRING("5229.04N/01334.32E[", amsg.getPos());
    amsg.reset();
    amsg.setLat(53.623864213380024);
    amsg.setLng(-8.836456298828015);
    TEST_ASSERT_EQUAL_STRING("5337.25N/00850.11W[", amsg.getPos());
    amsg.reset();
    amsg.setLat(-12.130876089066753);
    amsg.setLng(-77.03958129882801);
    TEST_ASSERT_EQUAL_STRING("1207.51S/07702.22W[", amsg.getPos());
    amsg.reset();
    amsg.setLat(-33.997938361803044);
    amsg.setLng(18.409637451171985);
    TEST_ASSERT_EQUAL_STRING("3359.52S/01824.34E[", amsg.getPos());
    amsg.reset();
    amsg.setLat(0);
    amsg.setLng(0);
    TEST_ASSERT_EQUAL_STRING("0000.00N/00000.00E[", amsg.getPos());
    amsg.reset();
    amsg.setLat(90);
    amsg.setLng(180);
    TEST_ASSERT_EQUAL_STRING("9000.00N/18000.00E[", amsg.getPos());
    amsg.setLat(64.69660065860629);
    amsg.setLng(177.579559326172);
    TEST_ASSERT_EQUAL_STRING("6441.47N/17734.46E[", amsg.getPos());


}

void test_APRSsymbol(void) {
    amsg.reset();
    amsg.setAPRSSymbolTable(APRS_MSG::alternate);
    amsg.setAPRSSymbol('_');
    amsg.setLat(-33.997938361803044);
    amsg.setLng(18.409637451171985);
    TEST_ASSERT_EQUAL_STRING("3359.52S\\01824.34E_", amsg.getPos());
}

// void test_getAltitudeFeed(void) {
//     amsg.setAltitudeFeed(0);
//     char got[7];
//     TEST_ASSERT_EQUAL_STRING("0", amsg.getAltitudeFeed(got));
//     TEST_ASSERT_EQUAL_STRING("0", amsg.getAltitudeMeter(got));
//     amsg.setAltitudeFeed(42);
//     TEST_ASSERT_EQUAL_STRING("42", amsg.getAltitudeFeed(got));
//     TEST_ASSERT_EQUAL_STRING("13", amsg.getAltitudeMeter(got));
//     amsg.setAltitudeFeed(100042);
//     TEST_ASSERT_EQUAL_STRING("100042", amsg.getAltitudeFeed(got));
//     TEST_ASSERT_EQUAL_STRING("30493", amsg.getAltitudeMeter(got));
// };




void test_getTrackInfo(void) {
    amsg.reset();
    amsg.setAPRSSymbolTable(APRS_MSG::alternate);
    amsg.setAPRSSymbol('>');
    amsg.setLat(-33.997938361803044);
    amsg.setLng(18.409637451171985);
    amsg.setAltitudeFeed(121);
    amsg.setSpeedmph(6);
    amsg.setCourse(142);
    TEST_ASSERT_EQUAL_STRING("3359.52S\\01824.34E>006/142/A=000121", amsg.getTrackInfo());

}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_mph2kmh);
    RUN_TEST(test_kmh2mph);
    RUN_TEST(test_hinch2mm);
    RUN_TEST(test_mm2hinch);
    RUN_TEST(test_f2c);
    RUN_TEST(test_c2f);
    RUN_TEST(test_getWXField);
    RUN_TEST(test_setLng);
    RUN_TEST(test_APRSsymbol);
    RUN_TEST(test_feed2meter);
    RUN_TEST(test_meter2feed);
    //RUN_TEST(test_getAltitudeFeed); // no getter yet
    RUN_TEST(test_getTrackInfo);
    UNITY_END();

    return 0;
}
