#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "gtestd.lib")
#else
#pragma comment(lib, "gtest.lib")
#endif
#endif

#define NO_COOLQ

#include <climits>
#include <cmath>
#include <list>
#include <regex>

#include "gtest/gtest.h"

#include "dicebot/dicebot.h"

class entry_test : public ::testing::Test {
protected:
    entry_test() { dicebot::initialize("./build/test_db/"); }
    ~entry_test() { dicebot::salvage(); }

public:
    bool test_call(dicebot::event_info &ei, const std::string &source,
                   const std::regex &reg_test) {
        std::string output;
        dicebot::try_fill_nickname(ei);
        dicebot::message_pipeline(source, ei, output);
        const char *s = output.c_str();
        return std::regex_search(output, reg_test);
    }

    void base_call(dicebot::event_info &ei, const std::string &source) {
        std::string output;
        dicebot::try_fill_nickname(ei);
        dicebot::message_pipeline(source, ei, output);
    }
};

TEST_F(entry_test, roll_2d20plus4) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";

    std::string source_1 = ".r2d20+4";
    std::string source_2 = ".r 2d20+4";
    std::string source_3 = ".roll2d20+4";
    std::string source_4 = ".roll 2d20+4";

    this->base_call(ei, ".ndice");

    std::regex result_reg(
        u8"^ \\* dice 掷骰: 2d20\\+4 = \\[\\d{1,2} \\+ \\d{1,2}\\]\\+4 = "
        u8"\\d{1,2}$");
    std::regex result_reg_s(u8"^ \\* dice 掷骰: 2d20\\+4 = \\d{1,2}$");
    std::regex result_reg_s_msg(u8"^ \\* dice test 掷骰: 2d20\\+4 = \\d{1,2}$");

    std::regex roll_source_on(u8"^ \\* dice 启用骰子详细输出$");
    ASSERT_TRUE(this->test_call(ei, ".rson", roll_source_on));
    ASSERT_TRUE(this->test_call(ei, ".r s on", roll_source_on));
    ASSERT_TRUE(this->test_call(ei, ".r s on", roll_source_on));
    ASSERT_TRUE(this->test_call(ei, ".roll source on", roll_source_on));
    ASSERT_TRUE(this->test_call(ei, source_1, result_reg));
    ASSERT_TRUE(this->test_call(ei, source_2, result_reg));
    ASSERT_TRUE(this->test_call(ei, source_3, result_reg));
    ASSERT_TRUE(this->test_call(ei, source_4, result_reg));

    std::regex roll_source_off(u8"^ \\* dice 关闭骰子详细输出$");
    ASSERT_TRUE(this->test_call(ei, ".rsoff", roll_source_off));
    ASSERT_TRUE(this->test_call(ei, ".r s off", roll_source_off));
    ASSERT_TRUE(this->test_call(ei, ".r s off", roll_source_off));
    ASSERT_TRUE(this->test_call(ei, ".roll source off", roll_source_off));
    ASSERT_TRUE(this->test_call(ei, source_1, result_reg_s));
    ASSERT_TRUE(this->test_call(ei, source_2, result_reg_s));
    ASSERT_TRUE(this->test_call(ei, source_3, result_reg_s));
    ASSERT_TRUE(this->test_call(ei, source_4, result_reg_s));
    ASSERT_TRUE(this->test_call(ei, ".rs2d20+4", result_reg));
    ASSERT_TRUE(this->test_call(ei, ".r s 2d20+4", result_reg));
    ASSERT_TRUE(this->test_call(ei, ".roll s 2d20+4", result_reg));
    ASSERT_TRUE(this->test_call(ei, ".roll source 2d20+4", result_reg));

    this->base_call(ei, ".rsoff");
    ASSERT_TRUE(this->test_call(ei, ".r 2d20+4", result_reg_s));
    ASSERT_TRUE(this->test_call(ei, ".r 2d20+4 ", result_reg_s));
    ASSERT_TRUE(this->test_call(ei, " . r 2d20+4 ", result_reg_s));
    ASSERT_TRUE(this->test_call(ei, ".r 2d20+4test", result_reg_s_msg));
    ASSERT_TRUE(this->test_call(ei, ".r 2d20+4 test", result_reg_s_msg));
    ASSERT_TRUE(this->test_call(ei, ".r 2d20+4 test", result_reg_s_msg));
}

TEST_F(entry_test, roll_6sharp4d6kl3) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";

    std::string source = ".r6#4d6kl3";

    this->base_call(ei, ".ndice");
    this->base_call(ei, ".rson");

    std::regex result_reg(
        u8"^ \\* dice 掷骰: \\{(4d6kl3, ){5}4d6kl3\\} = \\{(\\[\\d\\*? \\+ "
        u8"\\d\\*? \\+ \\d\\*? \\+ \\d\\*?\\],){5}(\\[\\d\\*? \\+ "
        u8"\\d\\*? \\+ \\d\\*? \\+ \\d\\*?\\])\\} = \\{(\\d{1,2}, "
        u8"){5}\\d{1,2}\\}$");
    ASSERT_TRUE(this->test_call(ei, source, result_reg));

    ASSERT_TRUE(this->test_call(
        ei, ".rsoff", std::regex(u8"^ \\* dice 关闭骰子详细输出$")));
    std::regex result_reg_s(
        u8"^ \\* dice 掷骰: \\{(4d6kl3, ){5}4d6kl3\\} = \\{(\\d{1,2}, "
        u8"){5}\\d{1,2}\\}$");
    ASSERT_TRUE(this->test_call(ei, source, result_reg_s));
    ASSERT_TRUE(this->test_call(ei, ".rs6#4d6kl3", result_reg));
}

TEST_F(entry_test, name_dice) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";

    ei.group_id = 10001;
    ASSERT_TRUE(this->test_call(
        ei, ".ndice1", std::regex(u8"^ \\* .* 的新名字是 dice1$")));
    ASSERT_TRUE(this->test_call(
        ei, ".namedice1", std::regex(u8"^ \\* dice1 的新名字是 dice1$")));
    ASSERT_TRUE(this->test_call(
        ei, ".n   dice1", std::regex(u8"^ \\* dice1 的新名字是 dice1$")));
    ASSERT_TRUE(this->test_call(
        ei, ".name   dice1", std::regex(u8"^ \\* dice1 的新名字是 dice1$")));

    ei.group_id = 10002;
    ASSERT_TRUE(this->test_call(
        ei, ".ndice2", std::regex(u8"^ \\* .* 的新名字是 dice2$")));
    ASSERT_TRUE(this->test_call(
        ei, ".namedice2", std::regex(u8"^ \\* dice2 的新名字是 dice2$")));
    ASSERT_TRUE(this->test_call(
        ei, ".n   dice2", std::regex(u8"^ \\* dice2 的新名字是 dice2$")));
    ASSERT_TRUE(this->test_call(
        ei, ".name   dice2", std::regex(u8"^ \\* dice2 的新名字是 dice2$")));

    std::regex result_reg_r1(
        u8"^ \\* dice1 掷骰: 2d20\\+4 = \\[\\d{1,2} \\+ \\d{1,2}\\]\\+4 = "
        u8"\\d{1,2}$");
    std::regex result_reg_r2(
        u8"^ \\* dice2 掷骰: 2d20\\+4 = \\[\\d{1,2} \\+ \\d{1,2}\\]\\+4 = "
        u8"\\d{1,2}$");

    ei.group_id = 10001;
    ASSERT_TRUE(this->test_call(ei, ".rs2d20+4", result_reg_r1));

    ei.group_id = 10002;
    ASSERT_TRUE(this->test_call(ei, ".rs2d20+4", result_reg_r2));

    ei.group_id = 10001;
    ASSERT_TRUE(this->test_call(
        ei, ".ndice", std::regex(u8"^ \\* dice1 的新名字是 dice$")));

    ei.group_id = 10002;
    ASSERT_TRUE(this->test_call(
        ei, ".ndice", std::regex(u8"^ \\* dice2 的新名字是 dice$")));
}

TEST_F(entry_test, macro_recall) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";

    this->base_call(ei, ".ndice");
    ASSERT_TRUE(this->test_call(
        ei, ".s 4d6", std::regex(u8"^ \\* dice 设置默认骰子指令: \\(4d6\\)")));
    ASSERT_TRUE(this->test_call(
        ei,
        ".set 4d6",
        std::regex(u8"^ \\* dice 设置默认骰子指令: \\(4d6\\)")));

    std::regex result_reg(
        u8"^ \\* dice 掷骰: \\(4d6\\) = \\(\\[\\d \\+ \\d \\+ \\d \\+ "
        u8"\\d\\]\\) = \\d{1,2}$");
    std::regex result_reg_s(u8"^ \\* dice 掷骰: \\(4d6\\) = \\d{1,2}$");

    this->base_call(ei, ".rsoff");
    ASSERT_TRUE(this->test_call(ei, ".rs", result_reg));
    ASSERT_TRUE(this->test_call(ei, ".r", result_reg_s));

    ASSERT_TRUE(this->test_call(
        ei,
        ".s 4d6 test",
        std::regex(u8"^ \\* dice 设置指令: \\(4d6\\) 为 test")));
    ASSERT_TRUE(this->test_call(
        ei,
        ".set 4d6 test",
        std::regex(u8"^ \\* dice 设置指令: \\(4d6\\) 为 test")));

    this->base_call(ei, ".rsoff");
    ASSERT_TRUE(this->test_call(ei, ".rs test", result_reg));
    ASSERT_TRUE(this->test_call(ei, ".r test", result_reg_s));

    ASSERT_TRUE(this->test_call(
        ei, ".s 4 test", std::regex(u8"^ \\* dice 设置指令: 4 为 test")));

    std::regex result2_reg(
        u8"^ \\* dice 掷骰: 4d6 = \\[\\d \\+ \\d \\+ \\d \\+ \\d\\] = "
        u8"\\d{1,2}$");
    std::regex result2_reg_s(u8"^ \\* dice 掷骰: 4d6 = \\d{1,2}$");

    this->base_call(ei, ".rsoff");
    ASSERT_TRUE(this->test_call(ei, ".rs testd6", result2_reg));
    ASSERT_TRUE(this->test_call(ei, ".rs test d 6 ", result2_reg));
    ASSERT_TRUE(this->test_call(ei, ".r testd6", result2_reg_s));
    ASSERT_TRUE(this->test_call(ei, ".r test d 6", result2_reg_s));

    this->base_call(ei, ".s 4d6k3");
    ASSERT_TRUE(
        this->test_call(ei, ".l", std::regex(u8"\\n\\* 默认 : \\(4d6k3\\)")));
    this->base_call(ei, ".s 4d6kl3");
    ASSERT_TRUE(
        this->test_call(ei, ".l", std::regex(u8"\\n\\* 默认 : \\(4d6kl3\\)")));
    this->base_call(ei, ".s 4d6k3 str");
    ASSERT_TRUE(
        this->test_call(ei, ".l", std::regex(u8"\\n> str : \\(4d6k3\\)")));
    this->base_call(ei, ".s 4d6kl3 str");
    ASSERT_TRUE(
        this->test_call(ei, ".l", std::regex(u8"\\n> str : \\(4d6kl3\\)")));

    ASSERT_TRUE(this->test_call(
        ei, ".d str", std::regex(u8"^ \\* dice 已删除骰子指令: str")));
    ASSERT_TRUE(this->test_call(
        ei, ".d", std::regex(u8"^ \\* dice 已删除所有骰子指令")));

    this->base_call(ei, ".s 4d6k3 test1");
    this->base_call(ei, ".s 4d6k3 test2");
    this->base_call(ei, ".s 4d6k3 test3");
    this->base_call(ei, ".s 4d6k3 tess");
    ASSERT_TRUE(this->test_call(
        ei, ".l test", std::regex(u8"(\\r\\n> test\\d : \\(4d6k3\\)){3}$")));
}

TEST_F(entry_test, roll_coc) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";

    this->base_call(ei, ".ndice");
    ASSERT_TRUE(this->test_call(
        ei, ".c", std::regex(u8"^ \\* dice 掷骰: CoC = \\d{1,3}")));
    ASSERT_TRUE(this->test_call(
        ei, ".coc", std::regex(u8"^ \\* dice 掷骰: CoC = \\d{1,3}")));
    ASSERT_TRUE(this->test_call(
        ei, ".c test", std::regex(u8"^ \\* dice test 掷骰: CoC = \\d{1,3}")));
    ASSERT_TRUE(this->test_call(
        ei, ".coc test", std::regex(u8"^ \\* dice test 掷骰: CoC = \\d{1,3}")));

    std::string regex_prefix = u8" \\* dice 掷骰: CoC";
    std::string regex_suffix =
        u8" = \\[\\d{1,2}\\*? \\+ \\d{1,2}\\*?\\] \\[\\d\\] = \\d{1,3}";

    ASSERT_TRUE(this->test_call(
        ei, ".cb1", std::regex(regex_prefix + "b1" + regex_suffix)));
    ASSERT_TRUE(this->test_call(
        ei, ".cp1", std::regex(regex_prefix + "p1" + regex_suffix)));

    ASSERT_TRUE(this->test_call(
        ei, ".cb2p1", std::regex(regex_prefix + "b2p1" + regex_suffix)));
    ASSERT_TRUE(this->test_call(
        ei, ".cp2b1", std::regex(regex_prefix + "p2b1" + regex_suffix)));
}

TEST_F(entry_test, roll_wod) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";

    std::regex reg_wodo6(
        u8"^ \\* dice 掷骰: oWoD = \\[(\\d{1,2}\\*? \\+ ){5}\\d{1,2}\\*?\\] = "
        u8"\\d");
    std::regex reg_wodn6(
        u8"^ \\* dice 掷骰: nWoD = \\[(\\d{1,2}\\*? \\+ ){5,}\\d{1,2}\\*?\\] = "
        u8"\\d");

    std::regex reg_wodo6_msg(
        u8"^ \\* dice test 掷骰: oWoD = \\[(\\d{1,2}\\*? \\+ "
        u8"){5}\\d{1,2}\\*?\\] = \\d");
    std::regex reg_wodn6_msg(
        u8"^ \\* dice test 掷骰: nWoD = \\[(\\d{1,2}\\*? \\+ "
        u8"){5,}\\d{1,2}\\*?\\] = \\d");
    this->base_call(ei, ".ndice");
    ASSERT_TRUE(this->test_call(ei, ".wodo6", reg_wodo6));
    ASSERT_TRUE(this->test_call(ei, ".wodn6", reg_wodn6));
    ASSERT_TRUE(this->test_call(ei, ".wo6", reg_wodo6));
    ASSERT_TRUE(this->test_call(ei, ".wn6", reg_wodn6));
    ASSERT_TRUE(this->test_call(ei, ".wodo6 test", reg_wodo6_msg));
    ASSERT_TRUE(this->test_call(ei, ".wodn6test", reg_wodn6_msg));
    ASSERT_TRUE(this->test_call(ei, ".wo6 test", reg_wodo6_msg));
    ASSERT_TRUE(this->test_call(ei, ".wn6test", reg_wodn6_msg));
}

TEST_F(entry_test, roll_fate) {
    dicebot::event_info ei(123456, 10000, dicebot::event_type::group);
    ei.nickname = "dynilath";

    std::regex reg_fate(
        u8"^ \\* dice 掷骰: FATE = \\[([o+\\-] ){3}[o+\\-]\\] = -?\\d");
    std::regex reg_fate1(
        u8"^ \\* dice 掷骰: FATE = \\[([o+\\-] ){3}[o+\\-]\\] \\+ 1 = -?\\d");
    std::regex reg_fate_msg(
        u8"^ \\* dice test 掷骰: FATE = \\[([o+\\-] ){3}[o+\\-]\\] = -?\\d");
    std::regex reg_fate1_msg(
        u8"^ \\* dice test 掷骰: FATE = \\[([o+\\-] ){3}[o+\\-]\\] \\+ 1 = "
        u8"-?\\d");

    this->base_call(ei, ".ndice");
    ASSERT_TRUE(this->test_call(ei, ".f", reg_fate));
    ASSERT_TRUE(this->test_call(ei, ".f+1", reg_fate1));
    ASSERT_TRUE(this->test_call(ei, ".fate", reg_fate));
    ASSERT_TRUE(this->test_call(ei, ".fate+1", reg_fate1));
    ASSERT_TRUE(this->test_call(ei, ".ftest", reg_fate_msg));
    ASSERT_TRUE(this->test_call(ei, ".f+1test", reg_fate1_msg));
    ASSERT_TRUE(this->test_call(ei, ".fate test", reg_fate_msg));
    ASSERT_TRUE(this->test_call(ei, ".fate+1 test", reg_fate1_msg));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}